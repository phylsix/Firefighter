#include "Firefighter/recoStuff/interface/DSAMuonValueMapProducer.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include <algorithm>

DSAMuonValueMapProducer::DSAMuonValueMapProducer( const edm::ParameterSet& ps )
    : fDsaMuonToken( consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) ) ),
      fRecoMuonToken( consumes<reco::MuonCollection>( edm::InputTag( "muons" ) ) ) {
  produces<edm::ValueMap<float>>( "maxSegmentOverlapRatio" );
  produces<edm::ValueMap<float>>( "minExtrapolateInnermostDistance" );
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

  auto vm_maxSegmentOverlapRatio          = make_unique<ValueMap<float>>();
  auto vm_minExtrapolateInnermostDistance = make_unique<ValueMap<float>>();

  e.getByToken( fDsaMuonToken, fDsaMuonHdl );
  assert( fDsaMuonHdl.isValid() );
  e.getByToken( fRecoMuonToken, fRecoMuonHdl );
  assert( fRecoMuonHdl.isValid() );

  vector<float> v_maxSegmentOverlapRatio( fDsaMuonHdl->size(), 0. );
  vector<float> v_minExtrapolateInnermostDistance( fDsaMuonHdl->size(), 999. );

  for ( size_t i( 0 ); i != fDsaMuonHdl->size(); i++ ) {
    const auto&   dsamuon = ( *fDsaMuonHdl )[ i ];
    vector<float> _segmentOverlapRatio( fRecoMuonHdl->size(), 0. );
    vector<float> _extrapolateInnermostDistance( fRecoMuonHdl->size(), 999. );

    for ( size_t j( 0 ); j != fRecoMuonHdl->size(); j++ ) {
      const auto& recomuon = ( *fRecoMuonHdl )[ j ];

      _segmentOverlapRatio[ j ]          = getSegmentOverlapRatio( dsamuon, recomuon );
      _extrapolateInnermostDistance[ j ] = getExtrapolateInnermostDistance( dsamuon, recomuon );
    }

    v_maxSegmentOverlapRatio[ i ]          = *max_element( _segmentOverlapRatio.begin(), _segmentOverlapRatio.end() );
    v_minExtrapolateInnermostDistance[ i ] = *min_element( _extrapolateInnermostDistance.begin(), _extrapolateInnermostDistance.end() );
  }

  ValueMap<float>::Filler ratioFiller( *vm_maxSegmentOverlapRatio );
  ratioFiller.insert( fDsaMuonHdl, v_maxSegmentOverlapRatio.begin(), v_maxSegmentOverlapRatio.end() );
  ratioFiller.fill();
  e.put( move( vm_maxSegmentOverlapRatio ), "maxSegmentOverlapRatio" );

  ValueMap<float>::Filler distanceFiller( *vm_minExtrapolateInnermostDistance );
  distanceFiller.insert( fDsaMuonHdl, v_minExtrapolateInnermostDistance.begin(), v_minExtrapolateInnermostDistance.end() );
  distanceFiller.fill();
  e.put( move( vm_minExtrapolateInnermostDistance ), "minExtrapolateInnermostDistance" );
}

std::vector<int>
DSAMuonValueMapProducer::getCSCSegmentKeys( const reco::Muon& muon ) const {
  std::vector<int> res{};

  if ( muon.numberOfChambersCSCorDT() == 0 )
    return res;

  for ( const auto& muonmatch : muon.matches() ) {
    for ( const auto& seg : muonmatch.segmentMatches ) {
      if ( seg.cscSegmentRef.isNonnull() )
        res.emplace_back( seg.cscSegmentRef.key() );
    }
  }

  return res;
}

std::vector<int>
DSAMuonValueMapProducer::getDTSegmentKeys( const reco::Muon& muon ) const {
  std::vector<int> res{};

  if ( muon.numberOfChambersCSCorDT() == 0 )
    return res;

  for ( const auto& muonmatch : muon.matches() ) {
    for ( const auto& seg : muonmatch.segmentMatches ) {
      if ( seg.dtSegmentRef.isNonnull() )
        res.emplace_back( seg.dtSegmentRef.key() );
    }
  }

  return res;
}

float
DSAMuonValueMapProducer::getSegmentOverlapRatio( const reco::Muon& dsamuon,
                                                 const reco::Muon& recomuon ) const {
  using namespace std;

  if ( !recomuon.isGlobalMuon() and !recomuon.isTrackerMuon() )
    return 0.;

  vector<int> dsamuonCSCSegKeys = getCSCSegmentKeys( dsamuon );
  vector<int> dsamuonDTSegKeys  = getDTSegmentKeys( dsamuon );
  sort( dsamuonCSCSegKeys.begin(), dsamuonCSCSegKeys.end() );
  sort( dsamuonDTSegKeys.begin(), dsamuonDTSegKeys.end() );

  vector<int> recomuonCSCSegKeys = getCSCSegmentKeys( recomuon );
  vector<int> recomuonDTSegKeys  = getDTSegmentKeys( recomuon );
  sort( recomuonCSCSegKeys.begin(), recomuonCSCSegKeys.end() );
  sort( recomuonDTSegKeys.begin(), recomuonDTSegKeys.end() );

  vector<int> overlappedCSCSegKeys{}, overlappedDTSegKeys{};
  set_intersection( dsamuonCSCSegKeys.begin(), dsamuonCSCSegKeys.end(), recomuonCSCSegKeys.begin(), recomuonCSCSegKeys.end(), back_inserter( overlappedCSCSegKeys ) );
  set_intersection( dsamuonDTSegKeys.begin(), dsamuonDTSegKeys.end(), recomuonDTSegKeys.begin(), recomuonDTSegKeys.end(), back_inserter( overlappedDTSegKeys ) );

  if ( ( dsamuonCSCSegKeys.size() + dsamuonDTSegKeys.size() ) == 0 )
    return 0.;

  // overlap ratio
  return float( overlappedCSCSegKeys.size() + overlappedDTSegKeys.size() ) / ( dsamuonCSCSegKeys.size() + dsamuonDTSegKeys.size() );
}

float
DSAMuonValueMapProducer::getExtrapolateInnermostDistance( const reco::Muon& dsamuon,
                                                          const reco::Muon& recomuon ) const {
  float res = 999.;
  if ( !recomuon.isTrackerMuon() )
    return res;
  if ( recomuon.innerTrack().isNull() )
    return res;
  assert( dsamuon.outerTrack().isNonnull() );

  const reco::TrackRef innerTk = recomuon.innerTrack();
  FreeTrajectoryState  startFTS =
      trajectoryStateTransform::initialFreeState( *innerTk,
                                                  fTkExtrapolator->getMagneticField() );

  const reco::TrackRef     dsaTk = dsamuon.outerTrack();
  TrajectoryStateOnSurface targetInner =
      trajectoryStateTransform::innerStateOnSurface( *dsaTk,
                                                     *( fTkExtrapolator->getTrackingGeometry() ),
                                                     fTkExtrapolator->getMagneticField() );
  if ( !targetInner.isValid() )
    return res;

  TrajectoryStateOnSurface tsosInner = fTkExtrapolator->propagate( startFTS,
                                                                   targetInner.surface() );
  if ( !tsosInner.isValid() )
    return res;

  res = deltaR( tsosInner.localMomentum(), targetInner.localMomentum() );
  return res;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( DSAMuonValueMapProducer );