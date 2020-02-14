#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonSimInfo.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

class ffNtupleMuon : public ffNtupleBaseNoHLT {
 public:
  ffNtupleMuon( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fMuonToken;
  edm::EDGetToken fPrimaryVertexToken;
  edm::EDGetToken fMuonSimInfoToken;

  unsigned int                       fNMuon;
  math::XYZTLorentzVectorFCollection fMuonP4;
  std::vector<int>                   fMuonCharge;
  std::vector<unsigned int>          fMuonType;
  std::vector<unsigned int>          fBestTrackType;
  std::vector<unsigned int>          fSelectors;
  std::vector<int>                   fHasInnerTrack;
  std::vector<int>                   fHasOuterTrack;
  std::vector<float>                 fD0;
  std::vector<float>                 fD0Sig;
  std::vector<float>                 fDz;
  std::vector<float>                 fDzSig;
  std::vector<float>                 fNormChi2;
  std::vector<float>                 fDtCscTime;
  std::vector<float>                 fRpcTime;
  std::vector<float>                 fIsoValue;
  std::vector<int>                   fSimType;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleMuon, "ffNtupleMuon" );

ffNtupleMuon::ffNtupleMuon( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleMuon::initialize( TTree&                   tree,
                          const edm::ParameterSet& ps,
                          edm::ConsumesCollector&& cc ) {
  fMuonToken          = cc.consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPrimaryVertexToken = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );
  fMuonSimInfoToken   = cc.consumes<edm::ValueMap<reco::MuonSimInfo>>( edm::InputTag( "muonSimClassifier" ) );

  tree.Branch( "muon_n", &fNMuon );
  tree.Branch( "muon_p4", &fMuonP4 );
  tree.Branch( "muon_charge", &fMuonCharge );
  tree.Branch( "muon_type", &fMuonType )->SetTitle( "<b>MuonType</b>: Global, Tracker, Standalone, Calo, PFMuon, RPC, GEM, ME0" );
  tree.Branch( "muon_bestTrackType", &fBestTrackType )->SetTitle( "<b>MuonTrackType</b>: None, Inner, Outer, Combined, TPFMS, Picky, DYT" );
  tree.Branch( "muon_selectors", &fSelectors )->SetTitle( "ID/Iso flags encoded as bitmap" );
  tree.Branch( "muon_hasInnerTrack", &fHasInnerTrack );
  tree.Branch( "muon_hasOuterTrack", &fHasOuterTrack );
  tree.Branch( "muon_d0", &fD0 );
  tree.Branch( "muon_d0Sig", &fD0Sig )->SetTitle( "d0 significance" );
  tree.Branch( "muon_dz", &fDz );
  tree.Branch( "muon_dzSig", &fDzSig )->SetTitle( "dz significance" );
  tree.Branch( "muon_normChi2", &fNormChi2 );
  tree.Branch( "muon_dtCscTime", &fDtCscTime )->SetTitle( "timing info from DT/CSC combined measurements" );
  tree.Branch( "muon_rpcTime", &fRpcTime )->SetTitle( "timing info from RPC" );
  tree.Branch( "muon_isoValue", &fIsoValue )->SetTitle( "pfIsoR04, delta-beta PU correction" );
  tree.Branch( "muon_simType", &fSimType )->SetTitle( "<b>MC-only</b> sim type inferred from gen association, check TWiki/Code" );
}

void
ffNtupleMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> muonHdl;
  e.getByToken( fMuonToken, muonHdl );
  assert( muonHdl.isValid() );

  Handle<reco::VertexCollection> primaryVertexHdl;
  e.getByToken( fPrimaryVertexToken, primaryVertexHdl );
  assert( primaryVertexHdl.isValid() && primaryVertexHdl->size() > 0 );
  const auto& pv = *( primaryVertexHdl->begin() );

  clear();

  fNMuon = muonHdl->size();
  for ( const auto& muon : *muonHdl ) {
    fMuonP4.emplace_back( muon.px(), muon.py(), muon.pz(), muon.energy() );
    fMuonCharge.emplace_back( muon.charge() );
    fMuonType.emplace_back( muon.type() );
    fBestTrackType.emplace_back( muon.muonBestTrackType() );
    fSelectors.emplace_back( muon.selectors() );
    fHasInnerTrack.emplace_back( muon.innerTrack().isNonnull() );
    fHasOuterTrack.emplace_back( muon.outerTrack().isNonnull() );

    const reco::Track* mutrack = muon.bestTrack();
    fD0.emplace_back( mutrack != nullptr ? -mutrack->dxy( pv.position() ) : NAN );
    fD0Sig.emplace_back( mutrack != nullptr ? fabs( -mutrack->dxy( pv.position() ) ) / mutrack->dxyError() : NAN );
    fDz.emplace_back( mutrack != nullptr ? mutrack->dz( pv.position() ) : NAN );
    fDzSig.emplace_back( mutrack != nullptr ? fabs( mutrack->dz( pv.position() ) ) / mutrack->dzError() : NAN );
    fNormChi2.emplace_back( mutrack != nullptr && mutrack->ndof() != 0 ? mutrack->normalizedChi2() : NAN );

    fDtCscTime.emplace_back( muon.time().timeAtIpInOut );
    fRpcTime.emplace_back( muon.rpcTime().timeAtIpInOut );
    fIsoValue.emplace_back( ff::getMuonIsolationValue( muon ) );
  }

  Handle<ValueMap<reco::MuonSimInfo>> muonSimInfoHdl;
  e.getByToken( fMuonSimInfoToken, muonSimInfoHdl );
  if ( muonSimInfoHdl.isValid() ) {
    for ( size_t i( 0 ); i != muonHdl->size(); i++ ) {
      Ptr<reco::Muon> muonptr( muonHdl, i );
      fSimType.emplace_back( ( *muonSimInfoHdl )[ muonptr ].primaryClass );
    }
  }
}

void
ffNtupleMuon::clear() {
  fNMuon = 0;
  fMuonP4.clear();
  fMuonCharge.clear();
  fMuonType.clear();
  fBestTrackType.clear();
  fSelectors.clear();
  fHasInnerTrack.clear();
  fHasOuterTrack.clear();
  fD0.clear();
  fD0Sig.clear();
  fDz.clear();
  fDzSig.clear();
  fNormChi2.clear();
  fDtCscTime.clear();
  fRpcTime.clear();
  fIsoValue.clear();
  fSimType.clear();
}

namespace {
/// ***********************************************
enum MuonTrackType {
  None,
  InnerTrack,
  OuterTrack,
  CombinedTrack,
  TPFMS,
  Picky,
  DYT
};

/// ***********************************************
enum Selector {
  CutBasedIdLoose        = 1UL << 0,
  CutBasedIdMedium       = 1UL << 1,
  CutBasedIdMediumPrompt = 1UL << 2,  // medium with IP cuts
  CutBasedIdTight        = 1UL << 3,
  CutBasedIdGlobalHighPt = 1UL << 4,   // high pt muon for Z',W' (better momentum resolution)
  CutBasedIdTrkHighPt    = 1UL << 5,   // high pt muon for boosted Z (better efficiency)
  PFIsoVeryLoose         = 1UL << 6,   // reliso<0.40
  PFIsoLoose             = 1UL << 7,   // reliso<0.25
  PFIsoMedium            = 1UL << 8,   // reliso<0.20
  PFIsoTight             = 1UL << 9,   // reliso<0.15
  PFIsoVeryTight         = 1UL << 10,  // reliso<0.10
  TkIsoLoose             = 1UL << 11,  // reliso<0.10
  TkIsoTight             = 1UL << 12,  // reliso<0.05
  SoftCutBasedId         = 1UL << 13,
  SoftMvaId              = 1UL << 14,
  MvaLoose               = 1UL << 15,
  MvaMedium              = 1UL << 16,
  MvaTight               = 1UL << 17,
  MiniIsoLoose           = 1UL << 18,  // reliso<0.40
  MiniIsoMedium          = 1UL << 19,  // reliso<0.20
  MiniIsoTight           = 1UL << 20,  // reliso<0.10
  MiniIsoVeryTight       = 1UL << 21,  // reliso<0.05
  TriggerIdLoose         = 1UL << 22,  // robust selector for HLT
  InTimeMuon             = 1UL << 23,
  PFIsoVeryVeryTight     = 1UL << 24,  // reliso<0.05
  MultiIsoLoose          = 1UL << 25,  // miniIso with ptRatio and ptRel
  MultiIsoMedium         = 1UL << 26   // miniIso with ptRatio and ptRel
};

/// ***********************************************
/// muon type - type of the algorithm that reconstructed this muon
/// multiple algorithms can reconstruct the same muon
enum MuonType {
  GlobalMuon     = 1 << 1,
  TrackerMuon    = 1 << 2,
  StandAloneMuon = 1 << 3,
  CaloMuon       = 1 << 4,
  PFMuon         = 1 << 5,
  RPCMuon        = 1 << 6,
  GEMMuon        = 1 << 7,
  ME0Muon        = 1 << 8
};

/// ***********************************************
/// https://cmssdt.cern.ch/dxr/CMSSW/source/DataFormats/MuonReco/interface/MuonSimInfo.h
enum MuonSimType {
  Unknown                     = 999,
  NotMatched                  = 0,
  MatchedPunchthrough         = 1,
  MatchedElectron             = 11,
  MatchedPrimaryMuon          = 4,
  MatchedMuonFromHeavyFlavour = 3,
  MatchedMuonFromLightFlavour = 2,
  GhostPunchthrough           = -1,
  GhostElectron               = -11,
  GhostPrimaryMuon            = -4,
  GhostMuonFromHeavyFlavour   = -3,
  GhostMuonFromLightFlavour   = -2
};
}  // namespace