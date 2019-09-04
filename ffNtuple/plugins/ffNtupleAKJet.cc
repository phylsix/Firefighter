#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleAKJet : public ffNtupleBase {
 public:
  ffNtupleAKJet( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void            clear() final;
  edm::EDGetToken fAKJetToken;

  std::vector<LorentzVector> fAKJetP4;
  std::vector<float>         fHadronEnergyFraction;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleAKJet, "ffNtupleAKJet" );

ffNtupleAKJet::ffNtupleAKJet( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleAKJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fAKJetToken = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "akjet_p4", &fAKJetP4 );
  tree.Branch( "akjet_hadronEnergyFraction", &fHadronEnergyFraction );
}

void
ffNtupleAKJet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PFJetCollection> akjetHdl;
  e.getByToken( fAKJetToken, akjetHdl );
  assert( akjetHdl.isValid() );
  const reco::PFJetCollection& akjets = *akjetHdl;

  clear();

  for ( const auto& akjet : akjets ) {
    fAKJetP4.push_back( LorentzVector( akjet.px(), akjet.py(), akjet.pz(), akjet.energy() ) );
    fHadronEnergyFraction.emplace_back( akjet.chargedHadronEnergyFraction() + akjet.neutralHadronEnergyFraction() );
  }
}

void
ffNtupleAKJet::clear() {
  fAKJetP4.clear();
  fHadronEnergyFraction.clear();
}