#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/objects/interface/DSAExtra.h"

class ffNtupleDsaMuon : public ffNtupleBaseNoHLT {
 public:
  ffNtupleDsaMuon( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fDSAMuonToken;
  edm::EDGetToken fPrimaryVertexToken;
  edm::EDGetToken fDSAExtraToken;

  unsigned int                       fNDSAMuon;
  math::XYZTLorentzVectorFCollection fDSAMuonP4;
  std::vector<float>                 fCharge;
  std::vector<float>                 fD0;
  std::vector<float>                 fD0Error;
  std::vector<float>                 fDz;
  std::vector<float>                 fDzError;
  std::vector<int>                   fCSCStations;
  std::vector<int>                   fDTStations;
  std::vector<int>                   fCSCHits;
  std::vector<int>                   fDTHits;
  std::vector<float>                 fPtErrorOverPt;
  std::vector<float>                 fPFIsoVal;
  std::vector<float>                 fSegOverlapRatio;
  std::vector<float>                 fExtrapolatedDr;
  std::vector<int>                   fIsSubsetAnyPFMuon;
  std::vector<int>                   fIsSubsetFilteredCosmic1Leg;
  std::vector<float>                 fNormChi2;
  std::vector<int>                   fHasOppositeMuon;
  std::vector<float>                 fTimeDiffDTCSC;
  std::vector<float>                 fTimeDiffRPC;
  std::vector<float>                 fDeltaRCosmicDSA;
  std::vector<float>                 fDeltaRCosmicEtasumDSA;
  std::vector<float>                 fDeltaRCosmicPhipidiffDSA;
  std::vector<float>                 fDeltaRCosmicSeg;
  std::vector<float>                 fDeltaRCosmicEtasumSeg;
  std::vector<float>                 fDeltaRCosmicPhipidiffSeg;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleDsaMuon, "ffNtupleDsaMuon" );

ffNtupleDsaMuon::ffNtupleDsaMuon( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleDsaMuon::initialize( TTree&                   tree,
                             const edm::ParameterSet& ps,
                             edm::ConsumesCollector&& cc ) {
  fDSAMuonToken       = cc.consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPrimaryVertexToken = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );
  fDSAExtraToken      = cc.consumes<edm::ValueMap<DSAExtra>>( edm::InputTag( "dsamuonExtra" ) );

  tree.Branch( "dsamuon_n", &fNDSAMuon );
  tree.Branch( "dsamuon_p4", &fDSAMuonP4 );
  tree.Branch( "dsamuon_charge", &fCharge );
  tree.Branch( "dsamuon_d0", &fD0 );
  tree.Branch( "dsamuon_d0Error", &fD0Error );
  tree.Branch( "dsamuon_dz", &fDz );
  tree.Branch( "dsamuon_dzError", &fDzError );
  tree.Branch( "dsamuon_CSCStations", &fCSCStations );
  tree.Branch( "dsamuon_DTStations", &fDTStations );
  tree.Branch( "dsamuon_CSCHits", &fCSCHits );
  tree.Branch( "dsamuon_DTHits", &fDTHits );
  tree.Branch( "dsamuon_ptErrorOverPt", &fPtErrorOverPt );
  tree.Branch( "dsamuon_PFIsoVal", &fPFIsoVal );
  tree.Branch( "dsamuon_segOverlapRatio", &fSegOverlapRatio );
  tree.Branch( "dsamuon_extrapolatedDr", &fExtrapolatedDr );
  tree.Branch( "dsamuon_isSubsetAnyPFMuon", &fIsSubsetAnyPFMuon )->SetTitle( "Associated Muon chamber DetId are subset of any PFMuon's" );
  tree.Branch( "dsamuon_isSubsetFilteredCosmic1Leg", &fIsSubsetFilteredCosmic1Leg )->SetTitle( "Associated DT/CSC segments are subset of filtered cosmicMuon1Leg's" );
  tree.Branch( "dsamuon_normChi2", &fNormChi2 );
  tree.Branch( "dsamuon_hasOppositeMuon", &fHasOppositeMuon )->SetTitle( "Whether this DSA muon has an opposite companion: cos(alpha)<-0.99" );
  tree.Branch( "dsamuon_timeDiffDTCSC", &fTimeDiffDTCSC )->SetTitle( "Time_upper-Time_lower from DT/CSC measurement if has opposite companion; otherwise -999." );
  tree.Branch( "dsamuon_timeDiffRPC", &fTimeDiffRPC )->SetTitle( "Time_upper-Time_lower from RPC measurement if has opposite companion; otherwise -999." );
  tree.Branch( "dsamuon_deltaRCosmicDSA", &fDeltaRCosmicDSA )->SetTitle( "dR_cosmic w/ DSA. Default: NAN" );
  tree.Branch( "dsamuon_deltaRCosmicEtasumDSA", &fDeltaRCosmicEtasumDSA )->SetTitle( "dR_cosmic w/ DSA, |eta_0+eta_1|. Default: NAN" );
  tree.Branch( "dsamuon_deltaRCosmicPhipidiffDSA", &fDeltaRCosmicPhipidiffDSA )->SetTitle( "dR_cosmic w/ DSA, M_PI-|phi_0-phi_1|. Default: NAN" );
  tree.Branch( "dsamuon_deltaRCosmicSeg", &fDeltaRCosmicSeg )->SetTitle( "dR_cosmic w/ segments. Default: NAN" );
  tree.Branch( "dsamuon_deltaRCosmicEtasumSeg", &fDeltaRCosmicEtasumSeg )->SetTitle( "dR_cosmic w/ segments, |eta_0+eta_1|. Default: NAN" );
  tree.Branch( "dsamuon_deltaRCosmicPhipidiffSeg", &fDeltaRCosmicPhipidiffSeg )->SetTitle( "dR_cosmic w/ segments, M_PI-|phi_0-phi_1|. Default: NAN" );
}

void
ffNtupleDsaMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> dsamuonHdl;
  e.getByToken( fDSAMuonToken, dsamuonHdl );
  assert( dsamuonHdl.isValid() );

  Handle<reco::VertexCollection> primaryVertexHdl;
  e.getByToken( fPrimaryVertexToken, primaryVertexHdl );
  assert( primaryVertexHdl.isValid() && primaryVertexHdl->size() > 0 );
  const auto& pv = *( primaryVertexHdl->begin() );

  Handle<ValueMap<DSAExtra>> dsaExtraHdl;
  e.getByToken( fDSAExtraToken, dsaExtraHdl );
  assert( dsaExtraHdl.isValid() );

  clear();

  fNDSAMuon = dsamuonHdl->size();
  for ( size_t i( 0 ); i != dsamuonHdl->size(); i++ ) {
    reco::MuonRef dsamuonref( dsamuonHdl, i );
    const auto&   dsamuon    = *dsamuonref;
    const auto&   trackref   = dsamuon.outerTrack();
    const auto&   hitpattern = trackref->hitPattern();
    const auto&   dsaextra   = ( *dsaExtraHdl )[ dsamuonref ];

    fDSAMuonP4.emplace_back( dsamuon.px(), dsamuon.py(), dsamuon.pz(), dsamuon.energy() );
    fCharge.emplace_back( dsamuon.charge() );
    fD0.emplace_back( -trackref->dxy( pv.position() ) );
    fD0Error.emplace_back( trackref->dxyError() );
    fDz.emplace_back( -trackref->dz( pv.position() ) );
    fDzError.emplace_back( trackref->dzError() );
    fCSCStations.emplace_back( hitpattern.cscStationsWithValidHits() );
    fDTStations.emplace_back( hitpattern.dtStationsWithValidHits() );
    fCSCHits.emplace_back( hitpattern.numberOfValidMuonCSCHits() );
    fDTHits.emplace_back( hitpattern.numberOfValidMuonDTHits() );
    fPtErrorOverPt.emplace_back( trackref->ptError() / trackref->pt() );
    fNormChi2.emplace_back( trackref->normalizedChi2() );

    fPFIsoVal.emplace_back( dsaextra.pfiso04() );
    fSegOverlapRatio.emplace_back( dsaextra.pfmuon_maxSegmentOverlapRatio() );
    fExtrapolatedDr.emplace_back( dsaextra.pfmuon_minLocalDeltaRAtInnermost() );
    fIsSubsetAnyPFMuon.emplace_back( dsaextra.pfmuon_detIdSubsetOfAny() );
    fIsSubsetFilteredCosmic1Leg.emplace_back( dsaextra.cosmci1leg_detIdSubsetOfFiltered() );
    fHasOppositeMuon.emplace_back( dsaextra.oppositeMuon().isNonnull() );
    fTimeDiffDTCSC.emplace_back( dsaextra.oppositeTimeDiff_dtcsc() );
    fTimeDiffRPC.emplace_back( dsaextra.oppositeTimeDiff_rpc() );
    fDeltaRCosmicDSA.emplace_back( dsaextra.dsamuon_minDeltaRCosmic() );
    fDeltaRCosmicEtasumDSA.emplace_back( dsaextra.dsamuon_minDeltaRCosmicEtasum() );
    fDeltaRCosmicPhipidiffDSA.emplace_back( dsaextra.dsamuon_minDeltaRCosmicPhipidiff() );
    fDeltaRCosmicSeg.emplace_back( dsaextra.segment_minDeltaRCosmic() );
    fDeltaRCosmicEtasumSeg.emplace_back( dsaextra.segment_minDeltaRCosmicEtasum() );
    fDeltaRCosmicPhipidiffSeg.emplace_back( dsaextra.segment_minDeltaRCosmicPhipidiff() );
  }
}

void
ffNtupleDsaMuon::clear() {
  fNDSAMuon = 0;
  fDSAMuonP4.clear();
  fCharge.clear();
  fD0.clear();
  fD0Error.clear();
  fDz.clear();
  fDzError.clear();
  fCSCStations.clear();
  fDTStations.clear();
  fCSCHits.clear();
  fDTHits.clear();
  fPtErrorOverPt.clear();
  fPFIsoVal.clear();
  fSegOverlapRatio.clear();
  fExtrapolatedDr.clear();
  fIsSubsetAnyPFMuon.clear();
  fIsSubsetFilteredCosmic1Leg.clear();
  fNormChi2.clear();
  fHasOppositeMuon.clear();
  fTimeDiffDTCSC.clear();
  fTimeDiffRPC.clear();
  fDeltaRCosmicDSA.clear();
  fDeltaRCosmicEtasumDSA.clear();
  fDeltaRCosmicPhipidiffDSA.clear();
  fDeltaRCosmicSeg.clear();
  fDeltaRCosmicEtasumSeg.clear();
  fDeltaRCosmicPhipidiffSeg.clear();
}