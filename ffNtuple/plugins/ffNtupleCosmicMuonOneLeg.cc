#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"
#include "Firefighter/recoStuff/interface/DSAMuonHelper.h"

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
  edm::EDGetToken fCosmicMatchVMToken;

  math::XYZTLorentzVectorFCollection fCosmicP4;
  std::vector<int>                   fNDTSegments;
  std::vector<int>                   fNCSCSegments;
  std::vector<float>                 fDxy;
  std::vector<float>                 fDz;
  std::vector<float>                 fNormChi2;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleCosmicMuonOneLeg, "ffNtupleCosmicMuonOneLeg" );

ffNtupleCosmicMuonOneLeg::ffNtupleCosmicMuonOneLeg( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleCosmicMuonOneLeg::initialize( TTree&                   tree,
                                      const edm::ParameterSet& ps,
                                      edm::ConsumesCollector&& cc ) {
  fCosmicToken        = cc.consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPvToken            = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );
  fCosmicMatchVMToken = cc.consumes<edm::ValueMap<std::vector<reco::MuonChamberMatch>>>( ps.getParameter<edm::InputTag>( "muonChamberMatch" ) );

  tree.Branch( "cosmiconeleg_p4", &fCosmicP4 );
  tree.Branch( "cosmiconeleg_nDTSegs", &fNDTSegments );
  tree.Branch( "cosmiconeleg_nCSCSegs", &fNCSCSegments );
  tree.Branch( "cosmiconeleg_dxy", &fDxy );
  tree.Branch( "cosmiconeleg_dz", &fDz );
  tree.Branch( "cosmiconeleg_normChi2", &fNormChi2 );
}

void
ffNtupleCosmicMuonOneLeg::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;
  using namespace std;

  Handle<reco::TrackCollection> cosmicHdl;
  e.getByToken( fCosmicToken, cosmicHdl );
  assert( cosmicHdl.isValid() );

  Handle<ValueMap<vector<reco::MuonChamberMatch>>> cosmicMatchHdl;
  e.getByToken( fCosmicMatchVMToken, cosmicMatchHdl );
  assert( cosmicMatchHdl.isValid() );

  Handle<reco::VertexCollection> pvHdl;
  e.getByToken( fPvToken, pvHdl );
  assert( pvHdl.isValid() && pvHdl->size() > 0 );
  const auto& pv = *( pvHdl->begin() );

  clear();

  for ( size_t i( 0 ); i != cosmicHdl->size(); i++ ) {
    reco::TrackRef tkref( cosmicHdl, i );
    const auto&    tk = *tkref;

    fCosmicP4.emplace_back( tk.px(), tk.py(), tk.pz(), tk.p() );
    fNDTSegments.emplace_back( DSAMuonHelper::getDTSegments( ( *cosmicMatchHdl )[ tkref ] ).size() );
    fNCSCSegments.emplace_back( DSAMuonHelper::getCSCSegements( ( *cosmicMatchHdl )[ tkref ] ).size() );
    fDxy.emplace_back( tk.dxy( pv.position() ) );
    fDz.emplace_back( tk.dz( pv.position() ) );
    fNormChi2.emplace_back( tk.normalizedChi2() );
  }
}

void
ffNtupleCosmicMuonOneLeg::clear() {
  fCosmicP4.clear();
  fNDTSegments.clear();
  fNCSCSegments.clear();
  fDxy.clear();
  fDz.clear();
  fNormChi2.clear();
}