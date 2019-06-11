#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include <numeric>

class ffNtupleGenEvent : public ffNtupleBase {
 public:
  ffNtupleGenEvent( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken puToken_;
  edm::EDGetToken genprodToken_;

  int   puInteractionNum_;
  float trueInteractionNum_;
  float weight_;
  float weightProduct_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleGenEvent, "ffNtupleGenEvent" );

ffNtupleGenEvent::ffNtupleGenEvent( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleGenEvent::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  puToken_ = cc.consumes<std::vector<PileupSummaryInfo>>(
      ps.getParameter<edm::InputTag>( "PileUp" ) );
  genprodToken_ = cc.consumes<GenEventInfoProduct>(
      ps.getParameter<edm::InputTag>( "GenProd" ) );

  tree.Branch( "puInteractionNum", &puInteractionNum_, "puInteractionNum/I" );
  tree.Branch( "trueInteractionNum", &trueInteractionNum_,
               "trueInteractionNum/F" );
  tree.Branch( "weight", &weight_, "weight/F" );
  tree.Branch( "weightProduct", &weightProduct_, "weightProduct/F" );
}

void
ffNtupleGenEvent::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  clear();

  Handle<vector<PileupSummaryInfo>> puInfo_h;
  e.getByToken( puToken_, puInfo_h );
  assert( puInfo_h.isValid() );

  Handle<GenEventInfoProduct> genprod_h;
  e.getByToken( genprodToken_, genprod_h );
  assert( genprod_h.isValid() );

  for ( const auto& PVI : *puInfo_h ) {
    if ( PVI.getBunchCrossing() == 0 ) {
      puInteractionNum_   = PVI.getPU_NumInteractions();
      trueInteractionNum_ = PVI.getTrueNumInteractions();
    }
  }

  weight_        = float( genprod_h->weight() );
  weightProduct_ = float( genprod_h->weightProduct() );
}

void
ffNtupleGenEvent::clear() {
  puInteractionNum_   = 0;
  trueInteractionNum_ = 0.;
  weight_             = NAN;
  weightProduct_      = NAN;
}
