#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

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

  edm::EDGetToken fSegOverlapRatioToken;
  edm::EDGetToken fExtrapolatedDrToken;
  edm::EDGetToken fIsSubsetAnyPFMuonToken;
  edm::EDGetToken fIsSubsetFilteredCosmic1LegToken;
  edm::EDGetToken fPfIsoValToken;
  edm::EDGetToken fOppositeMuonToken;
  edm::EDGetToken fTimeDiffDTCSCToken;
  edm::EDGetToken fTimeDiffRPCToken;

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
  std::vector<bool>                  fIsSubsetAnyPFMuon;
  std::vector<bool>                  fIsSubsetFilteredCosmic1Leg;
  std::vector<float>                 fNormChi2;
  std::vector<bool>                  fHasOppositeMuon;
  std::vector<float>                 fTimeDiffDTCSC;
  std::vector<float>                 fTimeDiffRPC;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleDsaMuon, "ffNtupleDsaMuon" );

ffNtupleDsaMuon::ffNtupleDsaMuon( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleDsaMuon::initialize( TTree&                   tree,
                             const edm::ParameterSet& ps,
                             edm::ConsumesCollector&& cc ) {
  fDSAMuonToken                    = cc.consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPrimaryVertexToken              = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );
  fSegOverlapRatioToken            = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "maxSegmentOverlapRatio" ) );
  fExtrapolatedDrToken             = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minExtrapolateInnermostLocalDr" ) );
  fIsSubsetAnyPFMuonToken          = cc.consumes<edm::ValueMap<bool>>( edm::InputTag( "dsamuonExtra", "isDetIdSubsetOfAnyPFMuon" ) );
  fIsSubsetFilteredCosmic1LegToken = cc.consumes<edm::ValueMap<bool>>( edm::InputTag( "dsamuonExtra", "isSegemntSubsetOfFilteredCosmic1Leg" ) );
  fPfIsoValToken                   = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "pfiso04" ) );
  fOppositeMuonToken               = cc.consumes<edm::ValueMap<reco::MuonRef>>( edm::InputTag( "dsamuonExtra", "oppositeMuon" ) );
  fTimeDiffDTCSCToken              = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "dTUpperMinusLowerDTCSC" ) );
  fTimeDiffRPCToken                = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "dTUpperMinusLowerRPC" ) );

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
  tree.Branch( "dsamuon_isSubsetAnyPFMuon", &fIsSubsetAnyPFMuon )->SetTitle("Associated Muon chamber DetId are subset of any PFMuon's");
  tree.Branch( "dsamuon_isSubsetFilteredCosmic1Leg", &fIsSubsetFilteredCosmic1Leg )->SetTitle("Associated DT/CSC segments are subset of filtered cosmicMuon1Leg's");
  tree.Branch( "dsamuon_normChi2", &fNormChi2 );
  tree.Branch( "dsamuon_hasOppositeMuon", &fHasOppositeMuon );
  tree.Branch( "dsamuon_timeDiffDTCSC", &fTimeDiffDTCSC );
  tree.Branch( "dsamuon_timeDiffRPC", &fTimeDiffRPC );
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

  Handle<ValueMap<float>> segOverlapRatioHdl;
  e.getByToken( fSegOverlapRatioToken, segOverlapRatioHdl );
  assert( segOverlapRatioHdl.isValid() );

  Handle<ValueMap<float>> extrapolatedDrHdl;
  e.getByToken( fExtrapolatedDrToken, extrapolatedDrHdl );
  assert( extrapolatedDrHdl.isValid() );

  Handle<ValueMap<bool>> isSubsetAnyPFMuonHdl;
  e.getByToken( fIsSubsetAnyPFMuonToken, isSubsetAnyPFMuonHdl );
  assert( isSubsetAnyPFMuonHdl.isValid() );

  Handle<ValueMap<float>> pfIsoValHdl;
  e.getByToken( fPfIsoValToken, pfIsoValHdl );
  assert( pfIsoValHdl.isValid() );

  Handle<ValueMap<reco::MuonRef>> oppositeMuonHdl;
  e.getByToken( fOppositeMuonToken, oppositeMuonHdl );
  assert( oppositeMuonHdl.isValid() );

  Handle<ValueMap<float>> timeDiffDTCSCHdl;
  e.getByToken( fTimeDiffDTCSCToken, timeDiffDTCSCHdl );
  assert( timeDiffDTCSCHdl.isValid() );

  Handle<ValueMap<float>> timeDiffRPCHdl;
  e.getByToken( fTimeDiffRPCToken, timeDiffRPCHdl );
  assert( timeDiffRPCHdl.isValid() );

  Handle<ValueMap<bool>> isSubsetFilteredCosmic1LegHdl;
  e.getByToken( fIsSubsetFilteredCosmic1LegToken, isSubsetFilteredCosmic1LegHdl );
  assert( isSubsetFilteredCosmic1LegHdl.isValid() );

  clear();

  // cout << "~~~~~~~~~~~~  " << e.run() << ":" << e.luminosityBlock() << ":" << e.id().event() << "  ~~~~~~~~~~~~" << endl;

  for ( size_t i( 0 ); i != dsamuonHdl->size(); i++ ) {
    reco::MuonRef dsamuonref( dsamuonHdl, i );
    const auto&   dsamuon    = *dsamuonref;
    const auto&   trackref   = dsamuon.outerTrack();
    const auto&   hitpattern = trackref->hitPattern();

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
    fPFIsoVal.emplace_back( ( *pfIsoValHdl )[ dsamuonref ] );
    fSegOverlapRatio.emplace_back( ( *segOverlapRatioHdl )[ dsamuonref ] );
    fExtrapolatedDr.emplace_back( ( *extrapolatedDrHdl )[ dsamuonref ] );
    fIsSubsetAnyPFMuon.emplace_back( ( *isSubsetAnyPFMuonHdl )[ dsamuonref ] );
    fIsSubsetFilteredCosmic1Leg.emplace_back( ( *isSubsetFilteredCosmic1LegHdl )[ dsamuonref ] );
    fNormChi2.emplace_back( trackref->normalizedChi2() );
    fHasOppositeMuon.emplace_back( ( *oppositeMuonHdl )[ dsamuonref ].isNonnull() );
    fTimeDiffDTCSC.emplace_back( ( *timeDiffDTCSCHdl )[ dsamuonref ] );
    fTimeDiffRPC.emplace_back( ( *timeDiffRPCHdl )[ dsamuonref ] );

    // cout<<"dsamuon] "<<i<<" isSubsetFilteredCosmic1Leg - "<<fIsSubsetFilteredCosmic1Leg.back()<<endl;
  }
}

void
ffNtupleDsaMuon::clear() {
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
}