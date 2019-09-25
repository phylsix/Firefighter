#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "RecoMuon/MuonIdentification/interface/MuonCosmicsId.h"

class ffNtupleDsaMuonExtra : public ffNtupleBase {
 public:
  ffNtupleDsaMuonExtra( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fDsaMuonToken;
  edm::EDGetToken fDsaMuonTrackToken;
  edm::EDGetToken fDsaMuonOverlapRatioToken;
  edm::EDGetToken fDsaMuonExpoLocalDrToken;
  edm::EDGetToken fDsaMuonExpoLocalDiffToken;
  edm::EDGetToken fDsaMuonGlobalDrToken;

  std::vector<float> fDsaMuonOverlapRatio;
  std::vector<float> fDsaMuonExpoLocalDr;
  std::vector<float> fDsaMuonExpoLocalDiff;
  std::vector<float> fDsaMuonGlobalDr;
  std::vector<unsigned int>  fDsaMuonSelectors;
  std::vector<bool>  fDsaFindOppositeTrack;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleDsaMuonExtra, "ffNtupleDsaMuonExtra" );

ffNtupleDsaMuonExtra::ffNtupleDsaMuonExtra( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleDsaMuonExtra::initialize( TTree&                   tree,
                                  const edm::ParameterSet& ps,
                                  edm::ConsumesCollector&& cc ) {
  fDsaMuonToken              = cc.consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) );
  fDsaMuonTrackToken         = cc.consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) );
  fDsaMuonOverlapRatioToken  = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "maxSegmentOverlapRatio" ) );
  fDsaMuonExpoLocalDrToken   = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minExtrapolateInnermostLocalDr" ) );
  fDsaMuonExpoLocalDiffToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minExtrapolateInnermostLocalDiff" ) );
  fDsaMuonGlobalDrToken      = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minGlobalDeltaR" ) );

  tree.Branch( "dsamuon_maxSegmentOverlapRatio", &fDsaMuonOverlapRatio );
  tree.Branch( "dsamuon_minExtrapolateInnermostLocalDr", &fDsaMuonExpoLocalDr );
  tree.Branch( "dsamuon_minExtrapolateInnermostLocalDiff", &fDsaMuonExpoLocalDiff );
  tree.Branch( "dsamuon_minGlobalDeltaR", &fDsaMuonGlobalDr );
  tree.Branch( "dsamuon_selectors", &fDsaMuonSelectors );
  tree.Branch( "dsamuon_findOppositeTrack", &fDsaFindOppositeTrack );
}

void
ffNtupleDsaMuonExtra::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> dsamuonHdl;
  e.getByToken( fDsaMuonToken, dsamuonHdl );
  assert( dsamuonHdl.isValid() );

  Handle<reco::TrackCollection> dsamuonTkHdl;
  e.getByToken( fDsaMuonTrackToken, dsamuonTkHdl );
  assert( dsamuonTkHdl.isValid() );

  Handle<ValueMap<float>> dsamuonOverlapRatioHdl;
  e.getByToken( fDsaMuonOverlapRatioToken, dsamuonOverlapRatioHdl );
  assert( dsamuonOverlapRatioHdl.isValid() );
  Handle<ValueMap<float>> dsamuonExpoLocalDrHdl;
  e.getByToken( fDsaMuonExpoLocalDrToken, dsamuonExpoLocalDrHdl );
  assert( dsamuonExpoLocalDrHdl.isValid() );
  Handle<ValueMap<float>> dsamuonExpoLocalDiffHdl;
  e.getByToken( fDsaMuonExpoLocalDiffToken, dsamuonExpoLocalDiffHdl );
  assert( dsamuonExpoLocalDiffHdl.isValid() );
  Handle<ValueMap<float>> dsamuonGlobalDrHdl;
  e.getByToken( fDsaMuonGlobalDrToken, dsamuonGlobalDrHdl );
  assert( dsamuonGlobalDrHdl.isValid() );

  clear();

  for ( size_t i( 0 ); i != dsamuonHdl->size(); i++ ) {
    Ptr<reco::Muon> dsamuonptr( dsamuonHdl, i );
    fDsaMuonOverlapRatio.emplace_back( ( *dsamuonOverlapRatioHdl )[ dsamuonptr ] );
    fDsaMuonExpoLocalDr.emplace_back( ( *dsamuonExpoLocalDrHdl )[ dsamuonptr ] );
    fDsaMuonExpoLocalDiff.emplace_back( ( *dsamuonExpoLocalDiffHdl )[ dsamuonptr ] );
    fDsaMuonGlobalDr.emplace_back( ( *dsamuonGlobalDrHdl )[ dsamuonptr ] );
    fDsaMuonSelectors.emplace_back( dsamuonptr->selectors() );
    fDsaFindOppositeTrack.emplace_back( muonid::findOppositeTrack( dsamuonTkHdl, *( dsamuonptr->bestTrack() ) ).isNonnull() );
  }
}

void
ffNtupleDsaMuonExtra::clear() {
  fDsaMuonOverlapRatio.clear();
  fDsaMuonExpoLocalDr.clear();
  fDsaMuonExpoLocalDiff.clear();
  fDsaMuonGlobalDr.clear();
  fDsaMuonSelectors.clear();
  fDsaFindOppositeTrack.clear();
}