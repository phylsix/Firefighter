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

  std::vector<std::string> fTimeInstances;

  edm::EDGetToken                        fMuonToken;
  std::map<std::string, edm::EDGetToken> fMuonTimeExtraTokenMap;

  std::vector<int> fHemisphere;

  std::map<std::string, std::vector<float>> fTimeAtIpInOut;
  std::map<std::string, std::vector<float>> fTimeAtIpInOutErr;
  std::map<std::string, std::vector<float>> fTimeAtIpOutIn;
  std::map<std::string, std::vector<float>> fTimeAtIpOutInErr;
  std::map<std::string, std::vector<float>> fInverseBeta;
  std::map<std::string, std::vector<float>> fInverseBetaErr;
  std::map<std::string, std::vector<float>> fFreeInverseBeta;
  std::map<std::string, std::vector<float>> fFreeInverseBetaErr;
  std::map<std::string, std::vector<int>>   fNdof;
  std::map<std::string, std::vector<int>>   fDirection;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleMuonTiming, "ffNtupleMuonTiming" );

ffNtupleMuonTiming::ffNtupleMuonTiming( const edm::ParameterSet& ps )
    : ffNtupleBase( ps ) {}

void
ffNtupleMuonTiming::initialize( TTree&                   tree,
                                const edm::ParameterSet& ps,
                                edm::ConsumesCollector&& cc ) {
  fTimeInstances = ps.getParameter<std::vector<std::string>>( "instances" );
  fMuonToken     = cc.consumes<reco::MuonCollection>(
      ps.getParameter<edm::InputTag>( "src" ) );

  for ( const auto& ins : fTimeInstances ) {
    fMuonTimeExtraTokenMap[ ins ] = cc.consumes<reco::MuonTimeExtraMap>(
        edm::InputTag( ps.getParameter<edm::InputTag>( "src" ).label(), ins ) );

    tree.Branch( ( "muon_timeAtIpInOut_" + ins ).c_str(),
                 &fTimeAtIpInOut[ ins ] );
    tree.Branch( ( "muon_timeAtIpInOutErr_" + ins ).c_str(),
                 &fTimeAtIpInOutErr[ ins ] );
    tree.Branch( ( "muon_timeAtIpOutIn_" + ins ).c_str(),
                 &fTimeAtIpOutIn[ ins ] );
    tree.Branch( ( "muon_timeAtIpOutInErr_" + ins ).c_str(),
                 &fTimeAtIpOutInErr[ ins ] );
    tree.Branch( ( "muon_inverseBeta_" + ins ).c_str(), &fInverseBeta[ ins ] );
    tree.Branch( ( "muon_inverseBetaErr_" + ins ).c_str(),
                 &fInverseBetaErr[ ins ] );
    tree.Branch( ( "muon_freeBetaInverse_" + ins ).c_str(),
                 &fFreeInverseBeta[ ins ] );
    tree.Branch( ( "muon_freeBetaInverseErr_" + ins ).c_str(),
                 &fFreeInverseBetaErr[ ins ] );
    tree.Branch( ( "muon_timeNdof_" + ins ).c_str(), &fNdof[ ins ] );
    tree.Branch( ( "muon_estimatedDirection_" + ins ).c_str(),
                 &fDirection[ ins ] );
  }
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

  map<string, Handle<reco::MuonTimeExtraMap>> fMuonTimeExtraHdlMap;
  for ( const auto& ins : fTimeInstances ) {
    e.getByToken( fMuonTimeExtraTokenMap[ ins ], fMuonTimeExtraHdlMap[ ins ] );
    assert( fMuonTimeExtraHdlMap[ ins ].isValid() );
  }

  clear();

  for ( size_t i( 0 ); i != muons.size(); ++i ) {
    reco::MuonRef muref( fMuonHdl, i );
    if ( muref->outerTrack().isNull() or
         muref->outerTrack()->extra().isNull() or
         muref->outerTrack()->outerY() == 0. )
      fHemisphere.push_back( 0 );
    else if ( muref->outerTrack()->outerY() > 0 )
      fHemisphere.push_back( 1 );
    else
      fHemisphere.push_back( -1 );

    for ( const auto& ins : fTimeInstances ) {
      const reco::MuonTimeExtra& timeInfo =
          ( *fMuonTimeExtraHdlMap[ ins ] )[ muref ];

      fTimeAtIpInOut[ ins ].push_back( timeInfo.timeAtIpInOut() );
      fTimeAtIpInOutErr[ ins ].push_back( timeInfo.timeAtIpInOutErr() );
      fTimeAtIpOutIn[ ins ].push_back( timeInfo.timeAtIpOutIn() );
      fTimeAtIpOutInErr[ ins ].push_back( timeInfo.timeAtIpOutInErr() );
      fInverseBeta[ ins ].push_back( timeInfo.inverseBeta() );
      fInverseBetaErr[ ins ].push_back( timeInfo.inverseBetaErr() );
      fFreeInverseBeta[ ins ].push_back( timeInfo.freeInverseBeta() );
      fFreeInverseBetaErr[ ins ].push_back( timeInfo.freeInverseBetaErr() );
      fNdof[ ins ].push_back( timeInfo.nDof() );
      fDirection[ ins ].push_back( timeInfo.direction() );
    }
  }
}

void
ffNtupleMuonTiming::clear() {
  for ( const auto& ins : fTimeInstances ) {
    fTimeAtIpInOut[ ins ].clear();
    fTimeAtIpInOutErr[ ins ].clear();
    fTimeAtIpOutIn[ ins ].clear();
    fTimeAtIpOutInErr[ ins ].clear();
    fInverseBeta[ ins ].clear();
    fInverseBetaErr[ ins ].clear();
    fFreeInverseBeta[ ins ].clear();
    fFreeInverseBetaErr[ ins ].clear();
    fNdof[ ins ].clear();
    fDirection[ ins ].clear();
  }
  fHemisphere.clear();
}
