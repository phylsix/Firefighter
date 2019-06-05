#include "Firefighter/ffEvtFilters/interface/ffTriggerObjectsMatchingFilter.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

ffTriggerObjectsMatchingFilter::ffTriggerObjectsMatchingFilter(
    const edm::ParameterSet& ps )
    : fProcessName( ps.getParameter<std::string>( "processName" ) ),
      fTriggerNames(
          ps.getParameter<std::vector<std::string>>( "triggerNames" ) ),
      fTriggerResultsToken( consumes<edm::TriggerResults>(
          ps.getParameter<edm::InputTag>( "triggerResults" ) ) ),
      fTriggerEventToken( consumes<trigger::TriggerEvent>(
          ps.getParameter<edm::InputTag>( "triggerEvent" ) ) ),
      fTracksToken( consumes<reco::TrackCollection>(
          ps.getParameter<edm::InputTag>( "tracks" ) ) ),
      fHLTPrescaleProvider( ps, consumesCollector(), *this ),
      fMinDr( ps.getParameter<double>( "minDr" ) ),
      fMinCounts( ps.getParameter<unsigned int>( "minCounts" ) ) {}

ffTriggerObjectsMatchingFilter::~ffTriggerObjectsMatchingFilter() = default;

void
ffTriggerObjectsMatchingFilter::beginRun( const edm::Run&        r,
                                          const edm::EventSetup& es ) {
  bool changed( true );
  if ( !fHLTPrescaleProvider.init( r, es, fProcessName, changed ) ) {
    throw cms::Exception( "HLT config extraction failure with prcocessName - " +
                          fProcessName );
  }
}

bool
ffTriggerObjectsMatchingFilter::filter( edm::Event&            e,
                                        const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  e.getByToken( fTriggerResultsToken, fTriggerResultsHandle );
  e.getByToken( fTriggerEventToken, fTriggerEventHandle );
  e.getByToken( fTracksToken, fTracksHandle );
  assert( fTriggerResultsHandle.isValid() );
  assert( fTriggerEventHandle.isValid() );
  assert( fTracksHandle.isValid() );

  const HLTConfigProvider& hltConfig = fHLTPrescaleProvider.hltConfigProvider();
  assert( fTriggerResultsHandle->size() == hltConfig.size() );

  const auto& tracks = *fTracksHandle;

  bool result( false );

  for ( const auto& p : fTriggerNames ) {
    math::XYZTLorentzVectorFCollection triggerObjects =
        triggerObjectsFromPath( p, hltConfig );
    if ( triggerObjects.size() < fMinCounts )
      continue;

    unsigned int nmatched( 0 );
    for ( const auto& to : triggerObjects ) {
      for ( const auto& tk : tracks ) {
        if ( deltaR( to, tk ) > fMinDr )
          continue;
        nmatched += 1;
        break;
      }
    }

    if ( nmatched < fMinCounts )
      continue;

    result = true;
    break;
  }

  return result;
}

void
ffTriggerObjectsMatchingFilter::endRun( const edm::Run&        r,
                                        const edm::EventSetup& es ) {}

math::XYZTLorentzVectorFCollection
ffTriggerObjectsMatchingFilter::triggerObjectsFromPath(
    const std::string&       triggerPath,
    const HLTConfigProvider& hltConfig ) const {
  using namespace std;
  using namespace edm;

  math::XYZTLorentzVectorFCollection triggerObjs{};

  const vector<string> matchedpaths =
      hltConfig.restoreVersion( hltConfig.triggerNames(), triggerPath );
  if ( matchedpaths.empty() ) {
    throw cms::Exception(
        "HLTConfigProvider cannot find matched full trigger path with - " +
        triggerPath );
  }

  const string triggerPathFullName = matchedpaths[ 0 ];
  const auto   triggerPathIndex = hltConfig.triggerIndex( triggerPathFullName );
  if ( triggerPathIndex >= hltConfig.size() ) {
    throw cms::Exception( "HLTConfigProvider cannot find trigger path - " +
                          triggerPathFullName );
  }

  // path was not run or error occured
  if ( !fTriggerResultsHandle->wasrun( triggerPathIndex ) or
       fTriggerResultsHandle->error( triggerPathIndex ) ) {
    return triggerObjs;
  }

  // path did not fire
  if ( !fTriggerResultsHandle->accept( triggerPathIndex ) ) {
    return triggerObjs;
  }

  // picking up the last filter index of the path
  trigger::size_type    lastFilterIndex( fTriggerEventHandle->sizeFilters() );
  const vector<string>& moduleLabels =
      hltConfig.moduleLabels( triggerPathIndex );

  for ( auto rit = moduleLabels.crbegin(); rit != moduleLabels.crend();
        ++rit ) {
    if ( hltConfig.moduleEDMType( *rit ) != "EDFilter" )
      continue;
    if ( !hltConfig.saveTags( *rit ) )
      continue;
    lastFilterIndex =
        fTriggerEventHandle->filterIndex( edm::InputTag( *rit, "", "HLT" ) );
    break;
  }

  // nothing saved
  if ( lastFilterIndex >= fTriggerEventHandle->sizeFilters() ) {
    return triggerObjs;
  }

  set<trigger::size_type> triggerObjectIndices{};
  const trigger::Keys&    keys =
      fTriggerEventHandle->filterKeys( lastFilterIndex );
  const trigger::Vids& types =
      fTriggerEventHandle->filterIds( lastFilterIndex );

  for ( size_t iK( 0 ); iK != keys.size(); ++iK ) {
    if ( types[ iK ] != trigger::TriggerObjectType::TriggerL1Mu and
         types[ iK ] != trigger::TriggerObjectType::TriggerMuon ) {
      continue;
    }
    triggerObjectIndices.emplace( keys[ iK ] );
  }

  for ( const auto& iTOidx : triggerObjectIndices ) {
    const auto& iTO = ( fTriggerEventHandle->getObjects() )[ iTOidx ];
    triggerObjs.emplace_back( iTO.px(), iTO.py(), iTO.pz(), iTO.energy() );
  }

  return triggerObjs;
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

DEFINE_FWK_MODULE( ffTriggerObjectsMatchingFilter );