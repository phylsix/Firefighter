#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"

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
  StringCutObjectSelector<reco::PFJet> fJetIdSelector;
  edm::EDGetToken                      fJetCorrectorToken;

  unsigned int               fNAKJet;
  std::vector<LorentzVector> fAKJetP4;
  std::vector<LorentzVector> fAKJetRawP4;
  std::vector<bool>          fJetId;
  std::vector<float>         fHadronEnergyFraction;
  std::vector<float>         fChaHadEnergyFraction;
  std::vector<float>         fEmEnergyFraction;
  std::vector<float>         fChaEmEnergyFraction;
  std::vector<float>         fMuonEnergyFraction;
  std::vector<int>           fNumCands;
  std::vector<int>           fNumMuons;
  std::vector<int>           fChargeSumOfMuons;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleAKJet, "ffNtupleAKJet" );

ffNtupleAKJet::ffNtupleAKJet( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      fJetIdSelector( ps.getParameter<std::string>( "jetid" ), true ) {}

void
ffNtupleAKJet::initialize( TTree&                   tree,
                           const edm::ParameterSet& ps,
                           edm::ConsumesCollector&& cc ) {
  fAKJetToken              = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  const std::string label_ = ps.getParameter<edm::InputTag>( "src" ).label();
  fJetCorrectorToken       = cc.consumes<reco::JetCorrector>( ps.getParameter<edm::InputTag>( "corrector" ) );

  tree.Branch( TString::Format( "akjet_%s_n", label_.c_str() ), &fNAKJet );
  tree.Branch( TString::Format( "akjet_%s_p4", label_.c_str() ), &fAKJetP4 )->SetTitle( "jet p4, w/ JEC applied" );
  tree.Branch( TString::Format( "akjet_%s_rawP4", label_.c_str() ), &fAKJetRawP4 )->SetTitle( "jet raw p4, w/o JEC" );
  tree.Branch( TString::Format( "akjet_%s_jetid", label_.c_str() ), &fJetId )->SetTitle( "whether pass jet ID" );
  tree.Branch( TString::Format( "akjet_%s_pfcands_n", label_.c_str() ), &fNumCands )->SetTitle( "constituents multiplicity" );
  tree.Branch( TString::Format( "akjet_%s_hadronEnergyFraction", label_.c_str() ), &fHadronEnergyFraction )->SetTitle( "hadron energy fraction" );
  tree.Branch( TString::Format( "akjet_%s_chaHadEnergyFraction", label_.c_str() ), &fChaHadEnergyFraction )->SetTitle( "charged hadron energy fraction" );
  tree.Branch( TString::Format( "akjet_%s_emEnergyFraction", label_.c_str() ), &fEmEnergyFraction )->SetTitle( "EM energy fraction" );
  tree.Branch( TString::Format( "akjet_%s_chaEmEnergyFraction", label_.c_str() ), &fChaEmEnergyFraction )->SetTitle( "charged EM energy fraction" );
  tree.Branch( TString::Format( "akjet_%s_muonEnergyFraction", label_.c_str() ), &fMuonEnergyFraction )->SetTitle( "muon energy fraction" );
  tree.Branch( TString::Format( "akjet_%s_muons_n", label_.c_str() ), &fNumMuons )->SetTitle( "muon multiplicity" );
  tree.Branch( TString::Format( "akjet_%s_muonChargeSum", label_.c_str() ), &fChargeSumOfMuons )->SetTitle( "muon charge sum" );
}

void
ffNtupleAKJet::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PFJetCollection> akjetHdl;
  e.getByToken( fAKJetToken, akjetHdl );
  assert( akjetHdl.isValid() );
  const reco::PFJetCollection& akjets = *akjetHdl;

  Handle<reco::JetCorrector> correctorHdl;
  e.getByToken( fJetCorrectorToken, correctorHdl );
  assert( correctorHdl.isValid() );

  clear();

  fNAKJet = akjets.size();
  for ( const auto& akjet : akjets ) {
    double jec = correctorHdl->correction( akjet );
    fAKJetP4.push_back( LorentzVector( akjet.px(), akjet.py(), akjet.pz(), akjet.energy() ) * jec );
    fAKJetRawP4.emplace_back( akjet.px(), akjet.py(), akjet.pz(), akjet.energy() );
    fJetId.emplace_back( fJetIdSelector( akjet ) );
    fNumCands.emplace_back( akjet.chargedMultiplicity() + akjet.neutralMultiplicity() );
    fHadronEnergyFraction.emplace_back( akjet.chargedHadronEnergyFraction() + akjet.neutralHadronEnergyFraction() );
    fChaHadEnergyFraction.emplace_back( akjet.chargedHadronEnergyFraction() );
    fEmEnergyFraction.emplace_back( akjet.chargedEmEnergyFraction() + akjet.neutralEmEnergyFraction() );
    fChaEmEnergyFraction.emplace_back( akjet.chargedEmEnergyFraction() );
    fMuonEnergyFraction.emplace_back( akjet.muonEnergyFraction() );
    fNumMuons.emplace_back( akjet.muonMultiplicity() );
    fChargeSumOfMuons.emplace_back( ff::sumPFMuonCharge( akjet ) );
  }
}

void
ffNtupleAKJet::clear() {
  fNAKJet = 0;
  fAKJetP4.clear();
  fAKJetRawP4.clear();
  fJetId.clear();
  fNumCands.clear();
  fHadronEnergyFraction.clear();
  fChaHadEnergyFraction.clear();
  fEmEnergyFraction.clear();
  fChaEmEnergyFraction.clear();
  fMuonEnergyFraction.clear();
  fNumMuons.clear();
  fChargeSumOfMuons.clear();
}