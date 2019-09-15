#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
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
  void clear() final;

  edm::EDGetToken                      fAKJetToken;
  StringCutObjectSelector<reco::PFJet> fJetIdSelector;

  std::vector<LorentzVector> fAKJetP4;
  std::vector<bool>          fJetId;
  std::vector<float>         fHadronEnergyFraction;
  std::vector<float>         fChaHadEnergyFraction;
  std::vector<float>         fEmEnergyFraction;
  std::vector<float>         fChaEmEnergyFraction;
  std::vector<float>         fMuonEnergyFraction;
  std::vector<int>           fNumCands;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleAKJet, "ffNtupleAKJet" );

ffNtupleAKJet::ffNtupleAKJet( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ),
      fJetIdSelector( ps.getParameter<std::string>( "jetid" ), true ) {}

void
ffNtupleAKJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fAKJetToken = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "akjet_p4", &fAKJetP4 );
  tree.Branch( "akjet_jetid", &fJetId );
  tree.Branch( "akjet_pfcands_n", &fNumCands );
  tree.Branch( "akjet_hadronEnergyFraction", &fHadronEnergyFraction );
  tree.Branch( "akjet_chaHadEnergyFraction", &fChaHadEnergyFraction );
  tree.Branch( "akjet_emEnergyFraction", &fEmEnergyFraction );
  tree.Branch( "akjet_chaEmEnergyFraction", &fChaEmEnergyFraction );
  tree.Branch( "akjet_muonEnergyFraction", &fMuonEnergyFraction );
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
    fAKJetP4.emplace_back( akjet.px(), akjet.py(), akjet.pz(), akjet.energy() );
    fJetId.emplace_back( fJetIdSelector( akjet ) );
    fNumCands.emplace_back( akjet.chargedMultiplicity() + akjet.neutralMultiplicity() );
    fHadronEnergyFraction.emplace_back( akjet.chargedHadronEnergyFraction() + akjet.neutralHadronEnergyFraction() );
    fChaHadEnergyFraction.emplace_back( akjet.chargedHadronEnergyFraction() );
    fEmEnergyFraction.emplace_back( akjet.chargedEmEnergyFraction() + akjet.neutralEmEnergyFraction() );
    fChaEmEnergyFraction.emplace_back( akjet.chargedEmEnergyFraction() );
    fMuonEnergyFraction.emplace_back( akjet.muonEnergyFraction() );
  }
}

void
ffNtupleAKJet::clear() {
  fAKJetP4.clear();
  fJetId.clear();
  fNumCands.clear();
  fHadronEnergyFraction.clear();
  fChaHadEnergyFraction.clear();
  fEmEnergyFraction.clear();
  fChaEmEnergyFraction.clear();
  fMuonEnergyFraction.clear();
}