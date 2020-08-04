#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleCosmicVeto : public ffNtupleBaseNoHLT {
 public:
  ffNtupleCosmicVeto( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fCosmicShowerVetoResultToken;
  edm::EDGetToken fParallelCosmicPairsToken;

  bool fCosmicShowerVetoResult;
  int  fParallelCosmicPairs;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleCosmicVeto, "ffNtupleCosmicVeto" );

ffNtupleCosmicVeto::ffNtupleCosmicVeto( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleCosmicVeto::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fCosmicShowerVetoResultToken = cc.consumes<bool>( edm::InputTag( "ffcosmiceventfilter" ) );
  fParallelCosmicPairsToken    = cc.consumes<int>( edm::InputTag( "ffcosmiceventfilter" ) );

  tree.Branch( "cosmicveto_result", &fCosmicShowerVetoResult )->SetTitle( "N(parallel)<=6" );
  tree.Branch( "cosmicveto_parallelpairs", &fParallelCosmicPairs )->SetTitle( "N(parallel cosmicMuon pairs) pT>5GeV & abs(eta)<1.2" );
}

void
ffNtupleCosmicVeto::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<bool> cosmicShowerVetoResultHdl;
  e.getByToken( fCosmicShowerVetoResultToken, cosmicShowerVetoResultHdl );
  assert( cosmicShowerVetoResultHdl.isValid() );

  Handle<int> parallelCosmicPairsHdl;
  e.getByToken( fParallelCosmicPairsToken, parallelCosmicPairsHdl );
  assert( parallelCosmicPairsHdl.isValid() );

  clear();

  fCosmicShowerVetoResult = *cosmicShowerVetoResultHdl;
  fParallelCosmicPairs    = *parallelCosmicPairsHdl;
}

void
ffNtupleCosmicVeto::clear() {
  fCosmicShowerVetoResult = false;
  fParallelCosmicPairs    = 0;
}