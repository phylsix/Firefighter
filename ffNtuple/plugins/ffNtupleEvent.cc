#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include <numeric>

class ffNtupleEvent : public ffNtupleBase {
 public:
  ffNtupleEvent( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken pvs_token_;

  unsigned int       run_;
  unsigned long long event_;
  unsigned int       lumi_;
  unsigned int       npv_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleEvent, "ffNtupleEvent" );

ffNtupleEvent::ffNtupleEvent( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleEvent::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  pvs_token_ = cc.consumes<reco::VertexCollection>( ps.getParameter<edm::InputTag>( "PrimaryVertices" ) );

  tree.Branch( "run", &run_ );
  tree.Branch( "event", &event_ );
  tree.Branch( "lumi", &lumi_ );
  tree.Branch( "npv", &npv_ );
}

void
ffNtupleEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;

  Handle<reco::VertexCollection> pvs_h;
  e.getByToken( pvs_token_, pvs_h );
  assert( pvs_h.isValid() && pvs_h->size() > 0 );

  clear();

  run_   = e.id().run();
  event_ = e.id().event();
  lumi_  = e.luminosityBlock();
  npv_   = pvs_h->size();
}

void
ffNtupleEvent::clear() {
  run_   = 0;
  event_ = 0;
  lumi_  = 0;
  npv_   = 0;
}
