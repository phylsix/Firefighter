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
  produces<edm::ValueMap<float>>( "minExtrapolateInnermostLocalDr" );
  produces<edm::ValueMap<float>>( "minExtrapolateInnermostLocalDiff" );
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

  auto vm_maxSegmentOverlapRatio           = make_unique<ValueMap<float>>();
  auto vm_minExtrapolateInnermostLocalDr   = make_unique<ValueMap<float>>();
  auto vm_minExtrapolateInnermostLocalDiff = make_unique<ValueMap<float>>();

  e.getByToken( fDsaMuonToken, fDsaMuonHdl );
  assert( fDsaMuonHdl.isValid() );
  e.getByToken( fRecoMuonToken, fRecoMuonHdl );
  assert( fRecoMuonHdl.isValid() );

  vector<float> v_maxSegmentOverlapRatio( fDsaMuonHdl->size(), 0. );
  vector<float> v_minExtrapolateInnermostLocalDr( fDsaMuonHdl->size(), 999. );
  vector<float> v_minExtrapolateInnermostLocalDiff( fDsaMuonHdl->size(), 999. );

  for ( size_t i( 0 ); i != fDsaMuonHdl->size(); i++ ) {
    const auto&   dsamuon = ( *fDsaMuonHdl )[ i ];
    vector<float> _segmentOverlapRatio( fRecoMuonHdl->size(), 0. );
    vector<float> _extrapolateInnermostLocalDr( fRecoMuonHdl->size(), 999. );
    vector<float> _extrapolateInnermostLocalDiff( fRecoMuonHdl->size(), 999. );

    for ( size_t j( 0 ); j != fRecoMuonHdl->size(); j++ ) {
      const auto& recomuon = ( *fRecoMuonHdl )[ j ];

      _segmentOverlapRatio[ j ]           = getSegmentOverlapRatioArbitration( dsamuon, recomuon );
      pair<float, float> localdistances   = getExtrapolateInnermostDistance( dsamuon, recomuon );
      _extrapolateInnermostLocalDr[ j ]   = localdistances.first;
      _extrapolateInnermostLocalDiff[ j ] = localdistances.second;
    }

    if ( !_segmentOverlapRatio.empty() )
      v_maxSegmentOverlapRatio[ i ] = *max_element( _segmentOverlapRatio.begin(), _segmentOverlapRatio.end() );
    if ( !_extrapolateInnermostLocalDr.empty() )
      v_minExtrapolateInnermostLocalDr[ i ] = *min_element( _extrapolateInnermostLocalDr.begin(), _extrapolateInnermostLocalDr.end() );
    if ( !_extrapolateInnermostLocalDiff.empty() )
      v_minExtrapolateInnermostLocalDiff[ i ] = *min_element( _extrapolateInnermostLocalDiff.begin(), _extrapolateInnermostLocalDiff.end() );
  }

  ValueMap<float>::Filler ratioFiller( *vm_maxSegmentOverlapRatio );
  ratioFiller.insert( fDsaMuonHdl, v_maxSegmentOverlapRatio.begin(), v_maxSegmentOverlapRatio.end() );
  ratioFiller.fill();
  e.put( move( vm_maxSegmentOverlapRatio ), "maxSegmentOverlapRatio" );

  ValueMap<float>::Filler localDrFiller( *vm_minExtrapolateInnermostLocalDr );
  localDrFiller.insert( fDsaMuonHdl, v_minExtrapolateInnermostLocalDr.begin(), v_minExtrapolateInnermostLocalDr.end() );
  localDrFiller.fill();
  e.put( move( vm_minExtrapolateInnermostLocalDr ), "minExtrapolateInnermostLocalDr" );

  ValueMap<float>::Filler localDiffFiller( *vm_minExtrapolateInnermostLocalDiff );
  localDiffFiller.insert( fDsaMuonHdl, v_minExtrapolateInnermostLocalDiff.begin(), v_minExtrapolateInnermostLocalDiff.end() );
  localDiffFiller.fill();
  e.put( move( vm_minExtrapolateInnermostLocalDiff ), "minExtrapolateInnermostLocalDiff" );
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

  if ( dsamuonCSCSegKeys.empty() and dsamuonDTSegKeys.empty() )
    return 0.;

  // overlap ratio
  return float( overlappedCSCSegKeys.size() + overlappedDTSegKeys.size() ) / ( dsamuonCSCSegKeys.size() + dsamuonDTSegKeys.size() );
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

std::pair<float, float>
DSAMuonValueMapProducer::getExtrapolateInnermostDistance( const reco::Muon& dsamuon,
                                                          const reco::Muon& recomuon ) const {
  std::pair<float, float> res = std::make_pair( 999., 999. );
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

  res.first  = deltaR( tsosInner.localMomentum(), targetInner.localMomentum() );
  res.second = ( tsosInner.localPosition() - targetInner.localPosition() ).mag();
  return res;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( DSAMuonValueMapProducer );