#include "Firefighter/recoStuff/interface/DSAMuonValueMapProducer.h"

#include <algorithm>

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "Firefighter/objects/interface/DSAExtra.h"
#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"
#include "Firefighter/cosmics/interface/CosmicHelper.h"
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
      fDTSegToken( consumes<DTRecSegment4DCollection>( edm::InputTag( "dt4DSegments" ) ) ),
      fCSCSegToken( consumes<CSCSegmentCollection>( edm::InputTag( "cscSegments" ) ) ),
      fCosmicMatchCutPars( ps.getParameterSet( "cosmicMatchCut" ) ) {
  produces<edm::ValueMap<DSAExtra>>();
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

  auto vm_DSAExtra = make_unique<ValueMap<DSAExtra>>();

  e.getByToken( fDsaMuonToken, fDsaMuonHdl );
  assert( fDsaMuonHdl.isValid() );
  e.getByToken( fPFMuonToken, fPFMuonHdl );
  assert( fPFMuonHdl.isValid() );
  e.getByToken( fCosmic1LegToken, fCosmic1LegHdl );
  assert( fCosmic1LegHdl.isValid() );
  e.getByToken( fPvToken, fPvHdl );
  assert( fPvHdl.isValid() && fPvHdl->size() > 0 );
  const auto& pv = *( fPvHdl->begin() );
  e.getByToken( fDTSegToken, fDTSegHdl );
  assert( fDTSegHdl.isValid() );
  e.getByToken( fCSCSegToken, fCSCSegHdl );
  assert( fCSCSegHdl.isValid() );


  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<CSCGeometry> cscG;
  ESHandle<DTGeometry>  dtG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );

  // *** collect PFMuon DetIds ***
  vector<vector<DTChamberId>> pfmuonDTIds{};
  vector<vector<CSCDetId>>    pfmuonCSCIds{};
  for ( const auto& m : *fPFMuonHdl ) {
    const auto& muonref = m.ptr()->muonRef();
    pfmuonDTIds.push_back( DSAMuonHelper::getDTDetIds( *muonref ) );
    pfmuonCSCIds.push_back( DSAMuonHelper::getCSCDetIds( *muonref ) );
  }

  // *** collect filtered Cosmic1Leg DetIds ***
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

  // *** loop over DSAs to set attributes ***
  vector<DSAExtra> v_DSAExtra( fDsaMuonHdl->size() );

  for ( size_t i( 0 ); i != fDsaMuonHdl->size(); i++ ) {
    reco::MuonRef dsamuonref( fDsaMuonHdl, i );
    const reco::Muon&   dsamuon = ( *fDsaMuonHdl )[ i ];
    const reco::Track& iDSA = *(dsamuon.outerTrack());

    auto& dsaextra_ = v_DSAExtra[ i ];
    dsaextra_.set_pfmuon_maxSegmentOverlapRatio( maxSegmentOverlapRatio( dsamuon ) );
    dsaextra_.set_pfmuon_minLocalDeltaRAtInnermost( minDeltaRAtInnermostPoint( dsamuon ) );
    dsaextra_.set_pfmuon_detIdSubsetOfAny( DSAMuonHelper::detIdsIsSubSetOfDTCSCIds( iDSA, pfmuonDTIds, pfmuonCSCIds, es ) );
    dsaextra_.set_pfiso04( ff::getMuonIsolationValue( dsamuon ) );
    dsaextra_.set_oppositeMuon( findOppositeMuon( dsamuonref ) );
    dsaextra_.set_oppositeTimeDiffDtcsc( timingDiffDT( dsamuonref, dsaextra_.oppositeMuon() ) );
    dsaextra_.set_oppositeTimeDiffRpc( timingDiffRPC( dsamuonref, dsaextra_.oppositeMuon() ) );
    dsaextra_.set_cosmci1leg_detIdSubsetOfFiltered( DSAMuonHelper::detIdsIsSubSetOfVDetIds( iDSA, cosmic1legDetIds, es ) );

    // **** find min dR_cosmic w/ opposite hemisphere DSAs
    reco::TrackRef minCosmicDrDSA;
    double         minCosmicDr( 999. );

    for ( size_t j( 0 ); j != fDsaMuonHdl->size(); j++ ) {
      const reco::Muon& jDSAmuon = ( *fDsaMuonHdl )[ j ];
      const reco::Track& jDSA = *(jDSAmuon.outerTrack());

      if ( !CosmicHelper::oppositeHemisphere( iDSA, jDSA ) ) continue;

      double cosmicDr_ = CosmicHelper::cosmicDeltaR( iDSA, jDSA );
      if ( cosmicDr_ < minCosmicDr ) {
        minCosmicDr    = cosmicDr_;
        minCosmicDrDSA = jDSAmuon.outerTrack();
      }
    }

    double i_etaSum( 999 ), i_phiPiDiff( 999 );
    if ( minCosmicDrDSA.isNonnull() ) {
      const reco::Track& iiDSA = *minCosmicDrDSA;
      i_etaSum          = fabs( iDSA.eta() + iiDSA.eta() );
      i_phiPiDiff       = M_PI - fabs( deltaPhi( iDSA.phi(), iiDSA.phi() ) );
    }

    dsaextra_.set_dsamuon_minDeltaRCosmic( minCosmicDr );
    dsaextra_.set_dsamuon_minDeltaRCosmicEtasum(i_etaSum);
    dsaextra_.set_dsamuon_minDeltaRCosmicPhipidiff(i_phiPiDiff);
    //_________________________________________________________


    // **** find min dR_cosmic w/ opposite hemisphere segments
    vector<pair<double, double>> segdist{};  // <etasum, phidiff>
    for ( const DTRecSegment4D& dtSeg : *fDTSegHdl ) {
      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, dtSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoDT( iDSA, dtSeg, es );
      if ( !propagationResult.first ) continue;
      const auto& dsaGlbDir   = get<1>( propagationResult.second );
      const auto& dtsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( dsaGlbDir.eta() + dtsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( dsaGlbDir.barePhi(), dtsegGlbDir.barePhi() ) );

      segdist.emplace_back( etaSum, phiPiDiff );
    }
    for ( const CSCSegment& cscSeg : *fCSCSegHdl ) {
      // not from same hemisphere
      if ( !CosmicHelper::oppositeHemisphere( iDSA, cscSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoCSC( iDSA, cscSeg, es );

      if ( !propagationResult.first ) continue;
      const auto& dsaGlbDir    = get<1>( propagationResult.second );
      const auto& cscsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( dsaGlbDir.eta() + cscsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( dsaGlbDir.barePhi(), cscsegGlbDir.barePhi() ) );

      segdist.emplace_back( etaSum, phiPiDiff );
    }
    // sort by cosmicDr, small->large
    sort( segdist.begin(), segdist.end(), []( const auto& lhs, const auto& rhs ) {
      return hypot( lhs.first, lhs.second ) < hypot( rhs.first, rhs.second );
    } );

    double s_etaSum( 999. ), s_phiPiDiff( 999. );
    if ( segdist.size() > 0 ) {
      s_etaSum    = segdist[ 0 ].first;
      s_phiPiDiff = segdist[ 0 ].second;
    }
    dsaextra_.set_segment_minDeltaRCosmic( hypot( s_etaSum, s_phiPiDiff ) );
    dsaextra_.set_segment_minDeltaRCosmicEtasum(s_etaSum);
    dsaextra_.set_segment_minDeltaRCosmicPhipidiff(s_phiPiDiff);
    //_________________________________________________________

  }

  ValueMap<DSAExtra>::Filler DSAExtraFiller( *vm_DSAExtra );
  DSAExtraFiller.insert( fDsaMuonHdl, v_DSAExtra.begin(), v_DSAExtra.end() );
  DSAExtraFiller.fill();

  e.put( move( vm_DSAExtra ) );
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