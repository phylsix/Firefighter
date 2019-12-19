#include "Firefighter/recoStuff/interface/DSAMuonValueMapProducer.h"

#include <algorithm>

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

DSAMuonValueMapProducer::DSAMuonValueMapProducer( const edm::ParameterSet& ps )
    : fDsaMuonToken( consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fPFMuonToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "leptonjetSourcePFMuon", "inclusive" ) ) ),
      fCosmic1LegToken( consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "cosmic" ) ) ),
      fPvToken( consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) ) ),
      fCosmicMatchCutPars( ps.getParameterSet( "cosmicMatchCut" ) ) {
  produces<edm::ValueMap<float>>( "maxSegmentOverlapRatio" );
  produces<edm::ValueMap<float>>( "minExtrapolateInnermostLocalDr" );
  produces<edm::ValueMap<bool>>( "isDetIdSubsetOfAnyPFMuon" );
  produces<edm::ValueMap<float>>( "pfiso04" );
  produces<edm::ValueMap<reco::MuonRef>>( "oppositeMuon" );
  produces<edm::ValueMap<float>>( "dTUpperMinusLowerDTCSC" );
  produces<edm::ValueMap<float>>( "dTUpperMinusLowerRPC" );
  produces<edm::ValueMap<bool>>( "isDetIdSubsetOfFilteredCosmic1Leg" );
}

DSAMuonValueMapProducer::~DSAMuonValueMapProducer() = default;

void
DSAMuonValueMapProducer::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  fTkExtrapolator = std::make_unique<ff::TrackExtrapolator>( es );
}

void
DSAMuonValueMapProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto vm_maxSegmentOverlapRatio         = make_unique<ValueMap<float>>();
  auto vm_minExtrapolateInnermostLocalDr = make_unique<ValueMap<float>>();
  auto vm_isDetIdSubsetOfAnyPFMuon       = make_unique<ValueMap<bool>>();
  auto vm_pfiso04                        = make_unique<ValueMap<float>>();
  auto vm_oppositeMuon                   = make_unique<ValueMap<reco::MuonRef>>();
  auto vm_timediffDTCSC                  = make_unique<ValueMap<float>>();
  auto vm_timediffRPC                    = make_unique<ValueMap<float>>();
  auto vm_isDetIdSubsetOfAnyCosmic1Leg   = make_unique<ValueMap<bool>>();

  e.getByToken( fDsaMuonToken, fDsaMuonHdl );
  assert( fDsaMuonHdl.isValid() );
  e.getByToken( fPFMuonToken, fPFMuonHdl );
  assert( fPFMuonHdl.isValid() );
  e.getByToken( fCosmic1LegToken, fCosmic1LegHdl );
  assert( fCosmic1LegHdl.isValid() );
  e.getByToken( fPvToken, fPvHdl );
  assert( fPvHdl.isValid() && fPvHdl->size() > 0 );
  const auto& pv = *( fPvHdl->begin() );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<CSCGeometry> cscG;
  ESHandle<DTGeometry>  dtG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );

  // collect PFMuon DetIds
  vector<vector<DTChamberId>> pfmuonDTIds{};
  vector<vector<CSCDetId>>    pfmuonCSCIds{};
  for ( const auto& m : *fPFMuonHdl ) {
    const auto& muonref = m.ptr()->muonRef();
    pfmuonDTIds.push_back( DSAMuonHelper::getDTDetIds( *muonref ) );
    pfmuonCSCIds.push_back( DSAMuonHelper::getCSCDetIds( *muonref ) );
  }

  // collect filtered Cosmic1Leg DetIds
  vector<vector<DetId>> cosmic1legDetIds{};
  for ( size_t i( 0 ); i != fCosmic1LegHdl->size(); i++ ) {
    reco::TrackRef cosmicRef( fCosmic1LegHdl, i );
    const auto&    cosmic = *cosmicRef;

    if ( cosmic.pt() < fCosmicMatchCutPars.getParameter<double>( "minPt" ) ) continue;
    if ( cosmic.normalizedChi2() > fCosmicMatchCutPars.getParameter<double>( "maxNormChi2" ) ) continue;

    int           dtT( 0 ), dtB( 0 ), cscT( 0 ), cscB( 0 );
    vector<DetId> chamberId{};
    for ( auto ih = cosmic.recHitsBegin(); ih != cosmic.recHitsEnd(); ih++ ) {
      const auto& hit = *( *ih );
      if ( !hit.isValid() ) continue;
      if ( hit.geographicalId().det() != DetId::Muon ) continue;
      if ( hit.geographicalId().subdetId() == MuonSubdetId::DT ) {
        const DTChamber* dtchamber = dtG->chamber( hit.geographicalId() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( dtchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( dtchamber->id().rawId() );
          if ( dtchamber->position().y() > 0 ) dtT++;
          if ( dtchamber->position().y() < 0 ) dtB++;
        }
      } else if ( hit.geographicalId().subdetId() == MuonSubdetId::CSC ) {
        const CSCChamber* cscchamber = cscG->chamber( hit.geographicalId() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( cscchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( cscchamber->id().rawId() );
          if ( cscchamber->position().y() > 0 ) cscT++;
          if ( cscchamber->position().y() < 0 ) cscB++;
        }
      }
    }

    if ( ( dtT + cscT ) < fCosmicMatchCutPars.getParameter<int>( "minNumChamberTop" ) ) continue;
    if ( ( dtB + cscB ) < fCosmicMatchCutPars.getParameter<int>( "minNumChamberBottom" ) ) continue;

    GlobalPoint              pvpos( pv.x(), pv.y(), pv.z() );
    TrajectoryStateOnSurface tsos = TransverseImpactPointExtrapolator( bField ).extrapolate(
        trajectoryStateTransform::initialFreeState( cosmic, bField ), pvpos );
    float impact2d( -999. );
    if ( tsos.isValid() ) impact2d = hypot( tsos.localPosition().x(), tsos.localPosition().y() );

    if ( impact2d < fCosmicMatchCutPars.getParameter<double>( "minImpactDist2D" ) ) continue;

    sort( chamberId.begin(), chamberId.end() );
    cosmic1legDetIds.push_back( chamberId );
  }
  if ( cosmic1legDetIds.size() < fCosmicMatchCutPars.getParameter<unsigned int>( "minCount" ) )
    cosmic1legDetIds.clear();

  vector<float>         v_maxSegmentOverlapRatio( fDsaMuonHdl->size(), 0. );
  vector<float>         v_minExtrapolateInnermostLocalDr( fDsaMuonHdl->size(), 999. );
  vector<bool>          v_isDetIdSubsetOfAnyPFMuon( fDsaMuonHdl->size(), true );
  vector<float>         v_pfiso04( fDsaMuonHdl->size(), 999. );
  vector<reco::MuonRef> v_oppositeMuon( fDsaMuonHdl->size(), reco::MuonRef() );
  vector<float>         v_timediffDTCSC( fDsaMuonHdl->size(), -999. );
  vector<float>         v_timediffRPC( fDsaMuonHdl->size(), -999. );
  vector<bool>          v_isDetIdSubsetOfAnyCosmic1Leg( fDsaMuonHdl->size(), false );

  for ( size_t i( 0 ); i != fDsaMuonHdl->size(); i++ ) {
    reco::MuonRef dsamuonref( fDsaMuonHdl, i );
    const auto&   dsamuon = ( *fDsaMuonHdl )[ i ];

    v_maxSegmentOverlapRatio[ i ]         = maxSegmentOverlapRatio( dsamuon );
    v_minExtrapolateInnermostLocalDr[ i ] = minDeltaRAtInnermostPoint( dsamuon );
    v_isDetIdSubsetOfAnyPFMuon[ i ]       = DSAMuonHelper::detIdsIsSubSetOfDTCSCIds( *dsamuon.outerTrack(), pfmuonDTIds, pfmuonCSCIds, es );
    v_pfiso04[ i ]                        = ff::getMuonIsolationValue( dsamuon );
    v_oppositeMuon[ i ]                   = findOppositeMuon( dsamuonref );
    v_timediffDTCSC[ i ]                  = timingDiffDT( dsamuonref, v_oppositeMuon[ i ] );
    v_timediffRPC[ i ]                    = timingDiffRPC( dsamuonref, v_oppositeMuon[ i ] );
    v_isDetIdSubsetOfAnyCosmic1Leg[ i ]   = DSAMuonHelper::detIdsIsSubSetOfVDetIds( *dsamuon.outerTrack(), cosmic1legDetIds, es );
  }

  ValueMap<float>::Filler ratioFiller( *vm_maxSegmentOverlapRatio );
  ratioFiller.insert( fDsaMuonHdl, v_maxSegmentOverlapRatio.begin(), v_maxSegmentOverlapRatio.end() );
  ratioFiller.fill();
  e.put( move( vm_maxSegmentOverlapRatio ), "maxSegmentOverlapRatio" );

  ValueMap<float>::Filler localDrFiller( *vm_minExtrapolateInnermostLocalDr );
  localDrFiller.insert( fDsaMuonHdl, v_minExtrapolateInnermostLocalDr.begin(), v_minExtrapolateInnermostLocalDr.end() );
  localDrFiller.fill();
  e.put( move( vm_minExtrapolateInnermostLocalDr ), "minExtrapolateInnermostLocalDr" );

  ValueMap<bool>::Filler isDetIdSubsetFiller( *vm_isDetIdSubsetOfAnyPFMuon );
  isDetIdSubsetFiller.insert( fDsaMuonHdl, v_isDetIdSubsetOfAnyPFMuon.begin(), v_isDetIdSubsetOfAnyPFMuon.end() );
  isDetIdSubsetFiller.fill();
  e.put( move( vm_isDetIdSubsetOfAnyPFMuon ), "isDetIdSubsetOfAnyPFMuon" );

  ValueMap<float>::Filler pfIsoValFiller( *vm_pfiso04 );
  pfIsoValFiller.insert( fDsaMuonHdl, v_pfiso04.begin(), v_pfiso04.end() );
  pfIsoValFiller.fill();
  e.put( move( vm_pfiso04 ), "pfiso04" );

  ValueMap<reco::MuonRef>::Filler oppositeMuonFiller( *vm_oppositeMuon );
  oppositeMuonFiller.insert( fDsaMuonHdl, v_oppositeMuon.begin(), v_oppositeMuon.end() );
  oppositeMuonFiller.fill();
  e.put( move( vm_oppositeMuon ), "oppositeMuon" );

  ValueMap<float>::Filler timediffDTCSCFiller( *vm_timediffDTCSC );
  timediffDTCSCFiller.insert( fDsaMuonHdl, v_timediffDTCSC.begin(), v_timediffDTCSC.end() );
  timediffDTCSCFiller.fill();
  e.put( move( vm_timediffDTCSC ), "dTUpperMinusLowerDTCSC" );

  ValueMap<float>::Filler timediffRPCFiller( *vm_timediffRPC );
  timediffRPCFiller.insert( fDsaMuonHdl, v_timediffRPC.begin(), v_timediffRPC.end() );
  timediffRPCFiller.fill();
  e.put( move( vm_timediffRPC ), "dTUpperMinusLowerRPC" );

  ValueMap<bool>::Filler isDetIdSubsetCosmicFiller( *vm_isDetIdSubsetOfAnyCosmic1Leg );
  isDetIdSubsetCosmicFiller.insert( fDsaMuonHdl, v_isDetIdSubsetOfAnyCosmic1Leg.begin(), v_isDetIdSubsetOfAnyCosmic1Leg.end() );
  isDetIdSubsetCosmicFiller.fill();
  e.put( move( vm_isDetIdSubsetOfAnyCosmic1Leg ), "isDetIdSubsetOfFilteredCosmic1Leg" );
}

int
DSAMuonValueMapProducer::numberOfSegments( const reco::Muon& muon,
                                           unsigned int      segmentArbitraionMask ) const {
  int segments( 0 );

  for ( const auto& chambermatch : muon.matches() ) {
    if ( chambermatch.segmentMatches.empty() )
      continue;
    for ( const auto& seg : chambermatch.segmentMatches ) {
      if ( !seg.isMask( segmentArbitraionMask ) )
        continue;
      ++segments;
    }
  }

  return segments;
}

float
DSAMuonValueMapProducer::getSegmentOverlapRatioArbitration( const reco::Muon& dsamuon,
                                                            const reco::Muon& recomuon,
                                                            unsigned int      segmentArbitraionMask ) const {
  int dsasegments = numberOfSegments( dsamuon );
  if ( dsasegments == 0 )
    return 0.;

  int sharedsegments = muon::sharedSegments( dsamuon, recomuon, segmentArbitraionMask );
  return float( sharedsegments ) / dsasegments;
}

float
DSAMuonValueMapProducer::maxSegmentOverlapRatio( const reco::Muon& dsamuon ) const {
  using namespace std;

  assert( fPFMuonHdl.isValid() );
  vector<float> _segmentOverlapRatio( fPFMuonHdl->size(), 0. );
  for ( size_t i( 0 ); i != fPFMuonHdl->size(); i++ ) {
    const auto& muonref = ( *fPFMuonHdl )[ i ].ptr()->muonRef();
    if ( muonref.isNull() or muonref->outerTrack().isNull() )
      continue;
    _segmentOverlapRatio[ i ] = getSegmentOverlapRatioArbitration( dsamuon, *muonref );
  }

  float res( 0. );
  if ( !_segmentOverlapRatio.empty() )
    res = *max_element( _segmentOverlapRatio.begin(), _segmentOverlapRatio.end() );

  return res;
}

float
DSAMuonValueMapProducer::minDeltaRAtInnermostPoint( const reco::Muon& dsamuon ) const {
  using namespace std;

  assert( fPFMuonHdl.isValid() );
  assert( dsamuon.outerTrack().isNonnull() );

  vector<float> _deltaRAtInnermostPoint( fPFMuonHdl->size(), 999. );
  for ( size_t i( 0 ); i != fPFMuonHdl->size(); i++ ) {
    const auto& muonref = ( *fPFMuonHdl )[ i ].ptr()->muonRef();

    if ( muonref->innerTrack().isNull() )
      continue;
    // same number of muon hits to reduce the rate of accidental matches
    // Sep.25,19 wsi not added for now
    // if ( dsamuon.outerTrack()->hitPattern().numberOfValidMuonHits() != muonref->bestTrack()->hitPattern().numberOfValidMuonHits() )
    //   continue;

    const reco::TrackRef innerTk = muonref->innerTrack();
    FreeTrajectoryState  startFTS =
        trajectoryStateTransform::initialFreeState( *innerTk,
                                                    fTkExtrapolator->getMagneticField() );

    const reco::TrackRef     dsaTk = dsamuon.outerTrack();
    TrajectoryStateOnSurface targetInner =
        trajectoryStateTransform::innerStateOnSurface( *dsaTk,
                                                       *( fTkExtrapolator->getTrackingGeometry() ),
                                                       fTkExtrapolator->getMagneticField() );
    if ( !targetInner.isValid() )
      continue;

    TrajectoryStateOnSurface tsosInner = fTkExtrapolator->propagate( startFTS,
                                                                     targetInner.surface() );
    if ( !tsosInner.isValid() )
      continue;

    _deltaRAtInnermostPoint[ i ] = deltaR( tsosInner.localMomentum(), targetInner.localMomentum() );
  }

  float res( 999. );
  if ( !_deltaRAtInnermostPoint.empty() )
    res = *min_element( _deltaRAtInnermostPoint.begin(), _deltaRAtInnermostPoint.end() );

  return res;
}

reco::MuonRef
DSAMuonValueMapProducer::findOppositeMuon( const reco::MuonRef& muon ) const {
  assert( fDsaMuonHdl.isValid() );

  reco::MuonRef res = reco::MuonRef();
  for ( size_t i( 0 ); i != fDsaMuonHdl->size(); i++ ) {
    reco::MuonRef probemuon( fDsaMuonHdl, i );
    if ( muon == probemuon )
      continue;

    double cosalpha = muon->momentum().Dot( probemuon->momentum() );
    cosalpha /= muon->momentum().R() * probemuon->momentum().R();
    if ( cosalpha < -0.99 )
      res = probemuon;
    if ( res.isNonnull() )
      break;
  }

  return res;
}

float
DSAMuonValueMapProducer::timingDiffDT( const reco::MuonRef& mu0, const reco::MuonRef& mu1 ) const {
  float res = -999.;
  if ( mu0.isNull() or mu1.isNull() )
    return res;

  float mu0time = mu0->time().timeAtIpInOut;
  float mu1time = mu1->time().timeAtIpInOut;

  if ( mu0->outerTrack()->outerY() > 0 and mu1->outerTrack()->outerY() < 0 )
    res = mu1time - mu0time;
  if ( mu0->outerTrack()->outerY() < 0 and mu1->outerTrack()->outerY() > 0 )
    res = mu0time - mu1time;

  return res;
}

float
DSAMuonValueMapProducer::timingDiffRPC( const reco::MuonRef& mu0, const reco::MuonRef& mu1 ) const {
  float res = -999.;
  if ( mu0.isNull() or mu1.isNull() )
    return res;

  float mu0time = mu0->rpcTime().timeAtIpInOut;
  float mu1time = mu1->rpcTime().timeAtIpInOut;

  if ( mu0->outerTrack()->outerY() >= 0 and mu1->outerTrack()->outerY() < 0 )
    res = mu0time - mu1time;
  if ( mu0->outerTrack()->outerY() < 0 and mu1->outerTrack()->outerY() >= 0 )
    res = mu1time - mu0time;

  return res;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( DSAMuonValueMapProducer );