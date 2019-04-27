#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtra.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraFwd.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ffNtupleMuonTiming : public ffNtupleBase {
 public:
  ffNtupleMuonTiming( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) override {}

 private:
  void clear() final;

  edm::EDGetToken fMuonToken;
  edm::EDGetToken fMuonTimeExtraToken;

  std::vector<float> fTimeAtIpInOut;
  std::vector<float> fTimeAtIpInOutErr;
  std::vector<float> fTimeAtIpOutIn;
  std::vector<float> fTimeAtIpOutInErr;
  std::vector<float> fInverseBeta;
  std::vector<float> fInverseBetaErr;
  std::vector<float> fFreeInverseBeta;
  std::vector<float> fFreeInverseBetaErr;
  std::vector<int>   fNdof;
  std::vector<int>   fDirection;
  std::vector<int>   fHemisphere;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleMuonTiming, "ffNtupleMuonTiming" );

ffNtupleMuonTiming::ffNtupleMuonTiming( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleMuonTiming::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fMuonToken = cc.consumes<reco::MuonCollection>(
      ps.getParameter<edm::InputTag>( "src" ) );
  fMuonTimeExtraToken = cc.consumes<reco::MuonTimeExtraMap>( edm::InputTag(
      ps.getParameter<edm::InputTag>( "src" ).label(), "combined" ) );

  tree.Branch( "muon_timeAtIpInOut", &fTimeAtIpInOut );
  tree.Branch( "muon_timeAtIpInOutErr", &fTimeAtIpInOutErr );
  tree.Branch( "muon_timeAtIpOutIn", &fTimeAtIpOutIn );
  tree.Branch( "muon_timeAtIpOutInErr", &fTimeAtIpOutInErr );
  tree.Branch( "muon_inverseBeta", &fInverseBeta );
  tree.Branch( "muon_inverseBetaErr", &fInverseBetaErr );
  tree.Branch( "muon_freeBetaInverse", &fFreeInverseBeta );
  tree.Branch( "muon_freeBetaInverseErr", &fFreeInverseBetaErr );
  tree.Branch( "muon_timeNdof", &fNdof );
  tree.Branch( "muon_estimatedDirection", &fDirection );
  tree.Branch( "muon_hemisphere", &fHemisphere );
}

void
ffNtupleMuonTiming::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::MuonCollection> fMuonHdl;
  e.getByToken( fMuonToken, fMuonHdl );
  assert( fMuonHdl.isValid() );
  const auto& muons = *fMuonHdl;

  Handle<reco::MuonTimeExtraMap> fMuonTimeExtraHdl;
  e.getByToken( fMuonTimeExtraToken, fMuonTimeExtraHdl );
  assert( fMuonTimeExtraHdl.isValid() );
  const auto& muonTimeExtras = *fMuonTimeExtraHdl;

  clear();

  for ( size_t i( 0 ); i != muons.size(); ++i ) {
    reco::MuonRef              muref( fMuonHdl, i );
    const reco::MuonTimeExtra& timecomb = muonTimeExtras[ muref ];

    fTimeAtIpInOut.push_back( timecomb.timeAtIpInOut() );
    fTimeAtIpInOutErr.push_back( timecomb.timeAtIpInOutErr() );
    fTimeAtIpOutIn.push_back( timecomb.timeAtIpOutIn() );
    fTimeAtIpOutInErr.push_back( timecomb.timeAtIpOutInErr() );
    fInverseBeta.push_back( timecomb.inverseBeta() );
    fInverseBetaErr.push_back( timecomb.inverseBetaErr() );
    fFreeInverseBeta.push_back( timecomb.freeInverseBeta() );
    fFreeInverseBetaErr.push_back( timecomb.freeInverseBetaErr() );
    fNdof.push_back( timecomb.nDof() );
    fDirection.push_back( timecomb.direction() );
    if ( muref->outerTrack().isNull() or
         muref->outerTrack()->extra().isNull() or
         muref->outerTrack()->outerY() == 0. )
      fHemisphere.push_back( 0 );
    else if ( muref->outerTrack()->outerY() > 0 )
      fHemisphere.push_back( 1 );
    else
      fHemisphere.push_back( -1 );
  }
}

void
ffNtupleMuonTiming::clear() {
  fTimeAtIpInOut.clear();
  fTimeAtIpInOutErr.clear();
  fTimeAtIpOutIn.clear();
  fTimeAtIpOutInErr.clear();
  fInverseBeta.clear();
  fInverseBetaErr.clear();
  fFreeInverseBeta.clear();
  fFreeInverseBetaErr.clear();
  fNdof.clear();
  fDirection.clear();
  fHemisphere.clear();
}