#include "Firefighter/recoStuff/interface/LeptonjetSourceDSAMuonProducer.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

LeptonjetSourceDSAMuonProducer::LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& ps )
    : fDSACandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfcandsFromMuondSAPtr" ) ) ),
      fPFMuonsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "leptonjetSourcePFMuon", "inclusive" ) ) ),
      fMinDTTimeDiff(ps.getParameter<double>("minDTTimeDiff")),
      fMinRPCTimeDiff(ps.getParameter<double>("minRPCTimeDiff")) {
  produces<reco::PFCandidateFwdPtrVector>( "inclusive" );
  produces<reco::PFCandidateFwdPtrVector>( "nonisolated" );
  produces<std::vector<float>>( "dtDT" );
  produces<std::vector<float>>( "dtRPC" );
}

LeptonjetSourceDSAMuonProducer::~LeptonjetSourceDSAMuonProducer() = default;

void
LeptonjetSourceDSAMuonProducer::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  fTkExtrapolator = std::make_unique<ff::TrackExtrapolator>( es );
}

void
LeptonjetSourceDSAMuonProducer::produce( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  auto inclusiveColl   = make_unique<reco::PFCandidateFwdPtrVector>();
  auto nonisolatedColl = make_unique<reco::PFCandidateFwdPtrVector>();
  auto dtDTColl        = make_unique<vector<float>>();
  auto dtRPCColl       = make_unique<vector<float>>();

  e.getByToken( fDSACandsToken, fDSACandsHdl );
  assert( fDSACandsHdl.isValid() );
  e.getByToken( fPFMuonsToken, fPFMuonsHdl );
  assert( fPFMuonsHdl.isValid() );

  // collect PFMuon DetIds
  vector<vector<DTChamberId>> pfmuonDTIds{};
  vector<vector<CSCDetId>>    pfmuonCSCIds{};
  for ( const auto& m : *fPFMuonsHdl ) {
    const auto& muonref = m.ptr()->muonRef();
    pfmuonDTIds.push_back( getDTDetIds( *muonref ) );
    pfmuonCSCIds.push_back( getCSCDetIds( *muonref ) );
  }

  for ( const auto& candfwdptr : *fDSACandsHdl ) {
    const auto& candptr  = candfwdptr.ptr();
    const auto& muonref  = candptr->muonRef();
    const auto& trackref = muonref->outerTrack();

    if ( candptr.isNull() or muonref.isNull() or trackref.isNull() )
      continue;

    const auto& hitpattern = trackref->hitPattern();

    //pre -id
    if ( ( hitpattern.cscStationsWithValidHits() + hitpattern.dtStationsWithValidHits() ) < 2 )
      continue;
    if ( ( hitpattern.numberOfValidMuonCSCHits() + hitpattern.numberOfValidMuonDTHits() ) < 13 )
      continue;
    if ( ( trackref->ptError() / trackref->pt() ) > 1 )
      continue;

    //loose iso. ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Particle_Flow_isolation
    const auto& pfiso04  = muonref->pfIsolationR04();
    double      iso04val = ( pfiso04.sumChargedHadronPt + max( 0., pfiso04.sumNeutralHadronEt + pfiso04.sumPhotonEt - 0.5 * pfiso04.sumPUPt ) ) / muonref->pt();
    if ( iso04val > 0.25 )  // 0.4, 0.25, 0.20, 0.15, 0.10, 0.05
      continue;

    //matching with loose PFMuon
    if ( maxSegmentOverlapRatio( *muonref ) > 0.66 )
      continue;
    if ( minDeltaRAtInnermostPoint( *muonref ) < 0.2 )
      continue;
    if ( detIdsIsSubSetOfAnyPFMuon( *trackref, pfmuonDTIds, pfmuonCSCIds ) )
      continue;

    //further -id
    if ( trackref->pt() < 10. )
      continue;
    if ( fabs( trackref->eta() ) > 2.4 )
      continue;
    if ( trackref->normalizedChi2() > 4 )
      continue;
    if ( hitpattern.numberOfValidMuonCSCHits() == 0 and hitpattern.numberOfValidMuonDTHits() <= 18 )
      continue;

    // reject cosmic-like
    reco::MuonRef oppositeMuon = findOppositeMuon( muonref );

    float deltaTDT  = timingDiffDT( muonref, oppositeMuon );
    float deltaTRPC = timingDiffRPC( muonref, oppositeMuon );
    if ( oppositeMuon.isNonnull() ) {
      dtDTColl->push_back( deltaTDT );
      dtRPCColl->push_back( deltaTRPC );
      if ( !( deltaTDT > fMinDTTimeDiff and deltaTRPC > fMinRPCTimeDiff ) )
        continue;
    }

    inclusiveColl->push_back( candfwdptr );
  }

  for ( const auto& candfwdptr : *inclusiveColl ) {
    for ( const auto& candfwdptr2 : *inclusiveColl ) {
      if ( candfwdptr.ptr() == candfwdptr2.ptr() )
        continue;
      if ( deltaR( *( candfwdptr.ptr() ), *( candfwdptr2.ptr() ) ) > 0.4 )
        continue;
      if ( ( candfwdptr.ptr()->charge() * candfwdptr2.ptr()->charge() ) != -1 )
        continue;
      nonisolatedColl->push_back( candfwdptr );
      break;
    }
  }

  e.put( move( inclusiveColl ), "inclusive" );
  e.put( move( nonisolatedColl ), "nonisolated" );
  e.put( move( dtDTColl ), "dtDT" );
  e.put( move( dtRPCColl ), "dtRPC" );
}

int
LeptonjetSourceDSAMuonProducer::numberOfSegments( const reco::Muon& muon,
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
LeptonjetSourceDSAMuonProducer::getSegmentOverlapRatioArbitration( const reco::Muon& dsamuon,
                                                                   const reco::Muon& recomuon,
                                                                   unsigned int      segmentArbitraionMask ) const {
  int dsasegments = numberOfSegments( dsamuon );
  if ( dsasegments == 0 )
    return 0.;

  int sharedsegments = muon::sharedSegments( dsamuon, recomuon, segmentArbitraionMask );
  return float( sharedsegments ) / dsasegments;
}

float
LeptonjetSourceDSAMuonProducer::maxSegmentOverlapRatio( const reco::Muon& dsamuon ) const {
  using namespace std;

  assert( fPFMuonsHdl.isValid() );
  vector<float> _segmentOverlapRatio( fPFMuonsHdl->size(), 0. );
  for ( size_t i( 0 ); i != fPFMuonsHdl->size(); i++ ) {
    const auto& muonref = ( *fPFMuonsHdl )[ i ].ptr()->muonRef();
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
LeptonjetSourceDSAMuonProducer::minDeltaRAtInnermostPoint( const reco::Muon& dsamuon ) const {
  using namespace std;

  assert( fPFMuonsHdl.isValid() );
  assert( dsamuon.outerTrack().isNonnull() );

  vector<float> _deltaRAtInnermostPoint( fPFMuonsHdl->size(), 999. );
  for ( size_t i( 0 ); i != fPFMuonsHdl->size(); i++ ) {
    const auto& muonref = ( *fPFMuonsHdl )[ i ].ptr()->muonRef();

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

std::vector<DTChamberId>
LeptonjetSourceDSAMuonProducer::getDTDetIds( const reco::Muon& muon ) const {
  using namespace std;
  vector<DTChamberId> res{};

  for ( const auto& chambermatch : muon.matches() ) {
    if ( chambermatch.detector() == MuonSubdetId::DT ) {
      DTChamberId dtId( chambermatch.id.rawId() );

      if ( find( res.begin(), res.end(), dtId ) == res.end() )
        res.push_back( dtId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<DTChamberId>
LeptonjetSourceDSAMuonProducer::getDTDetIds( const reco::Track& track ) const {
  using namespace std;
  vector<DTChamberId> res{};

  for ( auto hitIter = track.recHitsBegin(); hitIter != track.recHitsEnd(); ++hitIter ) {
    if ( !( *hitIter )->isValid() )
      continue;
    const DetId id = ( *hitIter )->geographicalId();
    if ( id.det() != DetId::Muon )
      continue;

    if ( id.subdetId() == MuonSubdetId::DT ) {
      DTChamberId dtId( id.rawId() );

      if ( find( res.begin(), res.end(), dtId ) == res.end() )
        res.push_back( dtId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<CSCDetId>
LeptonjetSourceDSAMuonProducer::getCSCDetIds( const reco::Muon& muon ) const {
  using namespace std;
  vector<CSCDetId> res{};

  for ( const auto& chambermatch : muon.matches() ) {
    if ( chambermatch.detector() == MuonSubdetId::CSC ) {
      CSCDetId cscId( chambermatch.id.rawId() );

      if ( find( res.begin(), res.end(), cscId ) == res.end() )
        res.push_back( cscId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

std::vector<CSCDetId>
LeptonjetSourceDSAMuonProducer::getCSCDetIds( const reco::Track& track ) const {
  using namespace std;
  vector<CSCDetId> res{};

  for ( auto hitIter = track.recHitsBegin(); hitIter != track.recHitsEnd(); ++hitIter ) {
    if ( !( *hitIter )->isValid() )
      continue;
    const DetId id = ( *hitIter )->geographicalId();
    if ( id.det() != DetId::Muon )
      continue;

    if ( id.subdetId() == MuonSubdetId::CSC ) {
      CSCDetId cscId( id.rawId() );

      if ( find( res.begin(), res.end(), cscId ) == res.end() )
        res.push_back( cscId );
    }
  }

  sort( res.begin(), res.end() );
  return res;
}

bool
LeptonjetSourceDSAMuonProducer::detIdsIsSubSetOfAnyPFMuon( const reco::Track&                           track,
                                                           const std::vector<std::vector<DTChamberId>>& dtids,
                                                           const std::vector<std::vector<CSCDetId>>&    cscids ) const {
  using namespace std;
  bool isSubsetPFMuon( false );

  vector<DTChamberId> _dtDetId  = getDTDetIds( track );
  vector<CSCDetId>    _cscDetId = getCSCDetIds( track );

  for ( pair<vector<vector<DTChamberId>>::const_iterator, vector<vector<CSCDetId>>::const_iterator> iter( dtids.begin(), cscids.begin() );
        iter.first != dtids.end() && iter.second != cscids.end();
        ++iter.first, ++iter.second ) {
    bool isDTSubsetPFMuon  = includes( iter.first->begin(), iter.first->end(), _dtDetId.begin(), _dtDetId.end() );
    bool isCSCSubsetPFMuon = includes( iter.second->begin(), iter.second->end(), _cscDetId.begin(), _cscDetId.end() );

    isSubsetPFMuon = isDTSubsetPFMuon && isCSCSubsetPFMuon;
    if ( isSubsetPFMuon )
      break;
  }

  return isSubsetPFMuon;
}

reco::MuonRef
LeptonjetSourceDSAMuonProducer::findOppositeMuon( const reco::MuonRef& muon ) const {
  assert( fDSACandsHdl.isValid() );

  reco::MuonRef res = reco::MuonRef();
  for ( const auto& dsacand : *fDSACandsHdl ) {
    const auto& candptr   = dsacand.ptr();
    const auto& probemuon = candptr->muonRef();
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
LeptonjetSourceDSAMuonProducer::timingDiffDT( const reco::MuonRef& mu0, const reco::MuonRef& mu1 ) const {
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
LeptonjetSourceDSAMuonProducer::timingDiffRPC( const reco::MuonRef& mu0, const reco::MuonRef& mu1 ) const {
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

DEFINE_FWK_MODULE( LeptonjetSourceDSAMuonProducer );
