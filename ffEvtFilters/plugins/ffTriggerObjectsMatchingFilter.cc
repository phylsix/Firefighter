#include "Firefighter/ffEvtFilters/interface/ffTriggerObjectsMatchingFilter.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

ffTriggerObjectsMatchingFilter::ffTriggerObjectsMatchingFilter(const edm::ParameterSet& ps )
    : fProcessName( ps.getParameter<std::string>( "processName" ) ),
      fTriggerNames( ps.getParameter<std::vector<std::string>>( "triggerNames" ) ),
      fSrcCut(ps.getParameter<std::string>("srcCut")),
      fTriggerResultsToken( consumes<edm::TriggerResults>( ps.getParameter<edm::InputTag>( "triggerResults" ) ) ),
      fTriggerEventToken( consumes<trigger::TriggerEvent>( ps.getParameter<edm::InputTag>( "triggerEvent" ) ) ),
      fTracksToken( consumes<reco::TrackCollection>( ps.getParameter<edm::InputTag>( "tracks" ) ) ),
      fHLTPrescaleProvider( ps, consumesCollector(), *this ),
      fTrackSelector(fSrcCut, true),
      fMinDr( ps.getParameter<double>( "minDr" ) ),
      fMinCounts( ps.getParameter<int>( "minCounts" ) ),
      fTaggingMode( ps.getParameter<bool>( "taggingMode" ) ) {
  produces<bool>();
  for (const auto& p : fTriggerNames) {
    std::string _p(p);
    std::replace(_p.begin(), _p.end(), '_', '-'); // '_' is not allowed in product instance name
    produces<int>(_p);
  }
}

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

  bool result( false );

  vector<Ptr<reco::Track>> filteredTracks{};
  for (size_t i(0); i!=fTracksHandle->size(); ++i) {
    if (fTrackSelector((*fTracksHandle)[i])) {
      filteredTracks.emplace_back(fTracksHandle, i);
    }
  }

  for (const auto& p : fTriggerNames) {
    fMatchedTriggerObjectsCountMap[p] = 0;
    math::XYZTLorentzVectorFCollection triggerObjects = triggerObjectsFromPath( p, hltConfig );
    for (const auto& to : triggerObjects) {
      for (const auto& tk : filteredTracks) {
        if (deltaR(to, *tk)> fMinDr) continue;
        fMatchedTriggerObjectsCountMap[p] += 1;
        break;
      }
    }
    string _p(p);
    replace(_p.begin(), _p.end(), '_', '-');
    e.put(make_unique<int>(fMatchedTriggerObjectsCountMap[p]), _p);
    if (!result and fMatchedTriggerObjectsCountMap[p]>=fMinCounts) result=true;
  }

  e.put( make_unique<bool>( result ) );

  return fTaggingMode || result;
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
  if ( matchedpaths.empty() )
    return triggerObjs;

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
