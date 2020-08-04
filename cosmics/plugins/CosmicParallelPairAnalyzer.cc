#include "CommonTools/UtilAlgos/interface/TFileService.h"
// #include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
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
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
// #include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include "Firefighter/cosmics/interface/CosmicHelper.h"
#include "TTree.h"
#include <cmath>


class CosmicParallelPairAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
 public:
  explicit CosmicParallelPairAnalyzer( const edm::ParameterSet& );
  ~CosmicParallelPairAnalyzer() = default;
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
  void clear();

  const edm::EDGetTokenT<int>  fCosmicNppToken;
  const edm::EDGetTokenT<reco::TrackCollection>  fCosmicMuonToken;
  const edm::EDGetTokenT<reco::VertexCollection> fPVToken;
  const edm::EDGetTokenT<DTRecSegment4DCollection> fDTSegToken;

  edm::Handle<int>  fCosmicNppHdl;
  edm::Handle<reco::TrackCollection>  fCosmicMuonHdl;
  edm::Handle<reco::VertexCollection> fPVHdl;
  edm::Handle<DTRecSegment4DCollection> fDTSegHdl;


  TTree* fTree;
  int fNpp;
  int fNAccCosmic;
  int fNGoodPV;
  std::vector<float> fCosmicPt;
  std::vector<float> fCosmicEta;
  std::vector<float> fCosmicPhi;
  std::vector<float> fCosmicMinDr;
  std::vector<float> fCosmicMinDrCos;
  std::vector<float> fCosmicMinDrCosSeg;
  std::vector<float> fCosmicMinDca;
  std::vector<float> fCosmicDxy;
  std::vector<float> fCosmicDz;
  std::vector<bool>  fCosmicPassDSASelection;
  std::vector<float> fCosmicPairDca;
  std::vector<float> fCosmicPairDrCosmic;
};

CosmicParallelPairAnalyzer::CosmicParallelPairAnalyzer( const edm::ParameterSet& ps )
    : fCosmicNppToken( consumes<int>( edm::InputTag( "ffcosmiceventfilter" ) ) ),
      fCosmicMuonToken( consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>("src") ) ),
      fPVToken( consumes<reco::VertexCollection>( edm::InputTag( "goodOfflinePrimaryVertices" ) ) ),
      fDTSegToken( consumes<DTRecSegment4DCollection>( edm::InputTag( "dt4DSegments" ) ) ) {

  edm::Service<TFileService> fs;
  fTree = fs->make<TTree>("cosmictree", "cosmictree");
  fTree->Branch( "event_ngoodpv", &fNGoodPV );
  fTree->Branch( "event_npp",     &fNpp );
  fTree->Branch( "cosmic_n",      &fNAccCosmic );
  fTree->Branch( "cosmic_pt",     &fCosmicPt );
  fTree->Branch( "cosmic_eta",    &fCosmicEta );
  fTree->Branch( "cosmic_phi",    &fCosmicPhi );
  fTree->Branch( "cosmic_dxy",    &fCosmicDxy );
  fTree->Branch( "cosmic_dz",     &fCosmicDz );
  fTree->Branch( "cosmic_minDr",       &fCosmicMinDr)->SetTitle("min(DR), if >=2 cosmic tracks, otherwise -1.");
  fTree->Branch( "cosmic_minDrCosmic", &fCosmicMinDrCos)->SetTitle("min(DR_{cosmic}), if opposite hemi, otherwise -1.");
  fTree->Branch( "cosmic_minDrCosSeg", &fCosmicMinDrCosSeg)->SetTitle("min(DR_{cosmic}, segment), if opposite hemi, otherwise -1.");
  fTree->Branch( "cosmic_minDCA",      &fCosmicMinDca)->SetTitle("min(DCA), if >=2 cosmic tracks, otherwise -1.");
  fTree->Branch( "cosmic_passDSASelection", &fCosmicPassDSASelection);
  fTree->Branch( "cosmicpair_DCA", &fCosmicPairDca);
  fTree->Branch( "cosmicpair_DrCosmic", &fCosmicPairDrCosmic);

  // fNpp = fs->make<TH1F>("nppcosmic", "N(parallel pairs) of cosmic muons;N(pairs);counts/1", 20, 0, 20);

}


void
CosmicParallelPairAnalyzer::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;

  e.getByToken( fCosmicNppToken, fCosmicNppHdl );
  assert( fCosmicNppHdl.isValid() );
  e.getByToken( fCosmicMuonToken, fCosmicMuonHdl );
  assert( fCosmicMuonHdl.isValid() );
  e.getByToken( fPVToken, fPVHdl );
  assert( fPVHdl.isValid() );
  e.getByToken( fDTSegToken, fDTSegHdl );
  assert( fDTSegHdl.isValid() );


  clear();

  fNpp = *fCosmicNppHdl;
  fNAccCosmic = fCosmicMuonHdl->size();
  fNGoodPV = fPVHdl->size();

  for (const auto& ctk : *fCosmicMuonHdl) {
    fCosmicPt.emplace_back( ctk.pt() );
    fCosmicEta.emplace_back( ctk.eta() );
    fCosmicPhi.emplace_back( ctk.phi() );
    fCosmicDxy.emplace_back( ctk.dxy() );
    fCosmicDz .emplace_back( ctk.dz() );

    fCosmicPassDSASelection.emplace_back( CosmicHelper::passDSApreselection(ctk) );


    double minDr_(-1), minDrCos_(-1), minDca_(-1);
    for (const auto& ctk2 : *fCosmicMuonHdl) {
      if (&ctk==&ctk2) continue; // same track

      // min DCA
      std::pair<bool, double> pairDcaResult = CosmicHelper::twoTrackMinDistance(ctk, ctk2, es);
      minDca_ = minDca_==-1 ? pairDcaResult.second : min(minDca_, pairDcaResult.second);

      // DR_cosmic
      if (CosmicHelper::oppositeHemisphere(ctk, ctk2)) {
        auto pairDrCosmic = CosmicHelper::cosmicDeltaR(ctk, ctk2);
        minDrCos_ = minDrCos_==-1 ? pairDrCosmic : min(minDrCos_, pairDrCosmic);
      }

      // DR
      auto pairDr = deltaR(ctk, ctk2);
      minDr_ = minDr_==-1 ? pairDr : min(minDr_, pairDr);
    }
    fCosmicMinDr.emplace_back( minDr_ );
    fCosmicMinDrCos.emplace_back( minDrCos_ );
    fCosmicMinDca.emplace_back( minDca_ );


    double minDrCosSeg_(-1);
    for ( const DTRecSegment4D& dtSeg : *fDTSegHdl ) {
      // DR_cosmic_seg
      if ( !CosmicHelper::oppositeHemisphere( ctk, dtSeg, es ) ) continue;

      pair<bool, tuple<double, GlobalVector, GlobalVector>> propagationResult = CosmicHelper::propagateDSAtoDT( ctk, dtSeg, es );
      if ( !propagationResult.first ) continue;
      const auto& ctkGlbDir   = get<1>( propagationResult.second );
      const auto& dtsegGlbDir = get<2>( propagationResult.second );

      double etaSum    = fabs( ctkGlbDir.eta() + dtsegGlbDir.eta() );
      double phiPiDiff = M_PI - fabs( deltaPhi( ctkGlbDir.barePhi(), dtsegGlbDir.barePhi() ) );

      auto segDrCosmic = hypot( etaSum, phiPiDiff );
      minDrCosSeg_ = minDrCosSeg_==-1 ? segDrCosmic : min(minDrCosSeg_, segDrCosmic);
    }
    fCosmicMinDrCosSeg.emplace_back(minDrCosSeg_);

  }

  for (auto iCosmic = fCosmicMuonHdl->cbegin(); iCosmic!=fCosmicMuonHdl->cend(); iCosmic++) {
    for (auto jCosmic = std::next(iCosmic); jCosmic!=fCosmicMuonHdl->cend(); jCosmic++) {
      std::pair<bool, double> tkdistResult = CosmicHelper::twoTrackMinDistance(*iCosmic, *jCosmic, es);
      fCosmicPairDca.emplace_back( tkdistResult.second );
      if (CosmicHelper::oppositeHemisphere(*iCosmic, *jCosmic))
        fCosmicPairDrCosmic.emplace_back( CosmicHelper::cosmicDeltaR(*iCosmic, *jCosmic) );
    }
  }

  fTree->Fill();

}

void
CosmicParallelPairAnalyzer::clear() {
  fCosmicPt.clear();
  fCosmicEta.clear();
  fCosmicPhi.clear();
  fCosmicMinDr.clear();
  fCosmicMinDrCos.clear();
  fCosmicMinDrCosSeg.clear();
  fCosmicMinDca.clear();
  fCosmicDxy.clear();
  fCosmicDz.clear();
  fCosmicPassDSASelection.clear();
  fCosmicPairDca.clear();
  fCosmicPairDrCosmic.clear();
}

void
CosmicParallelPairAnalyzer::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( CosmicParallelPairAnalyzer );