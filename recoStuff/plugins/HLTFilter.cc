#include "Firefighter/recoStuff/interface/HLTFilter.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h"

ff::HLTFilter::HLTFilter( const edm::ParameterSet& ps )
    : hlt_resultToken_( consumes<edm::TriggerResults>( ps.getParameter<edm::InputTag>( "TriggerResults" ) ) ),
      hlt_pathsNoVer_( ps.getParameter<std::vector<std::string>>( "TriggerPaths" ) ),
      hltProcessName_( ps.getParameter<std::string>( "HltProcName" ) ),
      taggingMode_( ps.getParameter<bool>( "TaggingMode" ) ) {
  produces<bool>();
}

void
ff::HLTFilter::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  bool changed( true );
  if ( hltConfig_.init( r, es, hltProcessName_, changed ) ) {
  } else {
    throw cms::Exception( "config extraction failure with processName -> " +
                          hltProcessName_ );
  }
}

bool
ff::HLTFilter::filter( edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;

  edm::Handle<edm::TriggerResults> hlt_resultH;
  e.getByToken( hlt_resultToken_, hlt_resultH );

  assert( hlt_resultH.isValid() );

  const vector<string>& allTriggerPaths = hltConfig_.triggerNames();

  bool cAccept = false;

  for ( const auto& p : hlt_pathsNoVer_ ) {
    const vector<string> matchedPaths(
        hltConfig_.restoreVersion( allTriggerPaths, p ) );
    if ( matchedPaths.size() == 0 )
      continue;

    const string trigPath         = matchedPaths[ 0 ];
    const auto   triggerPathIndex = hltConfig_.triggerIndex( trigPath );
    if ( triggerPathIndex >= hltConfig_.size() ) {
      throw cms::Exception( "Cannot find trigger path -> " + trigPath );
    }

    if ( !hlt_resultH->wasrun( triggerPathIndex ) or
         hlt_resultH->error( triggerPathIndex ) )
      continue;
    if ( hlt_resultH->accept( triggerPathIndex ) ) {
      cAccept = true;
      break;
    }
  }

  e.put( std::make_unique<bool>( cAccept ) );

  return taggingMode_ || cAccept;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( ff::HLTFilter );
