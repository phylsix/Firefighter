#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtuplePrimaryVertex : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePrimaryVertex( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fPvToken;

  unsigned int       fNpv;
  std::vector<float> fRho;
  std::vector<float> fZ;
  std::vector<bool>  fIsFake;
  std::vector<float> fNdof;
  std::vector<float> fNormalizedChi2;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePrimaryVertex, "ffNtuplePrimaryVertex" );

ffNtuplePrimaryVertex::ffNtuplePrimaryVertex( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtuplePrimaryVertex::initialize( TTree&                   tree,
                                   const edm::ParameterSet& ps,
                                   edm::ConsumesCollector&& cc ) {
  fPvToken = cc.consumes<reco::VertexCollection>( ps.getParameter<edm::InputTag>( "src" ) );

  tree.Branch( "pv_n", &fNpv );
  tree.Branch( "pv_rho", &fRho );
  tree.Branch( "pv_z", &fZ );
  tree.Branch( "pv_isFake", &fIsFake );
  tree.Branch( "pv_ndof", &fNdof );
  tree.Branch( "pv_normChi2", &fNormalizedChi2 );
}

void
ffNtuplePrimaryVertex::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace edm;

  Handle<reco::VertexCollection> pvs_h;
  e.getByToken( fPvToken, pvs_h );
  assert( pvs_h.isValid() && pvs_h->size() > 0 );

  clear();

  fNpv = pvs_h->size();
  for ( const auto& v : *pvs_h ) {
    fRho.emplace_back( v.position().rho() );
    fZ.emplace_back( v.z() );
    fIsFake.emplace_back( v.isFake() );
    fNdof.emplace_back( v.ndof() );
    fNormalizedChi2.emplace_back( v.normalizedChi2() );
  }
}

void
ffNtuplePrimaryVertex::clear() {
  fNpv = 0;
  fRho.clear();
  fZ.clear();
  fIsFake.clear();
  fNdof.clear();
  fNormalizedChi2.clear();
}
