#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtuplePfJetExtra : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePfJetExtra( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fLeptonjetToken;
  edm::EDGetToken fLeptonjetPfisoToken;
  edm::EDGetToken fLeptonjetMindrToken;

  std::vector<float> fLeptonjetPfiso;
  std::vector<float> fLeptonjetMindr;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePfJetExtra, "ffNtuplePfJetExtra" );

ffNtuplePfJetExtra::ffNtuplePfJetExtra( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtuplePfJetExtra::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fLeptonjetToken      = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fLeptonjetPfisoToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "leptonjetExtra", "pfIso" ) );
  fLeptonjetMindrToken = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "leptonjetExtra", "minDeltaR" ) );

  tree.Branch( "pfjet_pfiso", &fLeptonjetPfiso )->SetTitle("PFCandidate-based isolation value, delta-beta style PU correction");
  tree.Branch( "pfjet_mindr", &fLeptonjetMindr )->SetTitle("minimum distance wrt. other leptonjet, if any(999. otherwise), on eta-phi plane");
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

  Handle<ValueMap<float>> leptonjetMindrHdl;
  e.getByToken( fLeptonjetMindrToken, leptonjetMindrHdl );
  assert( leptonjetMindrHdl.isValid() );

  clear();

  for ( size_t i( 0 ); i != leptonjetHdl->size(); i++ ) {
    Ptr<reco::PFJet> leptonjetptr( leptonjetHdl, i );
    fLeptonjetPfiso.emplace_back( ( *leptonjetPfisoHdl )[ leptonjetptr ] );
    fLeptonjetMindr.emplace_back( ( *leptonjetMindrHdl )[ leptonjetptr ] );
  }
}

void
ffNtuplePfJetExtra::clear() {
  fLeptonjetPfiso.clear();
  fLeptonjetMindr.clear();
}