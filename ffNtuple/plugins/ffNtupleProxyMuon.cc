#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"
#include "Firefighter/recoStuff/interface/ffPFJetProcessors.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtupleProxyMuon : public ffNtupleBaseNoHLT {
 public:
  ffNtupleProxyMuon( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fLjSrcToken;
  edm::EDGetToken fLeptonjetToken;
  edm::EDGetToken fGeneralTkToken;
  edm::EDGetToken fPFCandNoPUToken;

  unsigned int                         fNProxies;
  std::vector<LorentzVector>           fProxyP4;
  std::vector<int>                     fProxyType;
  std::vector<unsigned int>            fProxyRefIdx;
  std::map<double, std::vector<float>> fProxyPFIsolationNoPU;

  std::vector<double> fIsoRadius;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleProxyMuon, "ffNtupleProxyMuon" );

ffNtupleProxyMuon::ffNtupleProxyMuon( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      fIsoRadius( ps.getParameter<std::vector<double>>( "IsolationRadius" ) ) {}

void
ffNtupleProxyMuon::initialize( TTree&                   tree,
                               const edm::ParameterSet& ps,
                               edm::ConsumesCollector&& cc ) {
  fLjSrcToken      = cc.consumes<reco::PFCandidateFwdPtrVector>( ps.getParameter<edm::InputTag>( "src" ) );
  fLeptonjetToken  = cc.consumes<reco::PFJetCollection>( ps.getParameter<edm::InputTag>( "leptonjet" ) );
  fGeneralTkToken  = cc.consumes<reco::TrackCollection>( edm::InputTag( "generalTracks" ) );
  fPFCandNoPUToken = cc.consumes<reco::PFCandidateFwdPtrVector>( edm::InputTag( "pfNoPileUpIso" ) );

  tree.Branch( "proxymuon_n", &fNProxies );
  tree.Branch( "proxymuon_p4", &fProxyP4 );
  tree.Branch( "proxymuon_type", &fProxyType )->SetTitle( "Proxy muon's PF type, 3: PFMuon, 8: DSA." );
  tree.Branch( "proxymuon_refIdx", &fProxyRefIdx )->SetTitle( "ref index. If ref null, set as 999." );
  for ( const auto& isor : fIsoRadius ) {
    fProxyPFIsolationNoPU[ isor ] = {};
    std::stringstream ss;
    ss << isor;
    std::string suffix = ss.str().replace( 1, 1, "" );
    tree.Branch( Form( "proxymuon_pfIsolationNoPU%s", suffix.c_str() ), &fProxyPFIsolationNoPU[ isor ] )->SetTitle( Form( "sum PFCandidates(<b>noMu</b>) energy within cone radius %.1f with proxy muon footprint removed, divided by sum of both", isor ) );
  }
}

void
ffNtupleProxyMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;
  using namespace ff;

  Handle<reco::PFCandidateFwdPtrVector> ljsrcHdl;
  e.getByToken( fLjSrcToken, ljsrcHdl );
  assert( ljsrcHdl.isValid() );

  Handle<reco::PFJetCollection> leptonjetHdl;
  e.getByToken( fLeptonjetToken, leptonjetHdl );
  assert( leptonjetHdl.isValid() );

  Handle<reco::TrackCollection> generalTkHdl;
  e.getByToken( fGeneralTkToken, generalTkHdl );
  assert( generalTkHdl.isValid() );

  Handle<reco::PFCandidateFwdPtrVector> pfCandNoPUHdl;
  e.getByToken( fPFCandNoPUToken, pfCandNoPUHdl );
  assert( pfCandNoPUHdl.isValid() );
  clear();

  vector<reco::PFCandidatePtr> muonDaughters{};
  for ( const auto& lj : *leptonjetHdl ) {
    if ( sumPFMuonCharge( lj ) != 0 ) continue;  // we care muon charge neutral LJ only!
    for ( const auto& cand : getPFCands( lj ) ) {
      if ( cand->particleId() != reco::PFCandidate::mu ) continue;
      muonDaughters.push_back( cand );
    }
  }

  for ( const auto& ljsrc : *ljsrcHdl ) {
    const auto& cand = *( ljsrc.get() );
    if ( cand.particleId() != reco::PFCandidate::mu ) continue;
    if ( find( muonDaughters.begin(), muonDaughters.end(), ljsrc.ptr() ) != muonDaughters.end() ) continue;

    fProxyP4.push_back( LorentzVector( cand.px(), cand.py(), cand.pz(), cand.energy() ) );
    fProxyType.emplace_back( getCandType( ljsrc.ptr(), generalTkHdl ) );
    fProxyRefIdx.emplace_back( cand.muonRef().isNonnull() ? cand.muonRef().key() : 999 );

    for ( const auto& isor : fIsoRadius ) {
      fProxyPFIsolationNoPU[ isor ].emplace_back( getCandPFIsolation( ljsrc.ptr(), pfCandNoPUHdl, isor ) );
    }
  }

  fNProxies = fProxyP4.size();
}

void
ffNtupleProxyMuon::clear() {
  fNProxies = 0;
  fProxyP4.clear();
  fProxyType.clear();
  fProxyRefIdx.clear();
  for ( const auto& isor : fIsoRadius ) {
    fProxyPFIsolationNoPU[ isor ].clear();
  }
}