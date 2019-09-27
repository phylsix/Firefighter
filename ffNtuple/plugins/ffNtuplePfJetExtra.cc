#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

class ffNtuplePfJetExtra : public ffNtupleBase {
 public:
  ffNtuplePfJetExtra( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fLeptonjetToken;
  edm::EDGetToken fLeptonjetPfisoToken;

  std::vector<float> fLeptonjetPfiso;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePfJetExtra, "ffNtuplePfJetExtra" );

ffNtuplePfJetExtra::ffNtuplePfJetExtra( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtuplePfJetExtra::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fLeptonjetToken      = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fLeptonjetPfisoToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "leptonjetExtra", "pfIso" ) );

  tree.Branch( "pfjet_pfiso", &fLeptonjetPfiso );
}

void
ffNtuplePfJetExtra::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PFJetCollection> leptonjetHdl;
  e.getByToken( fLeptonjetToken, leptonjetHdl );
  assert( leptonjetHdl.isValid() );

  Handle<ValueMap<float>> leptonjetPfisoHdl;
  e.getByToken( fLeptonjetPfisoToken, leptonjetPfisoHdl );
  assert( leptonjetPfisoHdl.isValid() );

  clear();

  for ( size_t i( 0 ); i != leptonjetHdl->size(); i++ ) {
    Ptr<reco::PFJet> leptonjetptr( leptonjetHdl, i );
    fLeptonjetPfiso.emplace_back( ( *leptonjetPfisoHdl )[ leptonjetptr ] );
  }
}

void
ffNtuplePfJetExtra::clear() {
  fLeptonjetPfiso.clear();
}