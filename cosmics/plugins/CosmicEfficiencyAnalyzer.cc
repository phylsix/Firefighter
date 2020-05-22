#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "TTree.h"

class CosmicEfficiencyAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit CosmicEfficiencyAnalyzer( const edm::ParameterSet& );
  ~CosmicEfficiencyAnalyzer() = default;
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
  void clear();

  const edm::EDGetTokenT<reco::TrackCollection>  fDSAMuonToken;
  const edm::EDGetTokenT<reco::VertexCollection> fPVToken;

  edm::Handle<reco::TrackCollection>  fDSAMuonHdl;
  edm::Handle<reco::VertexCollection> fPVHdl;

  TTree*                             fTree;
  unsigned int                       fNDSA;
  math::XYZTLorentzVectorFCollection fDSAP4;
  std::vector<float>                 fDxy;
  std::vector<float>                 fDz;
  std::vector<float>                 fNormChi2;
  std::vector<float>                 fPtErrorOverPt;
  std::vector<int>                   fCharge;
  std::vector<int>                   fNValSta;
  std::vector<int>                   fNValHit;
  std::vector<int>                   fHemisphere;
};

CosmicEfficiencyAnalyzer::CosmicEfficiencyAnalyzer( const edm::ParameterSet& ps )
    : fDSAMuonToken( consumes<reco::TrackCollection>( edm::InputTag( "displacedStandAloneMuons" ) ) ),
      fPVToken( consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) ) ) {
  edm::Service<TFileService> fs;
  fTree = fs->make<TTree>( "cosmictree", "cosmictree" );
  fTree->Branch( "dsa_n", &fNDSA );
  fTree->Branch( "dsa_p4", &fDSAP4 );
  fTree->Branch( "dsa_dxy", &fDxy );
  fTree->Branch( "dsa_dz", &fDz );
  fTree->Branch( "dsa_normchi2", &fNormChi2 );
  fTree->Branch( "dsa_pterroroverpt", &fPtErrorOverPt );
  fTree->Branch( "dsa_charge", &fCharge );
  fTree->Branch( "dsa_valStations", &fNValSta );
  fTree->Branch( "dsa_valHits", &fNValHit );
  fTree->Branch( "dsa_hemisphere", &fHemisphere )->SetTitle( "1: upper; -1: lower" );
}

void
CosmicEfficiencyAnalyzer::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fDSAMuonToken, fDSAMuonHdl );
  assert( fDSAMuonHdl.isValid() );

  e.getByToken( fPVToken, fPVHdl );
  assert( fPVHdl.isValid() );
  if ( fPVHdl->size() == 0 ) return;
  const auto& pv = *( fPVHdl->begin() );

  clear();

  for ( const auto& dsa : *fDSAMuonHdl ) {
    if ( dsa.pt() < 5 ) continue;

    fDSAP4.emplace_back( dsa.px(), dsa.py(), dsa.pz(), dsa.p() );
    fDxy.emplace_back( -dsa.dxy( pv.position() ) );
    fDz.emplace_back( -dsa.dz( pv.position() ) );
    fNormChi2.emplace_back( dsa.normalizedChi2() );
    fPtErrorOverPt.emplace_back( dsa.ptError() / dsa.pt() );
    fCharge.emplace_back( dsa.charge() );

    const auto& hitpattern = dsa.hitPattern();

    fNValSta.emplace_back( hitpattern.cscStationsWithValidHits() + hitpattern.dtStationsWithValidHits() );
    fNValHit.emplace_back( hitpattern.numberOfValidMuonCSCHits() + hitpattern.numberOfValidMuonDTHits() );
    fHemisphere.emplace_back( dsa.outerY() > 0 ? 1 : -1 );
  }
  fNDSA = fDSAP4.size();

  fTree->Fill();
}

void
CosmicEfficiencyAnalyzer::clear() {
  fNDSA = 0;
  fDSAP4.clear();
  fDxy.clear();
  fDz.clear();
  fNormChi2.clear();
  fPtErrorOverPt.clear();
  fCharge.clear();
  fNValSta.clear();
  fNValHit.clear();
  fHemisphere.clear();
}

void
CosmicEfficiencyAnalyzer::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( CosmicEfficiencyAnalyzer );