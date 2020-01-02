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
  std::vector<edm::EDGetToken>   fIdResultTokens;
  const std::string              fIdVersion;
  const std::string              fIdLabel;
  const std::vector<std::string> fCutFlowNames;

  const std::vector<std::string> kIdLabels{"loose", "medium", "tight"};

  math::XYZTLorentzVectorFCollection        fPhotonP4;
  std::map<std::string, std::vector<float>> fCutFlowValMap;
  std::vector<unsigned int>                 fIdBit;
  std::vector<unsigned int>                 fIdResults;
  std::vector<bool>                         fIsConversion;
  std::vector<bool>                         fHasPixelSeed;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtuplePhoton, "ffNtuplePhoton" );

ffNtuplePhoton::ffNtuplePhoton( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      fIdVersion( ps.getParameter<std::string>( "idVersion" ) ),
      fIdLabel( ps.getParameter<std::string>( "idLabel" ) ),
      fCutFlowNames( ps.getParameter<std::vector<std::string>>( "cutNames" ) ) {
  assert( std::find( kIdLabels.begin(), kIdLabels.end(), fIdLabel ) != kIdLabels.end() );
}

void
ffNtuplePhoton::initialize( TTree&                   tree,
                            const edm::ParameterSet& ps,
                            edm::ConsumesCollector&& cc ) {
  fPhotonToken          = cc.consumes<reco::PhotonCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fPhotonIdCutflowToken = cc.consumes<edm::ValueMap<vid::CutFlowResult>>( edm::InputTag( "egmPhotonIDs", fIdVersion + "-" + fIdLabel ) );
  for ( const auto& idLabel : kIdLabels )
    fIdResultTokens.push_back( cc.consumes<edm::ValueMap<bool>>( edm::InputTag( "egmPhotonIDs", fIdVersion + "-" + idLabel ) ) );

  tree.Branch( "photon_p4", &fPhotonP4 );
  for ( const auto& name : fCutFlowNames ) {
    fCutFlowValMap[ name ] = {};
    tree.Branch( ( "photon_" + name ).c_str(), &fCutFlowValMap[ name ] )->SetTitle( ( "ID variable `" + name + "`'s value" ).c_str() );
  }
  tree.Branch( "photon_idBit", &fIdBit )->SetTitle( "sub-ID result for defined idLabel" );
  tree.Branch( "photon_idResults", &fIdResults )->SetTitle( "ID result, encoded as bitmap" );
  tree.Branch( "photon_isConversion", &fIsConversion )->SetTitle( ">0 references to conversion tracks" );
  tree.Branch( "photon_hasPixelSeed", &fHasPixelSeed )->SetTitle( ">0 references to electron pixel seeds" );
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

  vector<Handle<ValueMap<bool>>> idResultHdls( kIdLabels.size() );
  for ( size_t i( 0 ); i != kIdLabels.size(); ++i ) {
    e.getByToken( fIdResultTokens[ i ], idResultHdls[ i ] );
    assert( idResultHdls[ i ].isValid() );
  }

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

    unsigned int idresult = 0;
    for ( size_t i( 0 ); i != kIdLabels.size(); ++i ) {
      if ( ( *( idResultHdls[ i ] ) )[ photonptr ] )
        idresult |= 1 << i;
    }
    fIdResults.emplace_back( idresult );

    fIsConversion.emplace_back( photon.hasConversionTracks() );
    fHasPixelSeed.emplace_back( photon.hasPixelSeed() );
  }
}

void
ffNtuplePhoton::clear() {
  fPhotonP4.clear();
  for ( const auto& name : fCutFlowNames )
    fCutFlowValMap[ name ].clear();
  fIdBit.clear();
  fIdResults.clear();
  fIsConversion.clear();
  fHasPixelSeed.clear();
}