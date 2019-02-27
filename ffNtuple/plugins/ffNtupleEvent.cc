#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

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

  int run_;
  int event_;
  int lumi_;

  edm::EDGetToken puToken_;

  int   puInteractionNum_;
  float trueInteractionNum_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleEvent, "ffNtupleEvent" );

ffNtupleEvent::ffNtupleEvent( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleEvent::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  puToken_ = cc.consumes<std::vector<PileupSummaryInfo>>(
      ps.getParameter<edm::InputTag>( "PileUp" ) );

  tree.Branch( "run", &run_, "run/I" );
  tree.Branch( "event", &event_, "event/I" );
  tree.Branch( "lumi", &lumi_, "lumi/I" );

  tree.Branch( "puInteractionNum", &puInteractionNum_, "puInteractionNum/I" );
  tree.Branch( "trueInteractionNum", &trueInteractionNum_,
               "trueInteractionNum/F" );
}

void
ffNtupleEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<vector<PileupSummaryInfo>> puInfo_h;
  e.getByToken( puToken_, puInfo_h );
  assert( puInfo_h.isValid() );

  clear();

  run_   = e.id().run();
  event_ = e.id().event();
  lumi_  = e.luminosityBlock();

  for ( const auto& PVI : *puInfo_h ) {
    if ( PVI.getBunchCrossing() == 0 ) {
      puInteractionNum_   = PVI.getPU_NumInteractions();
      trueInteractionNum_ = PVI.getTrueNumInteractions();
    }
  }
}

void
ffNtupleEvent::clear() {
  run_   = 0;
  event_ = 0;
  lumi_  = 0;

  puInteractionNum_   = 0;
  trueInteractionNum_ = 0.;
}