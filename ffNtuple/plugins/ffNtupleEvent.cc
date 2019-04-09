#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

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

  int run_;
  int event_;
  int lumi_;

  edm::EDGetToken puToken_;
  edm::EDGetToken genprodToken_;

  int   puInteractionNum_;
  float trueInteractionNum_;
  float weight_;
  float weightProduct_;
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
  genprodToken_ = cc.consumes<GenEventInfoProduct>(
      ps.getParameter<edm::InputTag>( "GenProd" ) );

  tree.Branch( "run", &run_, "run/I" );
  tree.Branch( "event", &event_, "event/I" );
  tree.Branch( "lumi", &lumi_, "lumi/I" );

  tree.Branch( "puInteractionNum", &puInteractionNum_, "puInteractionNum/I" );
  tree.Branch( "trueInteractionNum", &trueInteractionNum_,
               "trueInteractionNum/F" );
  tree.Branch( "weight", &weight_, "weight/F" );
  tree.Branch( "weightProduct", &weightProduct_, "weightProduct/F" );
}

void
ffNtupleEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<vector<PileupSummaryInfo>> puInfo_h;
  e.getByToken( puToken_, puInfo_h );
  assert( puInfo_h.isValid() );

  Handle<GenEventInfoProduct> genprod_h;
  e.getByToken( genprodToken_, genprod_h );

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

  if ( genprod_h.isValid() ) {
    weight_        = float( genprod_h->weight() );
    weightProduct_ = float( genprod_h->weightProduct() );
  }
}

void
ffNtupleEvent::clear() {
  run_   = 0;
  event_ = 0;
  lumi_  = 0;

  puInteractionNum_   = 0;
  trueInteractionNum_ = 0.;
  weight_             = NAN;
  weightProduct_      = NAN;
}
