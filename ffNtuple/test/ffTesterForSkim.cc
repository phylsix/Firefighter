// framework headers
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

// dataformats
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

class ffTesterForSkim : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit ffTesterForSkim( const edm::ParameterSet& );
  ~ffTesterForSkim();

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );
  int         numberOfDsaMuon( const reco::PFJet&                        jet,
                               const edm::Handle<reco::TrackCollection>& tkHdl ) const;

 private:
  virtual void beginJob() override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::JetView>                           fJetToken;
  edm::EDGetTokenT<reco::JetView>                           fSjetToken;
  edm::EDGetTokenT<reco::PFJetCollection>                   fSubjetToken;
  edm::EDGetTokenT<reco::PFJetCollection>                   fPfjetToken;
  edm::EDGetTokenT<reco::TrackCollection>                   fTkToken;
  edm::EDGetTokenT<edm::Association<reco::PFJetCollection>> fJetsjetMapToken;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetEnergyDistrToken;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetMomentumDistrToken;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetEcf1Token;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetEcf2Token;
  edm::EDGetTokenT<edm::ValueMap<float>> fJetEcf3Token;

  edm::Handle<reco::JetView>                           fjetHdl;
  edm::Handle<reco::JetView>                           fSjetHdl;
  edm::Handle<reco::PFJetCollection>                   fSubjetHdl;
  edm::Handle<reco::PFJetCollection>                   fPfjetHdl;
  edm::Handle<reco::TrackCollection>                   fTkHdl;
  edm::Handle<edm::Association<reco::PFJetCollection>> fJetsjetMapHdl;

  edm::Handle<edm::ValueMap<float>> fJetEnergyDistrHdl;
  edm::Handle<edm::ValueMap<float>> fJetMomentumDistrHdl;
  edm::Handle<edm::ValueMap<float>> fJetEcf1Hdl;
  edm::Handle<edm::ValueMap<float>> fJetEcf2Hdl;
  edm::Handle<edm::ValueMap<float>> fJetEcf3Hdl;

  void debugJetSubjetId( const edm::Event& e );
  void debugJetSubstructureVariables( const edm::Event& e );
  void debugJetMuonMultiplicity( const edm::Event& e );

  edm::EDGetTokenT<reco::MuonCollection>  fMuonFromDSAToken;
  edm::EDGetTokenT<reco::TrackCollection> fDsaMuonTrackToken;
  edm::Handle<reco::MuonCollection>       fMuonsFromDSAHdl;
  edm::Handle<reco::TrackCollection>      fDsaMuonTrackHdl;
  void debugMuonsFromDSA( const edm::Event& e );
};

ffTesterForSkim::ffTesterForSkim( const edm::ParameterSet& iC ) {
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
  fJetEcf1Token = consumes<edm::ValueMap<float>>(
      edm::InputTag( "ffLeptonJetSubjetECF", "ecf1" ) );
  fJetEcf2Token = consumes<edm::ValueMap<float>>(
      edm::InputTag( "ffLeptonJetSubjetECF", "ecf2" ) );
  fJetEcf3Token = consumes<edm::ValueMap<float>>(
      edm::InputTag( "ffLeptonJetSubjetECF", "ecf3" ) );
  fPfjetToken =
      consumes<reco::PFJetCollection>( edm::InputTag( "ffLeptonJet" ) );
  fTkToken =
      consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) );

  fMuonFromDSAToken =
      consumes<reco::MuonCollection>( edm::InputTag( "muonsFromdSA" ) );
  fDsaMuonTrackToken = consumes<reco::TrackCollection>(
      edm::InputTag( "displacedStandAloneMuons" ) );
}

ffTesterForSkim::~ffTesterForSkim() {}

void
ffTesterForSkim::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;

  cout << "++++ ++++" << e.id().run() << " : " << e.luminosityBlock() << " : "
       << e.id().event() << "++++ ++++" << endl;

  // debugJetSubjetId( e );
  // debugJetSubstructureVariables( e );
  // debugJetMuonMultiplicity( e );
  debugMuonsFromDSA( e );

  cout << "********************************" << endl;
}

void
ffTesterForSkim::beginJob() {}
void
ffTesterForSkim::endJob() {}

void
ffTesterForSkim::debugJetSubjetId( const edm::Event& e ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fJetToken, fjetHdl );
  assert( fjetHdl.isValid() );
  e.getByToken( fSjetToken, fSjetHdl );
  assert( fSjetHdl.isValid() );
  e.getByToken( fSubjetToken, fSubjetHdl );
  assert( fSubjetHdl.isValid() );
  e.getByToken( fJetsjetMapToken, fJetsjetMapHdl );
  assert( fJetsjetMapHdl.isValid() );

  const auto& subjets    = *fSubjetHdl;
  const auto& jetsjetMap = *fJetsjetMapHdl;

  cout << "jet prodId: " << fjetHdl.id() << " sjet prodId:" << fSjetHdl.id()
       << ", the Assocation contains ? " << jetsjetMap.contains( fSjetHdl.id() )
       << endl;
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
}

void
ffTesterForSkim::debugJetSubstructureVariables( const edm::Event& e ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fJetEnergyDistrToken, fJetEnergyDistrHdl );
  assert( fJetEnergyDistrHdl.isValid() );
  e.getByToken( fJetMomentumDistrToken, fJetMomentumDistrHdl );
  assert( fJetMomentumDistrHdl.isValid() );
  e.getByToken( fJetEcf1Token, fJetEcf1Hdl );
  assert( fJetEcf1Hdl.isValid() );
  e.getByToken( fJetEcf2Token, fJetEcf2Hdl );
  assert( fJetEcf2Hdl.isValid() );
  e.getByToken( fJetEcf3Token, fJetEcf3Hdl );
  assert( fJetEcf3Hdl.isValid() );

  const auto& jetEnergyDistrVM   = *fJetEnergyDistrHdl;
  const auto& jetMomentumDistrVM = *fJetMomentumDistrHdl;

  const auto& jetecf1VM = *fJetEcf1Hdl;
  const auto& jetecf2VM = *fJetEcf2Hdl;
  const auto& jetecf3VM = *fJetEcf3Hdl;

  vector<Ptr<reco::Jet>> jetptrs{};
  for ( size_t i( 0 ); i != fjetHdl->size(); ++i ) {
    jetptrs.emplace_back( fjetHdl, i );
  }
  cout << "jet energy distribution/epsilon: ";
  for ( const auto& jet : jetptrs ) {
    cout << jetEnergyDistrVM[ jet ] << " ";
  }
  cout << endl;

  cout << "jet momentum distribution/lambda: ";
  for ( const auto& jet : jetptrs ) {
    cout << jetMomentumDistrVM[ jet ] << " ";
  }
  cout << endl;

  cout << "jet ECF 1: ";
  for ( const auto& jet : jetptrs ) {
    cout << jetecf1VM[ jet ] << " ";
  }
  cout << endl;

  cout << "jet ECF 2: ";
  for ( const auto& jet : jetptrs ) {
    cout << jetecf2VM[ jet ] << " ";
  }
  cout << endl;

  cout << "jet ECF 3: ";
  for ( const auto& jet : jetptrs ) {
    cout << jetecf3VM[ jet ] << " ";
  }
  cout << endl;
}

void
ffTesterForSkim::debugJetMuonMultiplicity( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fPfjetToken, fPfjetHdl );
  assert( fPfjetHdl.isValid() );
  e.getByToken( fTkToken, fTkHdl );
  assert( fTkHdl.isValid() );

  for ( const auto& jet : *fPfjetHdl ) {
    int muonCounts( 0 );
    for ( const auto& c : jet.getPFConstituents() ) {
      if ( abs( c->pdgId() ) == 13 )
        muonCounts++;
    }
    cout << "#dSA: " << numberOfDsaMuon( jet, fTkHdl )
         << "\tmuonCnts: " << muonCounts
         << "\tmuonMultiplicity: " << jet.muonMultiplicity() << endl;
  }
}

void
ffTesterForSkim::debugMuonsFromDSA( const edm::Event& e ) {
  using namespace std;

  e.getByToken( fMuonFromDSAToken, fMuonsFromDSAHdl );
  assert( fMuonsFromDSAHdl.isValid() );
  e.getByToken( fDsaMuonTrackToken, fDsaMuonTrackHdl );
  assert( fDsaMuonTrackHdl.isValid() );

  cout << "[size]\tmuonsFromdSA|dSA:\t" << fMuonsFromDSAHdl->size() << "|"
       << fDsaMuonTrackHdl->size() << endl;
  cout << "[dSA id]\t" << fDsaMuonTrackHdl.id() << endl;
  cout << "[muonsFromdSA] <bestTrackType/muonBestTrack.id/muonBestTrack.key>\t";
  for ( const auto& dmu : *fMuonsFromDSAHdl ) {
    cout << dmu.muonBestTrackType() << "/" << dmu.muonBestTrack().id() << "/"
         << dmu.muonBestTrack().key() << ", ";
  }
  cout << endl;
}

int
ffTesterForSkim::numberOfDsaMuon(
    const reco::PFJet&                        jet,
    const edm::Handle<reco::TrackCollection>& tkHdl ) const {
  int result( 0 );
  for ( const auto& cptr : jet.getPFConstituents() ) {
    if ( cptr.isNull() )
      continue;
    if ( cptr->bestTrack() == nullptr )
      continue;
    if ( cptr->trackRef().isNonnull() and cptr->trackRef().id() != tkHdl.id() )
      result++;
  }
  return result;
}

void
ffTesterForSkim::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterForSkim );
