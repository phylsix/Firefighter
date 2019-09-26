#include "Firefighter/recoStuff/interface/LeptonjetSourceDSAMuonProducer.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

LeptonjetSourceDSAMuonProducer::LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& ps )
    : fDSACandsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfcandsFromMuondSAPtr" ) ) ),
      fPFMuonsToken( consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "leptonjetSourcePFMuon", "inclusive" ) ) ) {
  produces<reco::PFCandidateFwdPtrVector>( "inclusive" );
  produces<reco::PFCandidateFwdPtrVector>( "nonisolated" );
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

  e.getByToken( fDSACandsToken, fDSACandsHdl );
  assert( fDSACandsHdl.isValid() );
  e.getByToken( fPFMuonsToken, fPFMuonsHdl );
  assert( fPFMuonsHdl.isValid() );

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
    const auto&  pfiso04  = muonref->pfIsolationR04();
    double       iso04val = ( pfiso04.sumChargedHadronPt + max( 0., pfiso04.sumNeutralHadronEt + pfiso04.sumPhotonEt - 0.5 * pfiso04.sumPUPt ) ) / muonref->pt();
    if (iso04val>0.25) // 0.4, 0.25, 0.20, 0.15, 0.10, 0.05
      continue;

    //matching with loose PFMuon
    if ( maxSegmentOverlapRatio( *muonref ) > 0.66 )
      continue;
    if ( minDeltaRAtInnermostPoint( *muonref ) < 0.2 )
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

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( LeptonjetSourceDSAMuonProducer );
