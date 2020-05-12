#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleCosmicMuon : public ffNtupleBaseNoHLT {
 public:
  ffNtupleCosmicMuon( const edm::ParameterSet& );

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

  std::vector<float> fDxy;
  std::vector<float> fDz;
  std::vector<float> fNormChi2;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleCosmicMuon, "ffNtupleCosmicMuon" );

ffNtupleCosmicMuon::ffNtupleCosmicMuon( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleCosmicMuon::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fCosmicToken = cc.consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPvToken     = cc.consumes<reco::VertexCollection>( edm::InputTag( "offlinePrimaryVertices" ) );

  tree.Branch( "cosmicmuon_n", &fNCosmic );
  tree.Branch( "cosmicmuon_p4", &fCosmicP4 );
  tree.Branch( "cosmicmuon_dxy", &fDxy );
  tree.Branch( "cosmicmuon_dz", &fDz );
  tree.Branch( "cosmicmuon_normChi2", &fNormChi2 );
}

void
ffNtupleCosmicMuon::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;
  using namespace std;

  Handle<reco::TrackCollection> cosmicHdl;
  e.getByToken( fCosmicToken, cosmicHdl );
  assert( cosmicHdl.isValid() );

  Handle<reco::VertexCollection> pvHdl;
  e.getByToken( fPvToken, pvHdl );
  assert( pvHdl.isValid() && pvHdl->size() > 0 );
  const auto& pv = *( pvHdl->begin() );

  clear();

  fNCosmic = cosmicHdl->size();
  for ( const auto& cosmic : *cosmicHdl ) {
    fCosmicP4.emplace_back( cosmic.px(), cosmic.py(), cosmic.pz(), cosmic.p() );
    fDxy.emplace_back( -cosmic.dxy( pv.position() ) );
    fDz.emplace_back( -cosmic.dz( pv.position() ) );
    fNormChi2.emplace_back( cosmic.normalizedChi2() );
  }
}

void
ffNtupleCosmicMuon::clear() {
  fNCosmic = 0;
  fCosmicP4.clear();
  fDxy.clear();
  fDz.clear();
  fNormChi2.clear();
}