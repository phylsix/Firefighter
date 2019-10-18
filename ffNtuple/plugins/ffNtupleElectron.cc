#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleElectron : public ffNtupleBaseNoHLT {
 public:
  ffNtupleElectron( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken                fElectronToken;
  edm::EDGetToken                fElectronIdCutflowToken;
  std::vector<edm::EDGetToken>   fIdResultTokens;
  const std::string              fIdVersion;
  const std::string              fIdLabel;
  const std::vector<std::string> fCutFlowNames;

  const std::vector<std::string> kIdLabels{"veto", "loose", "medium", "tight"};

  math::XYZTLorentzVectorFCollection        fElectronP4;
  std::vector<int>                          fElectronCharge;
  std::map<std::string, std::vector<float>> fCutFlowValMap;
  std::vector<unsigned int>                 fIdBit;
  std::vector<unsigned int>                 fIdResults;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleElectron, "ffNtupleElectron" );

ffNtupleElectron::ffNtupleElectron( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ),
      fIdVersion( ps.getParameter<std::string>( "idVersion" ) ),
      fIdLabel( ps.getParameter<std::string>( "idLabel" ) ),
      fCutFlowNames( ps.getParameter<std::vector<std::string>>( "cutNames" ) ) {
  assert( std::find( kIdLabels.begin(), kIdLabels.end(), fIdLabel ) != kIdLabels.end() );
}
void
ffNtupleElectron::initialize( TTree&                   tree,
                              const edm::ParameterSet& ps,
                              edm::ConsumesCollector&& cc ) {
  fElectronToken          = cc.consumes<reco::GsfElectronCollection>( ps.getParameter<edm::InputTag>( "src" ) );
  fElectronIdCutflowToken = cc.consumes<edm::ValueMap<vid::CutFlowResult>>( edm::InputTag( "egmGsfElectronIDs", fIdVersion + "-" + fIdLabel ) );
  for ( const auto& idLabel : kIdLabels )
    fIdResultTokens.push_back( cc.consumes<edm::ValueMap<bool>>( edm::InputTag( "egmGsfElectronIDs", fIdVersion + "-" + idLabel ) ) );

  tree.Branch( "electron_p4", &fElectronP4 );
  tree.Branch( "electron_charge", &fElectronCharge );
  for ( const auto& name : fCutFlowNames ) {
    fCutFlowValMap[ name ] = {};
    tree.Branch( ( "electron_" + name ).c_str(), &fCutFlowValMap[ name ] );
  }
  tree.Branch( "electron_idbit", &fIdBit );
  tree.Branch( "electron_idResults", &fIdResults );
}

void
ffNtupleElectron::fill( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<reco::GsfElectronCollection> electronHdl;
  e.getByToken( fElectronToken, electronHdl );
  assert( electronHdl.isValid() );

  Handle<ValueMap<vid::CutFlowResult>> cutflowHdl;
  e.getByToken( fElectronIdCutflowToken, cutflowHdl );
  assert( cutflowHdl.isValid() );

  vector<Handle<ValueMap<bool>>> idResultHdls( kIdLabels.size() );
  for ( size_t i( 0 ); i != kIdLabels.size(); ++i ) {
    e.getByToken( fIdResultTokens[ i ], idResultHdls[ i ] );
    assert( idResultHdls[ i ].isValid() );
  }

  clear();

  for ( size_t iele( 0 ); iele != electronHdl->size(); iele++ ) {
    Ptr<reco::GsfElectron> electronptr( electronHdl, iele );
    const auto&            electron = *electronptr;
    fElectronP4.emplace_back( electron.px(), electron.py(), electron.pz(), electron.energy() );
    fElectronCharge.emplace_back( electron.charge() );

    const auto& cutflow = ( *cutflowHdl )[ electronptr ];
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
      if ( ( *( idResultHdls[ i ] ) )[ electronptr ] )
        idresult |= 1 << i;
    }
    fIdResults.emplace_back( idresult );
  }
}

void
ffNtupleElectron::clear() {
  fElectronP4.clear();
  fElectronCharge.clear();
  for ( const auto& name : fCutFlowNames )
    fCutFlowValMap[ name ].clear();
  fIdBit.clear();
  fIdResults.clear();
}
