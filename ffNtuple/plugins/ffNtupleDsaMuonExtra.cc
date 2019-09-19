#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

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
  edm::EDGetToken fDsaMuonOverlapRatioToken;
  edm::EDGetToken fDsaMuonExpoDistanceToken;

  std::vector<float> fDsaMuonOverlapRatio;
  std::vector<float> fDsaMuonExpoDistance;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleDsaMuonExtra, "ffNtupleDsaMuonExtra" );

ffNtupleDsaMuonExtra::ffNtupleDsaMuonExtra( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleDsaMuonExtra::initialize( TTree&                   tree,
                                  const edm::ParameterSet& ps,
                                  edm::ConsumesCollector&& cc ) {
  fDsaMuonToken             = cc.consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) );
  fDsaMuonOverlapRatioToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "maxSegmentOverlapRatio" ) );
  fDsaMuonExpoDistanceToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minExtrapolateInnermostDistance" ) );

  tree.Branch( "dsamuon_maxSegmentOverlapRatio", &fDsaMuonOverlapRatio );
  tree.Branch( "dsamuon_minExtrapolateInnermostDistance", &fDsaMuonExpoDistance );
}

void
ffNtupleDsaMuonExtra::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> dsamuonHdl;
  e.getByToken( fDsaMuonToken, dsamuonHdl );
  assert( dsamuonHdl.isValid() );

  Handle<ValueMap<float>> dsamuonOverlapRatioHdl;
  e.getByToken( fDsaMuonOverlapRatioToken, dsamuonOverlapRatioHdl );
  assert( dsamuonOverlapRatioHdl.isValid() );
  Handle<ValueMap<float>> dsamuonExpoDistanceHdl;
  e.getByToken( fDsaMuonExpoDistanceToken, dsamuonExpoDistanceHdl );
  assert( dsamuonExpoDistanceHdl.isValid() );

  clear();

  for ( size_t i( 0 ); i != dsamuonHdl->size(); i++ ) {
    Ptr<reco::Muon> dsamuonptr( dsamuonHdl, i );
    fDsaMuonOverlapRatio.emplace_back( ( *dsamuonOverlapRatioHdl )[ dsamuonptr ] );
    fDsaMuonExpoDistance.emplace_back( ( *dsamuonExpoDistanceHdl )[ dsamuonptr ] );
  }
}

void
ffNtupleDsaMuonExtra::clear() {
  fDsaMuonOverlapRatio.clear();
  fDsaMuonExpoDistance.clear();
}