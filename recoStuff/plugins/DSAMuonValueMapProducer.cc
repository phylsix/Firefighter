#include "Firefighter/recoStuff/interface/DSAMuonValueMapProducer.h"

#include <algorithm>

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

DSAMuonValueMapProducer::DSAMuonValueMapProducer( const edm::ParameterSet& ps )
    : fDsaMuonToken( consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) ) ),
      fPFMuonToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "leptonjetSourcePFMuon", "inclusive" ) ) ),
      fCosmic1LegToken( consumes<reco::TrackCollection>( edm::InputTag( "cosmicMuons1Leg" ) ) ),
      fCosmicMatchVMToken( consumes<edm::ValueMap<std::vector<reco::MuonChamberMatch>>>( edm::InputTag( "cosmicMuon1LegChamberMatch" ) ) ),
      fPvToken( consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) ) ),
      fCosmicMatchCutPars( ps.getParameterSet( "cosmicMatchCut" ) ) {
  produces<edm::ValueMap<float>>( "maxSegmentOverlapRatio" );
  produces<edm::ValueMap<float>>( "minExtrapolateInnermostLocalDr" );
  produces<edm::ValueMap<bool>>( "isDetIdSubsetOfAnyPFMuon" );
  produces<edm::ValueMap<float>>( "pfiso04" );
  produces<edm::ValueMap<reco::MuonRef>>( "oppositeMuon" );
  produces<edm::ValueMap<float>>( "dTUpperMinusLowerDTCSC" );
  produces<edm::ValueMap<float>>( "dTUpperMinusLowerRPC" );
  produces<edm::ValueMap<bool>>( "isSegemntSubsetOfFilteredCosmic1Leg" );
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
  auto vm_isSegmentSubsetOfAnyCosmic1Leg = make_unique<ValueMap<bool>>();

  e.getByToken( fDsaMuonToken, fDsaMuonHdl );
  assert( fDsaMuonHdl.isValid() );
  e.getByToken( fPFMuonToken, fPFMuonHdl );
  assert( fPFMuonHdl.isValid() );
  e.getByToken( fCosmic1LegToken, fCosmic1LegHdl );
  assert( fCosmic1LegHdl.isValid() );
  e.getByToken( fCosmicMatchVMToken, fCosmicMatchVMHdl );
  assert( fCosmicMatchVMHdl.isValid() );
  e.getByToken( fPvToken, fPvHdl );
  assert( fPvHdl.isValid() && fPvHdl->size() > 0 );
  const auto& pv = *( fPvHdl->begin() );

  // collect PFMuon DetIds
  vector<vector<DTChamberId>> pfmuonDTIds{};
  vector<vector<CSCDetId>>    pfmuonCSCIds{};
  for ( const auto& m : *fPFMuonHdl ) {
    const auto& muonref = m.ptr()->muonRef();
    pfmuonDTIds.push_back( DSAMuonHelper::getDTDetIds( *muonref ) );
    pfmuonCSCIds.push_back( DSAMuonHelper::getCSCDetIds( *muonref ) );
  }

  // collect cosmicMuon1Leg DT/CSC segments
  vector<vector<DTRecSegment4DRef>> cosmic1legDTSegs{};
  vector<vector<CSCSegmentRef>>     cosmic1legCSCSegs{};
  for ( size_t i( 0 ); i != fCosmic1LegHdl->size(); i++ ) {
    reco::TrackRef                 cosmic( fCosmic1LegHdl, i );
    const auto&                    chambermatches = ( *fCosmicMatchVMHdl )[ cosmic ];
    std::vector<DTRecSegment4DRef> dtsegs         = DSAMuonHelper::getDTSegments( chambermatches );
    std::vector<CSCSegmentRef>     cscsegs        = DSAMuonHelper::getCSCSegements( chambermatches );

    if ( cosmic->pt() < fCosmicMatchCutPars.getParameter<double>( "minpt" ) ) continue;
    if ( fCosmicMatchCutPars.getParameter<bool>( "requireDT" ) && dtsegs.size() == 0 ) continue;
    if ( fabs( cosmic->dz( pv.position() ) ) < fCosmicMatchCutPars.getParameter<double>( "mindz" ) ) continue;

    cosmic1legDTSegs.push_back( dtsegs );
    cosmic1legCSCSegs.push_back( cscsegs );
  }

  vector<float>         v_maxSegmentOverlapRatio( fDsaMuonHdl->size(), 0. );
  vector<float>         v_minExtrapolateInnermostLocalDr( fDsaMuonHdl->size(), 999. );
  vector<bool>          v_isDetIdSubsetOfAnyPFMuon( fDsaMuonHdl->size(), true );
  vector<float>         v_pfiso04( fDsaMuonHdl->size(), 999. );
  vector<reco::MuonRef> v_oppositeMuon( fDsaMuonHdl->size(), reco::MuonRef() );
  vector<float>         v_timediffDTCSC( fDsaMuonHdl->size(), -999. );
  vector<float>         v_timediffRPC( fDsaMuonHdl->size(), -999. );
  vector<bool>          v_isSegmentSubsetOfAnyCosmic1Leg( fDsaMuonHdl->size(), false );

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
    v_isSegmentSubsetOfAnyCosmic1Leg[ i ] = DSAMuonHelper::segmentsIsSubsetOfDTCSCSegs( dsamuon, cosmic1legDTSegs, cosmic1legCSCSegs );
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

  ValueMap<bool>::Filler isSegmentSubsetFiller( *vm_isSegmentSubsetOfAnyCosmic1Leg );
  isSegmentSubsetFiller.insert( fDsaMuonHdl, v_isSegmentSubsetOfAnyCosmic1Leg.begin(), v_isSegmentSubsetOfAnyCosmic1Leg.end() );
  isSegmentSubsetFiller.fill();
  e.put( move( vm_isSegmentSubsetOfAnyCosmic1Leg ), "isSegemntSubsetOfFilteredCosmic1Leg" );
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