
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ffTester : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTester( const edm::ParameterSet& );
  ~ffTester();

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::JetView>                           fJetToken;
  edm::EDGetTokenT<reco::JetView>                           fSjetToken;
  edm::EDGetTokenT<reco::PFJetCollection>                   fSubjetToken;
  edm::EDGetTokenT<edm::Association<reco::PFJetCollection>> fJetsjetMapToken;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetEnergyDistrToken;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetMomentumDistrToken;

  edm::Handle<reco::JetView>                           fjetHdl;
  edm::Handle<reco::JetView>                           fSjetHdl;
  edm::Handle<reco::PFJetCollection>                   fSubjetHdl;
  edm::Handle<edm::Association<reco::PFJetCollection>> fJetsjetMapHdl;
  edm::Handle<edm::ValueMap<float>>                    fJetEnergyDistrHdl;
  edm::Handle<edm::ValueMap<float>>                    fJetMomentumDistrHdl;
};

ffTester::ffTester( const edm::ParameterSet& iC ) {
  fJetToken  = consumes<reco::JetView>( edm::InputTag( "ffLeptonJet" ) );
  fSjetToken = consumes<reco::JetView>( edm::InputTag( "ffLeptonJetSubjets" ) );
  fSubjetToken = consumes<reco::PFJetCollection>(
      edm::InputTag( "ffLeptonJetSubjets", "SubJets" ) );
  fJetsjetMapToken = consumes<edm::Association<reco::PFJetCollection>>(
      edm::InputTag( "ffLeptonJetSLeptonJetMap" ) );
  fJetEnergyDistrToken = consumes<edm::ValueMap<float>>(
      edm::InputTag( "ffLeptonJetSubjetEMD", "energy" ) );
  fJetMomentumDistrToken = consumes<edm::ValueMap<float>>(
      edm::InputTag( "ffLeptonJetSubjetEMD", "momentum" ) );
}

ffTester::~ffTester() {}

void
ffTester::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;
  using namespace std;

  e.getByToken( fJetToken, fjetHdl );
  assert( fjetHdl.isValid() );
  e.getByToken( fSjetToken, fSjetHdl );
  assert( fSjetHdl.isValid() );
  e.getByToken( fSubjetToken, fSubjetHdl );
  assert( fSubjetHdl.isValid() );
  e.getByToken( fJetsjetMapToken, fJetsjetMapHdl );
  assert( fJetsjetMapHdl.isValid() );
  e.getByToken( fJetEnergyDistrToken, fJetEnergyDistrHdl );
  assert( fJetEnergyDistrHdl.isValid() );
  e.getByToken( fJetMomentumDistrToken, fJetMomentumDistrHdl );

  const auto& subjets    = *fSubjetHdl;
  const auto& jetsjetMap = *fJetsjetMapHdl;

  const auto& jetEnergyDistrVM   = *fJetEnergyDistrHdl;
  const auto& jetMomentumDistrVM = *fJetMomentumDistrHdl;

  // cout<<"jet prodId: "<<fjetHdl.id()<<" sjet prodId:
  // "<<fSjetHdl.id()<<", the Assocation contains?
  // "<<jetsjetMap.contains(fSjetHdl.id())<<endl;
  cout << "subjetId: " << fSubjetHdl.id() << " size: " << subjets.size()
       << endl;

  for ( size_t i( 0 ); i != fSjetHdl->size(); ++i ) {
    Ptr<reco::Jet> sjet( fSjetHdl, i );
    cout << "sjet pt: " << sjet->pt() << " eta: " << sjet->eta()
         << " phi: " << sjet->phi() << endl;
    const std::vector<reco::CandidatePtr>& sjdaus = sjet->daughterPtrVector();
    for ( const reco::CandidatePtr& dau : sjdaus ) {
      cout << "\t\t+ pt: " << dau->pt() << " eta: " << dau->eta()
           << " phi: " << dau->phi() << " |" << dau.id() << " " << dau.key()
           << "|"
           << "pt: " << subjets[ dau.key() ].pt()
           << " eta: " << subjets[ dau.key() ].eta()
           << " phi: " << subjets[ dau.key() ].phi() << endl;
    }
    Ptr<reco::PFJet> jet = refToPtr( jetsjetMap[ sjet ] );
    if ( jet.isNonnull() ) {
      cout << "\tjet [" << jet.key() << "] pt: " << jet->pt() << endl;
    }
    std::cout << std::endl;
  }

  cout << "jet energy distribution/epsilon: ";
  for ( size_t i( 0 ); i != fjetHdl->size(); ++i ) {
    Ptr<reco::Jet> jet( fjetHdl, i );
    cout << jetEnergyDistrVM[ jet ] << " ";
  }
  cout << endl;

  cout << "jet momentum distribution/lambda: ";
  for ( size_t i( 0 ); i != fjetHdl->size(); ++i ) {
    Ptr<reco::Jet> jet( fjetHdl, i );
    cout << jetMomentumDistrVM[ jet ] << " ";
  }
  cout << endl;
}

void
ffTester::beginJob() {}
void
ffTester::endJob() {}

void
ffTester::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTester );