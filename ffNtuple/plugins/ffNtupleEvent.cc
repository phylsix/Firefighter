#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

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

  unsigned int       run_;
  unsigned long long event_;
  unsigned int       lumi_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleEvent, "ffNtupleEvent" );

ffNtupleEvent::ffNtupleEvent( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleEvent::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  tree.Branch( "run", &run_ );
  tree.Branch( "event", &event_ );
  tree.Branch( "lumi", &lumi_ );
}

void
ffNtupleEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  run_   = e.id().run();
  event_ = e.id().event();
  lumi_  = e.luminosityBlock();
}

void
ffNtupleEvent::clear() {
  run_   = 0;
  event_ = 0;
  lumi_  = 0;
}
