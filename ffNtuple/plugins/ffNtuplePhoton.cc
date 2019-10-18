#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

using LorentzVector = math::XYZTLorentzVectorF;

class ffNtuplePhoton : public ffNtupleBaseNoHLT {
 public:
  ffNtuplePhoton( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken                fPhotonToken;
  edm::EDGetToken                fPhotonIdCutflowToken;
  const std::string              fIdName;
  const std::vector<std::string> fCutFlowNames;

  math::XYZTLorentzVectorFCollection        fPhotonP4;
  std::map<std::string, std::vector<float>> fCutFlowValMap;
  std::vector<unsigned int>                 fIdBit;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePhoton, "ffNtuplePhoton" );

ffNtuplePhoton::ffNtuplePhoton( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      fIdName( ps.getParameter<std::string>( "idName" ) ),
      fCutFlowNames( ps.getParameter<std::vector<std::string>>( "cutNames" ) ) {}

void
ffNtuplePhoton::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  fPhotonToken          = cc.consumes<reco::PhotonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPhotonIdCutflowToken = cc.consumes<edm::ValueMap<vid::CutFlowResult>>( edm::InputTag( "egmPhotonIDs", fIdName ) );

  tree.Branch( "photon_p4", &fPhotonP4 );
  for ( const auto& name : fCutFlowNames ) {
    fCutFlowValMap[ name ] = {};
    tree.Branch( ( "photon_" + name ).c_str(), &fCutFlowValMap[ name ] );
  }
  tree.Branch( "photon_idBit", &fIdBit );
}

void
ffNtuplePhoton::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::PhotonCollection> photonHdl;
  e.getByToken( fPhotonToken, photonHdl );
  assert( photonHdl.isValid() );

  Handle<ValueMap<vid::CutFlowResult>> cutflowHdl;
  e.getByToken( fPhotonIdCutflowToken, cutflowHdl );
  assert( cutflowHdl.isValid() );

  clear();
  for ( size_t ipho( 0 ); ipho != photonHdl->size(); ipho++ ) {
    Ptr<reco::Photon> photonptr( photonHdl, ipho );
    const auto&       photon = *photonptr;
    fPhotonP4.emplace_back( photon.px(), photon.py(), photon.pz(), photon.energy() );

    const auto& cutflow = ( *cutflowHdl )[ photonptr ];
    // cout<<cutflow.cutFlowName()<<" ** "<<cutflow.cutFlowPassed()<<endl;
    unsigned int idbit = 0;
    for ( size_t i( 0 ); i != cutflow.cutFlowSize(); i++ ) {
      const string& name = cutflow.getNameAtIndex( i );
      if ( cutflow.getCutResultByIndex( i ) )
        idbit |= 1 << i;
      fCutFlowValMap[ name ].emplace_back( cutflow.getValueCutUpon( i ) );
    }

    fIdBit.emplace_back( idbit );
  }
}

void
ffNtuplePhoton::clear() {
  fPhotonP4.clear();
  for ( const auto& name : fCutFlowNames )
    fCutFlowValMap[ name ].clear();
  fIdBit.clear();
}