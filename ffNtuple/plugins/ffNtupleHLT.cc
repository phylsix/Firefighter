#include <map>

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleHLT : public ffNtupleBaseHLT {
 public:
  ffNtupleHLT( const edm::ParameterSet& );

  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&,
             const edm::EventSetup&,
             HLTConfigProvider& ) final;

 private:
  void clear() final;

  edm::EDGetToken hlt_eventToken_;
  edm::EDGetToken hlt_resultToken_;

  std::vector<std::string>                                  hlt_pathsNoVer_;
  std::map<std::string, bool>                               hlt_bit_;

  std::vector<std::string> trigObj_filterLabels_;
  unsigned int trigObj_N_;
  math::XYZTLorentzVectorFCollection trigObj_P4_;
  std::vector<long> trigObj_qualityBit_;
  std::vector<int> trigObj_id_;

  bool fDebug;
  // std::map<std::string, unsigned int>                       hlt_n_;
  // std::map<std::string, math::XYZTLorentzVectorFCollection> hlt_triggerObjectP4_;
  // std::map<std::string, unsigned int>                       l1t_n_;
  // std::map<std::string, math::XYZTLorentzVectorFCollection> l1t_triggerObjectP4_;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleHLT, "ffNtupleHLT" );

ffNtupleHLT::ffNtupleHLT( const edm::ParameterSet& ps )
    : ffNtupleBaseHLT( ps ) {}


void
ffNtupleHLT::initialize( TTree&                   tree,
                         const edm::ParameterSet& ps,
                         edm::ConsumesCollector&& cc ) {
  hlt_eventToken_  = cc.consumes<trigger::TriggerEvent>( ps.getParameter<edm::InputTag>( "TriggerEvent" ) );
  hlt_resultToken_ = cc.consumes<edm::TriggerResults>( ps.getParameter<edm::InputTag>( "TriggerResults" ) );
  hlt_pathsNoVer_  = ps.getParameter<std::vector<std::string>>( "TriggerPaths" );
  trigObj_filterLabels_  = ps.getParameter<std::vector<std::string>>( "FilterLabels" );
  fDebug = ps.getParameter<bool>("debug");

  for ( const auto& p : hlt_pathsNoVer_ ) {
    hlt_bit_[ p ]             = false;
    tree.Branch( p.c_str(), &hlt_bit_[ p ], ( p + "/O" ).c_str() );
  }

  tree.Branch("trigobj_n", &trigObj_N_);
  tree.Branch("trigobj_p4", &trigObj_P4_);
  tree.Branch("trigobj_pid", &trigObj_id_);

  std::stringstream desc;
  desc << "<ul>";
  for (size_t i(0); i!=trigObj_filterLabels_.size(); i++) {
    desc << "<li> bit&amp;(1&lt;&lt;<b>"<<i<<"</b>) "<<trigObj_filterLabels_[i]<<"</li>";
  }
  desc <<"</ul>";

  tree.Branch("trigobj_bit", &trigObj_qualityBit_)->SetTitle(desc.str().c_str());
}

void
ffNtupleHLT::fill( const edm::Event&      e,
                   const edm::EventSetup& es,
                   HLTConfigProvider&     hcp ) {
  using namespace std;

  edm::Handle<trigger::TriggerEvent> hlt_eventH;
  e.getByToken( hlt_eventToken_, hlt_eventH );

  edm::Handle<edm::TriggerResults> hlt_resultH;
  e.getByToken( hlt_resultToken_, hlt_resultH );

  assert( hlt_eventH.isValid() && hlt_resultH.isValid() );
  clear();

  const vector<string>& allTriggerPaths = hcp.triggerNames();

  for ( const auto& p : hlt_pathsNoVer_ ) {
    const vector<string> matchedPaths( hcp.restoreVersion( allTriggerPaths, p ) );
    if ( matchedPaths.size() == 0 )
      continue;

    const std::string trigPath         = matchedPaths[ 0 ];
    const auto        triggerPathIndex = hcp.triggerIndex( trigPath );
    if ( triggerPathIndex >= hcp.size() ) {
      throw cms::Exception( "Cannot find trigger path -> " + trigPath );
    }

    if ( !hlt_resultH->wasrun( triggerPathIndex ) or
         hlt_resultH->error( triggerPathIndex ) )
      continue;

    hlt_bit_[ p ] = hlt_resultH->accept( triggerPathIndex );


    // if ( !hlt_resultH->accept( triggerPathIndex ) )
    //   continue;

    // // picking out last filter(index) of this trigger p.
    // trigger::size_type    lastFilterIndex( hlt_eventH->sizeFilters() );
    // trigger::size_type    lastL1FilterIndex( hlt_eventH->sizeFilters() );
    // const vector<string>& nameModules = hcp.saveTagsModules( triggerPathIndex );
    // size_t                iM          = nameModules.size();
    // while ( iM > 0 ) {
    //   const string& nameFilter = nameModules[ --iM ];
    //   if ( hcp.moduleEDMType( nameFilter ) != "EDFilter" )
    //     continue;
    //   if ( !hcp.saveTags( nameFilter ) )
    //     continue;

    //   const string identifier = nameFilter.substr( 0, 5 );
    //   if ( ( identifier == "hltL2" or identifier == "hltL3" ) and lastFilterIndex == hlt_eventH->sizeFilters() ) {
    //     lastFilterIndex = hlt_eventH->filterIndex( edm::InputTag( nameFilter, "", "HLT" ) );
    //   }
    //   if ( identifier == "hltL1" and lastL1FilterIndex == hlt_eventH->sizeFilters() ) {
    //     lastL1FilterIndex = hlt_eventH->filterIndex( edm::InputTag( nameFilter, "", "HLT" ) );
    //   }

    //   if ( lastFilterIndex != hlt_eventH->sizeFilters() and lastL1FilterIndex != hlt_eventH->sizeFilters() )
    //     break;
    // }

    // if ( lastFilterIndex >= hlt_eventH->sizeFilters() )
    //   continue;

    // filling trigger objects' indexes.
    // set<trigger::size_type> triggerObjectIndices{};

    // const trigger::Keys& keys  = hlt_eventH->filterKeys( lastFilterIndex );
    // const trigger::Vids& types = hlt_eventH->filterIds( lastFilterIndex );

    // for ( size_t iK( 0 ); iK != keys.size(); ++iK ) {
    //   if ( types[ iK ] != trigger::TriggerObjectType::TriggerL1Mu and
    //        types[ iK ] != trigger::TriggerObjectType::TriggerMuon ) {
    //     continue;
    //   }
    //   triggerObjectIndices.emplace( keys[ iK ] );
    // }

    // hlt_n_[ p ] = triggerObjectIndices.size();
    // for ( const auto& iTOidx : triggerObjectIndices ) {
    //   const auto& iTO = ( hlt_eventH->getObjects() )[ iTOidx ];
    //   hlt_triggerObjectP4_[ p ].emplace_back( iTO.px(), iTO.py(), iTO.pz(), iTO.energy() );
    // }

    // if ( lastL1FilterIndex >= hlt_eventH->sizeFilters() )
    //   continue;

    // // filling L1 trigger objects' indexes.
    // set<trigger::size_type> l1TriggerObjectIndices{};

    // const trigger::Keys& l1keys  = hlt_eventH->filterKeys( lastL1FilterIndex );
    // const trigger::Vids& l1types = hlt_eventH->filterIds( lastL1FilterIndex );

    // for ( size_t iK( 0 ); iK != l1keys.size(); ++iK ) {
    //   if ( l1types[ iK ] != trigger::TriggerObjectType::TriggerL1Mu and
    //        l1types[ iK ] != trigger::TriggerObjectType::TriggerMuon ) {
    //     continue;
    //   }
    //   l1TriggerObjectIndices.emplace( l1keys[ iK ] );
    // }

    // l1t_n_[ p ] = l1TriggerObjectIndices.size();
    // for ( const auto& iTOidx : l1TriggerObjectIndices ) {
    //   const auto& iTO = ( hlt_eventH->getObjects() )[ iTOidx ];
    //   l1t_triggerObjectP4_[ p ].emplace_back( iTO.px(), iTO.py(), iTO.pz(), iTO.energy() );
    // }
  }

  // filling trigger objects' indexes.
  map<trigger::size_type, long> selectedTrigObjs{};
  for ( size_t i( 0 ); i != trigObj_filterLabels_.size(); i++ ) {
    trigger::size_type filterIdx = hlt_eventH->filterIndex( edm::InputTag( trigObj_filterLabels_[ i ], "", hcp.processName() ) );
    if ( filterIdx >= hlt_eventH->sizeFilters() ) {
      if (fDebug) cout << ">> Failed to find filter index for filter label: " << trigObj_filterLabels_[ i ] << endl;
      continue;
    }

    const trigger::Keys& keys  = hlt_eventH->filterKeys( filterIdx );
    // const trigger::Vids& types = hlt_eventH->filterIds( filterIdx );

    for ( size_t iK( 0 ); iK != keys.size(); iK++ ) {
      // types[iK] = trigger::TriggerObjectType::TriggerL1Mu
      if ( selectedTrigObjs.find( keys[ iK ] ) == selectedTrigObjs.end() ) {
        selectedTrigObjs[ keys[ iK ] ] = 1 << i;
      } else {
        selectedTrigObjs[ keys[ iK ] ] |= 1 << i;
      }
    }
    if (fDebug) cout << "[" << keys.size() << "] " << trigObj_filterLabels_[ i ] << endl;
  }

  trigObj_N_ = selectedTrigObjs.size();
  for (const auto& keyBit : selectedTrigObjs) {
    const auto& TO = ( hlt_eventH->getObjects() )[ keyBit.first ];
    trigObj_P4_.emplace_back( TO.px(), TO.py(), TO.pz(), TO.energy() );
    trigObj_qualityBit_.emplace_back( keyBit.second );
    trigObj_id_.emplace_back( TO.id() );
  }
}

void
ffNtupleHLT::clear() {
  for ( const auto& p : hlt_pathsNoVer_ ) hlt_bit_[ p ] = false;
  trigObj_N_ = 0;
  trigObj_P4_.clear();
  trigObj_qualityBit_.clear();
  trigObj_id_.clear();
}
