#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
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

  edm::EDGetToken fSegOverlapRatioToken;
  edm::EDGetToken fExtrapolatedDrToken;
  edm::EDGetToken fIsSubsetAnyPFMuonToken;
  edm::EDGetToken fPfIsoValToken;
  edm::EDGetToken fOppositeMuonToken;
  edm::EDGetToken fTimeDiffDTCSCToken;
  edm::EDGetToken fTimeDiffRPCToken;

  math::XYZTLorentzVectorFCollection fDSAMuonP4;
  std::vector<float>                 fCharge;
  std::vector<int>                   fCSCStations;
  std::vector<int>                   fDTStations;
  std::vector<int>                   fCSCHits;
  std::vector<int>                   fDTHits;
  std::vector<float>                 fPtErrorOverPt;
  std::vector<float>                 fPFIsoVal;
  std::vector<float>                 fSegOverlapRatio;
  std::vector<float>                 fExtrapolatedDr;
  std::vector<bool>                  fIsSubsetAnyPFMuon;
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
  fDSAMuonToken           = cc.consumes<reco::MuonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fSegOverlapRatioToken   = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "maxSegmentOverlapRatio" ) );
  fExtrapolatedDrToken    = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "minExtrapolateInnermostLocalDr" ) );
  fIsSubsetAnyPFMuonToken = cc.consumes<edm::ValueMap<bool>>( edm::InputTag( "dsamuonExtra", "isDetIdSubsetOfAnyPFMuon" ) );
  fPfIsoValToken          = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "pfiso04" ) );
  fOppositeMuonToken      = cc.consumes<edm::ValueMap<reco::MuonRef>>( edm::InputTag( "dsamuonExtra", "oppositeMuon" ) );
  fTimeDiffDTCSCToken     = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "dTUpperMinusLowerDTCSC" ) );
  fTimeDiffRPCToken       = cc.consumes<edm::ValueMap<float>>( edm::InputTag( "dsamuonExtra", "dTUpperMinusLowerRPC" ) );

  tree.Branch( "dsamuon_p4", &fDSAMuonP4 );
  tree.Branch( "dsamuon_charge", &fCharge );
  tree.Branch( "dsamuon_CSCStations", &fCSCStations );
  tree.Branch( "dsamuon_DTStations", &fDTStations );
  tree.Branch( "dsamuon_CSCHits", &fCSCHits );
  tree.Branch( "dsamuon_DTHits", &fDTHits );
  tree.Branch( "dsamuon_ptErrorOverPt", &fPtErrorOverPt );
  tree.Branch( "dsamuon_PFIsoVal", &fPFIsoVal );
  tree.Branch( "dsamuon_segOverlapRatio", &fSegOverlapRatio );
  tree.Branch( "dsamuon_extrapolatedDr", &fExtrapolatedDr );
  tree.Branch( "dsamuon_isSubsetAnyPFMuon", &fIsSubsetAnyPFMuon );
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

  clear();

  for ( size_t i( 0 ); i != dsamuonHdl->size(); i++ ) {
    reco::MuonRef dsamuonref( dsamuonHdl, i );
    const auto&   dsamuon    = *dsamuonref;
    const auto&   trackref   = dsamuon.outerTrack();
    const auto&   hitpattern = trackref->hitPattern();

    fDSAMuonP4.emplace_back( dsamuon.px(), dsamuon.py(), dsamuon.pz(), dsamuon.energy() );
    fCharge.emplace_back( dsamuon.charge() );
    fCSCStations.emplace_back( hitpattern.cscStationsWithValidHits() );
    fDTStations.emplace_back( hitpattern.dtStationsWithValidHits() );
    fCSCHits.emplace_back( hitpattern.numberOfValidMuonCSCHits() );
    fDTHits.emplace_back( hitpattern.numberOfValidMuonDTHits() );
    fPtErrorOverPt.emplace_back( trackref->ptError() / trackref->pt() );
    fPFIsoVal.emplace_back( ( *pfIsoValHdl )[ dsamuonref ] );
    fSegOverlapRatio.emplace_back( ( *segOverlapRatioHdl )[ dsamuonref ] );
    fExtrapolatedDr.emplace_back( ( *extrapolatedDrHdl )[ dsamuonref ] );
    fIsSubsetAnyPFMuon.emplace_back( ( *isSubsetAnyPFMuonHdl )[ dsamuonref ] );
    fNormChi2.emplace_back( trackref->normalizedChi2() );
    fHasOppositeMuon.emplace_back( ( *oppositeMuonHdl )[ dsamuonref ].isNonnull() );
    fTimeDiffDTCSC.emplace_back( ( *timeDiffDTCSCHdl )[ dsamuonref ] );
    fTimeDiffRPC.emplace_back( ( *timeDiffRPCHdl )[ dsamuonref ] );
  }
}

void
ffNtupleDsaMuon::clear() {
  fDSAMuonP4.clear();
  fCharge.clear();
  fCSCStations.clear();
  fDTStations.clear();
  fCSCHits.clear();
  fDTHits.clear();
  fPtErrorOverPt.clear();
  fPFIsoVal.clear();
  fSegOverlapRatio.clear();
  fExtrapolatedDr.clear();
  fIsSubsetAnyPFMuon.clear();
  fNormChi2.clear();
  fHasOppositeMuon.clear();
  fTimeDiffDTCSC.clear();
  fTimeDiffRPC.clear();
}