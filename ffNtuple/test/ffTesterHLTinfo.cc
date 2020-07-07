#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class ffTesterHLTinfo : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
 public:
  explicit ffTesterHLTinfo( const edm::ParameterSet& );
  ~ffTesterHLTinfo() = default;
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void beginJob() override {}
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
  void endJob() override {}
  void endRun( const edm::Run&, const edm::EventSetup& ) override {}

  edm::EDGetTokenT<edm::TriggerResults>   fHLTResultToken;
  edm::EDGetTokenT<trigger::TriggerEvent> fHLTEventToken;
  edm::Handle<edm::TriggerResults>        fHLTResultHdl;
  edm::Handle<trigger::TriggerEvent>      fHLTEventHdl;

  std::vector<std::string> fHLTPaths;
  std::vector<std::string> fTrigObjFilterLabels;
  HLTConfigProvider        fHLTConfig;

  bool fDebug;
};

ffTesterHLTinfo::ffTesterHLTinfo( const edm::ParameterSet& ps )
    : fHLTResultToken( consumes<edm::TriggerResults>( ps.getParameter<edm::InputTag>( "triggerResults" ) ) ),
      fHLTEventToken( consumes<trigger::TriggerEvent>( ps.getParameter<edm::InputTag>( "triggerEvent" ) ) ),
      fHLTPaths( ps.getParameter<std::vector<std::string>>( "triggerPaths" ) ),
      fTrigObjFilterLabels( ps.getParameter<std::vector<std::string>>( "triggerObjFilterLabels" ) ),
      fDebug( ps.getParameter<bool>( "debug" ) ) {}

void
ffTesterHLTinfo::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  bool changed( true );
  if ( !fHLTConfig.init( r, es, "HLT", changed ) ) {
    throw cms::Exception( "config extraction failure with processName: HLT" );
  }
}

void
ffTesterHLTinfo::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fHLTResultToken, fHLTResultHdl );
  assert( fHLTResultHdl.isValid() );
  e.getByToken( fHLTEventToken, fHLTEventHdl );
  assert( fHLTEventHdl.isValid() );

  const vector<string>& allTriggerPaths = fHLTConfig.triggerNames();
  for ( const auto& p : fHLTPaths ) {
    const vector<string> versionedPath( fHLTConfig.restoreVersion( allTriggerPaths, p ) );
    if ( versionedPath.empty() ) {
      if ( fDebug ) cout << ">> Failed to find versioned path for: " << p << endl;
      continue;
    }
    const string trigPath    = versionedPath[ 0 ];
    const auto   trigPathIdx = fHLTConfig.triggerIndex( trigPath );
    if ( trigPathIdx >= fHLTConfig.size() )
      throw cms::Exception( ">> Failed to find trigger path: " + trigPath );

    if ( !fHLTResultHdl->wasrun( trigPathIdx ) ) {
      if ( fDebug ) cout << ">> Trigger Path " << trigPath << " was not run." << endl;
      continue;
    }

    if ( fHLTResultHdl->error( trigPathIdx ) ) {
      if ( fDebug ) cout << ">> Trigger Path " << trigPath << " run into error." << endl;
      continue;
    }

    bool fired = fHLTResultHdl->accept( trigPathIdx );

    /* pick the last saved filter(index) of the path */
    // trigger::size_type    lastFilterIdx(   fHLTEventHdl->sizeFilters() );
    // trigger::size_type    lastL1FilterIdx( fHLTEventHdl->sizeFilters() );
    string                L23FilterName;
    string                L1FilterName;
    const vector<string>& moduleNames = fHLTConfig.saveTagsModules( trigPathIdx );
    size_t                iM          = moduleNames.size();
    while ( iM > 0 ) {
      const string& filterName = moduleNames[ --iM ];
      if ( fHLTConfig.moduleEDMType( filterName ) != "EDFilter" ) continue;

      // string filterLabel(fHLTEventHdl->filterLabel(fHLTEventHdl->filterIndex(edm::InputTag( filterName, "", "HLT" ))));
      // cout<<"  - "<<filterName;
      // if ( fHLTConfig.saveTags( filterName ) ) cout<<"  X";
      // cout<<endl;

      // const string identifier = filterName.substr( 0, 5 );
      if ( L23FilterName.size() == 0 ) {
        if ( filterName.substr( 0, 5 ) == "hltL3"            // L3 muon
             or filterName.substr( 0, 5 ) == "hltL2"         // L2 muon
             or filterName.substr( 0, 6 ) == "hltEle"        // electron
             or filterName.find( "Photon" ) != string::npos  // photon
        )
          L23FilterName = filterName;
      }
      if ( L1FilterName.size() == 0 ) {
        if ( filterName.substr( 0, 6 ) == "hltL1s"      // L1 muon seed
             or filterName.substr( 0, 7 ) == "hltEGL1"  // L1 EGM seed
        )
          L1FilterName = filterName;
      }

      if ( L23FilterName.size() > 0 and L1FilterName.size() > 0 ) break;
    }
    cout << "[" << fired << "] " << trigPath << endl;
    cout << "  - " << L23FilterName << endl;
    cout << "  - " << L1FilterName << endl;

    const vector<string>& L1SeedModules = fHLTConfig.hltL1TSeeds( trigPathIdx );
    for ( const auto& L1ModName : L1SeedModules ) {
      cout << "  + " << L1ModName << endl;
    }

    // if ( lastFilterIdx >= fHLTEventHdl->sizeFilters() ) continue;
    // string filterLabel( fHLTEventHdl->filterLabel(lastFilterIdx) );
    // string L1FilterLabel( fHLTEventHdl->filterLabel(lastL1FilterIdx) );

    // if (fDebug) cout<<"["<<fired<<"] "<<left<<setw(55)<<trigPath << setw(55) << filterLabel << setw(55) << L1FilterLabel <<endl;
  }

  map<trigger::size_type, long> selectedTrigObjs{};
  for ( size_t i( 0 ); i != fTrigObjFilterLabels.size(); i++ ) {
    trigger::size_type filterIdx = fHLTEventHdl->filterIndex( edm::InputTag( fTrigObjFilterLabels[ i ], "", fHLTConfig.processName() ) );
    if ( filterIdx >= fHLTEventHdl->sizeFilters() ) {
      cout << ">> Failed to find filter index for filter label: " << fTrigObjFilterLabels[ i ] << endl;
      continue;
    }

    const trigger::Keys& keys  = fHLTEventHdl->filterKeys( filterIdx );
    const trigger::Vids& types = fHLTEventHdl->filterIds( filterIdx );

    for ( size_t iK( 0 ); iK != keys.size(); iK++ ) {
      // types[iK] = trigger::TriggerObjectType::TriggerL1Mu
      if ( selectedTrigObjs.find( keys[ iK ] ) == selectedTrigObjs.end() ) {
        selectedTrigObjs[ keys[ iK ] ] = 1 << i;
      } else {
        selectedTrigObjs[ keys[ iK ] ] |= 1 << i;
      }
    }
    cout << "[" << keys.size() << "] " << fTrigObjFilterLabels[ i ] << endl;
  }
  for (const auto& idbit : selectedTrigObjs) {
      cout<<"+ "<<idbit.first<<" "<<idbit.second<<endl;
  }

  if ( fDebug ) cout << "   ~~~~~~~~~~~~   " << endl;
}

void
ffTesterHLTinfo::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ffTesterHLTinfo );
