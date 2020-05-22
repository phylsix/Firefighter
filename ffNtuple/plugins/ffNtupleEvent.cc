#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleEvent : public ffNtupleBaseNoHLT {
 public:
  ffNtupleEvent( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fGridRhoToken;

  unsigned int       run_;
  unsigned long long event_;
  unsigned int       lumi_;
  float              fixedgridrho_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleEvent, "ffNtupleEvent" );

ffNtupleEvent::ffNtupleEvent( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleEvent::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fGridRhoToken = cc.consumes<double>( edm::InputTag( "fixedGridRhoAll" ) );

  tree.Branch( "run", &run_ );
  tree.Branch( "event", &event_ );
  tree.Branch( "lumi", &lumi_ );
  tree.Branch( "fixedGridRho", &fixedgridrho_ )->SetTitle( "median grid pT density" );
}

void
ffNtupleEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  run_   = e.id().run();
  event_ = e.id().event();
  lumi_  = e.luminosityBlock();

  edm::Handle<double> gridrhoHdl;
  e.getByToken( fGridRhoToken, gridrhoHdl );
  assert( gridrhoHdl.isValid() );

  fixedgridrho_ = *gridrhoHdl;
}

void
ffNtupleEvent::clear() {
  run_          = 0;
  event_        = 0;
  lumi_         = 0;
  fixedgridrho_ = 0.;
}
