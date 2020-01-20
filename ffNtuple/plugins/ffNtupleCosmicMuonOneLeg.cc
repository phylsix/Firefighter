#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

class ffNtupleCosmicMuonOneLeg : public ffNtupleBaseNoHLT {
 public:
  ffNtupleCosmicMuonOneLeg( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fCosmicToken;
  edm::EDGetToken fPvToken;

  unsigned int                       fNCosmic;
  math::XYZTLorentzVectorFCollection fCosmicP4;

  std::vector<int>   fNhits;
  std::vector<int>   fNDTHits;
  std::vector<int>   fNCSCHits;
  std::vector<int>   fNRPCHits;
  std::vector<int>   fNChambers;
  std::vector<int>   fNDTChambers;
  std::vector<int>   fNCSCChambers;
  std::vector<int>   fNRPCChambers;
  std::vector<int>   fNDTChambersTop;
  std::vector<int>   fNDTChambersBottom;
  std::vector<int>   fNCSCChambersTop;
  std::vector<int>   fNCSCChambersBottom;
  std::vector<int>   fNRPCChambersTop;
  std::vector<int>   fNRPCChambersBottom;
  std::vector<float> fImpactDist2D;
  std::vector<float> fNormChi2;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleCosmicMuonOneLeg, "ffNtupleCosmicMuonOneLeg" );

ffNtupleCosmicMuonOneLeg::ffNtupleCosmicMuonOneLeg( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleCosmicMuonOneLeg::initialize( TTree&                   tree,
                                      const edm::ParameterSet& ps,
                                      edm::ConsumesCollector&& cc ) {
  fCosmicToken = cc.consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPvToken     = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );

  tree.Branch( "cosmiconeleg_n", &fNCosmic );
  tree.Branch( "cosmiconeleg_p4", &fCosmicP4 );
  tree.Branch( "cosmiconeleg_nhits", &fNhits );
  tree.Branch( "cosmiconeleg_nDTHits", &fNDTHits );
  tree.Branch( "cosmiconeleg_nCSCHits", &fNCSCHits );
  tree.Branch( "cosmiconeleg_nRPCHits", &fNRPCHits );
  tree.Branch( "cosmiconeleg_nChambers", &fNChambers );
  tree.Branch( "cosmiconeleg_nDTChambers", &fNDTChambers );
  tree.Branch( "cosmiconeleg_nCSCChambers", &fNCSCChambers );
  tree.Branch( "cosmiconeleg_nRPCChambers", &fNRPCChambers );
  tree.Branch( "cosmiconeleg_nDTChambersTop", &fNDTChambersTop );
  tree.Branch( "cosmiconeleg_nDTChambersBottom", &fNDTChambersBottom );
  tree.Branch( "cosmiconeleg_nCSCChambersTop", &fNCSCChambersTop );
  tree.Branch( "cosmiconeleg_nCSCChambersBottom", &fNCSCChambersBottom );
  tree.Branch( "cosmiconeleg_nRPCChambersTop", &fNRPCChambersTop );
  tree.Branch( "cosmiconeleg_nRPCChambersBottom", &fNRPCChambersBottom );
  tree.Branch( "cosmiconeleg_impactDist2D", &fImpactDist2D );
  tree.Branch( "cosmiconeleg_normChi2", &fNormChi2 );
}

void
ffNtupleCosmicMuonOneLeg::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;
  using namespace std;

  Handle<reco::TrackCollection> cosmicHdl;
  e.getByToken( fCosmicToken, cosmicHdl );
  assert( cosmicHdl.isValid() );

  Handle<reco::VertexCollection> pvHdl;
  e.getByToken( fPvToken, pvHdl );
  assert( pvHdl.isValid() && pvHdl->size() > 0 );
  const auto& pv = *( pvHdl->begin() );

  ESHandle<MagneticField> field_h;
  es.get<IdealMagneticFieldRecord>().get( field_h );
  assert( field_h.isValid() );
  const MagneticField* bField = field_h.product();

  ESHandle<CSCGeometry> cscG;
  ESHandle<DTGeometry>  dtG;
  ESHandle<RPCGeometry> rpcG;
  es.get<MuonGeometryRecord>().get( cscG );
  es.get<MuonGeometryRecord>().get( dtG );
  es.get<MuonGeometryRecord>().get( rpcG );

  clear();

  fNCosmic = cosmicHdl->size();
  for ( size_t i( 0 ); i != cosmicHdl->size(); i++ ) {
    reco::TrackRef tkref( cosmicHdl, i );
    const auto&    tk = *tkref;

    fCosmicP4.emplace_back( tk.px(), tk.py(), tk.pz(), tk.p() );

    int dtHits( 0 ), cscHits( 0 ), rpcHits( 0 ), totalHits( 0 );
    int dtChambers( 0 ), cscChambers( 0 ), rpcChambers( 0 ), totalChambers( 0 );
    int dtT( 0 ), dtB( 0 ), cscT( 0 ), cscB( 0 ), rpcT( 0 ), rpcB( 0 );

    vector<DetId> chamberId{};

    for ( auto ih = tk.recHitsBegin(); ih != tk.recHitsEnd(); ih++ ) {
      const auto& hit = *( *ih );
      if ( !hit.isValid() ) continue;
      if ( hit.geographicalId().det() != DetId::Muon ) continue;
      if ( hit.geographicalId().subdetId() == MuonSubdetId::DT ) {
        dtHits++;
        totalHits++;
        const DTChamber* dtchamber = dtG->chamber( hit.geographicalId() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( dtchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( dtchamber->id().rawId() );
          if ( dtchamber->position().y() > 0 ) dtT++;
          if ( dtchamber->position().y() < 0 ) dtB++;
          dtChambers++;
          totalChambers++;
        }
      } else if ( hit.geographicalId().subdetId() == MuonSubdetId::CSC ) {
        cscHits++;
        totalHits++;
        const CSCChamber* cscchamber = cscG->chamber( hit.geographicalId() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( cscchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( cscchamber->id().rawId() );
          if ( cscchamber->position().y() > 0 ) cscT++;
          if ( cscchamber->position().y() < 0 ) cscB++;
          cscChambers++;
          totalChambers++;
        }
      } else if ( hit.geographicalId().subdetId() == MuonSubdetId::RPC ) {
        rpcHits++;
        totalHits++;
        const RPCChamber* rpcchamber = rpcG->chamber( hit.geographicalId() );
        if ( find( chamberId.begin(), chamberId.end(), DetId( rpcchamber->id().rawId() ) ) == chamberId.end() ) {
          chamberId.emplace_back( rpcchamber->id().rawId() );
          if ( rpcchamber->position().y() > 0 ) rpcT++;
          if ( rpcchamber->position().y() < 0 ) rpcB++;
          rpcChambers++;
          totalChambers++;
        }
      } else {
        totalHits++;
      }
    }

    fNhits.emplace_back( totalHits );
    fNDTHits.emplace_back( dtHits );
    fNCSCHits.emplace_back( cscHits );
    fNRPCHits.emplace_back( rpcHits );
    fNChambers.emplace_back( totalChambers );
    fNDTChambers.emplace_back( dtChambers );
    fNCSCChambers.emplace_back( cscChambers );
    fNRPCChambers.emplace_back( rpcChambers );
    fNDTChambersTop.emplace_back( dtT );
    fNDTChambersBottom.emplace_back( dtB );
    fNCSCChambersTop.emplace_back( cscT );
    fNCSCChambersBottom.emplace_back( cscB );
    fNRPCChambersTop.emplace_back( rpcT );
    fNRPCChambersBottom.emplace_back( rpcB );

    /// transverse impact distance
    GlobalPoint              pvpos( pv.x(), pv.y(), pv.z() );
    TrajectoryStateOnSurface tsos = TransverseImpactPointExtrapolator( bField ).extrapolate(
        trajectoryStateTransform::initialFreeState( tk, bField ), pvpos );
    float impact2d( -999. );
    if ( tsos.isValid() ) impact2d = hypot( tsos.localPosition().x(), tsos.localPosition().y() );
    fImpactDist2D.emplace_back( impact2d );

    fNormChi2.emplace_back( tk.normalizedChi2() );
  }
}

void
ffNtupleCosmicMuonOneLeg::clear() {
  fNCosmic = 0;
  fCosmicP4.clear();
  fNhits.clear();
  fNDTHits.clear();
  fNCSCHits.clear();
  fNRPCHits.clear();
  fNChambers.clear();
  fNDTChambers.clear();
  fNCSCChambers.clear();
  fNRPCChambers.clear();
  fNDTChambersTop.clear();
  fNDTChambersBottom.clear();
  fNCSCChambersTop.clear();
  fNCSCChambersBottom.clear();
  fNRPCChambersTop.clear();
  fNRPCChambersBottom.clear();
  fImpactDist2D.clear();
  fNormChi2.clear();
}