#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleAKJet : public ffNtupleBaseNoHLT {
 public:
  ffNtupleAKJet( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken                      fAKJetToken;
  StringCutObjectSelector<reco::PFJet> fJetCut;
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
    : ffNtupleBaseNoHLT( ps ),
      fJetCut( ps.getParameter<std::string>( "cut" ), true ),
      fJetIdSelector( ps.getParameter<std::string>( "jetid" ), true ) {}

void
ffNtupleAKJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fAKJetToken              = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  const std::string label_ = ps.getParameter<edm::InputTag>( "src" ).label();

  tree.Branch( TString::Format( "akjet_%s_p4", label_.c_str() ), &fAKJetP4 );
  tree.Branch( TString::Format( "akjet_%s_jetid", label_.c_str() ), &fJetId );
  tree.Branch( TString::Format( "akjet_%s_pfcands_n", label_.c_str() ), &fNumCands );
  tree.Branch( TString::Format( "akjet_%s_hadronEnergyFraction", label_.c_str() ), &fHadronEnergyFraction );
  tree.Branch( TString::Format( "akjet_%s_chaHadEnergyFraction", label_.c_str() ), &fChaHadEnergyFraction );
  tree.Branch( TString::Format( "akjet_%s_emEnergyFraction", label_.c_str() ), &fEmEnergyFraction );
  tree.Branch( TString::Format( "akjet_%s_chaEmEnergyFraction", label_.c_str() ), &fChaEmEnergyFraction );
  tree.Branch( TString::Format( "akjet_%s_muonEnergyFraction", label_.c_str() ), &fMuonEnergyFraction );
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
    if ( !fJetCut( akjet ) )
      continue;
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