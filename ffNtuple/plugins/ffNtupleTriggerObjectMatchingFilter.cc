#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleTriggerObjectMatchingFilter : public ffNtupleBaseNoHLT {
 public:
  ffNtupleTriggerObjectMatchingFilter( const edm::ParameterSet& );
  void initialize( TTree&,
                   const edm::ParameterSet&,
                   edm::ConsumesCollector&& ) final;
  void fill( const edm::Event&, const edm::EventSetup& ) final;

 private:
  void clear() final;

  edm::EDGetToken fTriggerObjectMatchingToken;
  bool            fTriggerObjectMatchingResult;
  bool            fDebug;

  std::vector<std::string>               fTriggerNames;
  std::map<std::string, edm::EDGetToken> fTokenMap;
  std::map<std::string, int>             fMatchedCountsMap;
};

DEFINE_EDM_PLUGIN( ffNtupleFactory, ffNtupleTriggerObjectMatchingFilter, "ffNtupleTriggerObjectMatchingFilter" );

ffNtupleTriggerObjectMatchingFilter::ffNtupleTriggerObjectMatchingFilter( const edm::ParameterSet& ps )
    : ffNtupleBaseNoHLT( ps ) {}

void
ffNtupleTriggerObjectMatchingFilter::initialize( TTree&                   tree,
                                                 const edm::ParameterSet& ps,
                                                 edm::ConsumesCollector&& cc ) {
  fTriggerObjectMatchingToken = cc.consumes<bool>( edm::InputTag( "triggerObjectMatchingFilter" ) );
  tree.Branch( "tomatchfilter_result", &fTriggerObjectMatchingResult );

  fDebug = ps.getParameter<bool>( "debug" );
  if ( fDebug ) {
    fTriggerNames = ps.getParameter<std::vector<std::string>>( "triggerNames" );
    for ( const auto& p : fTriggerNames ) {
      std::string _p( p );
      std::replace( _p.begin(), _p.end(), '_', '-' );
      fTokenMap[ p ]         = cc.consumes<int>( edm::InputTag( "triggerObjectMatchingFilter", _p ) );
      fMatchedCountsMap[ p ] = 0;

      _p.erase( std::remove( _p.begin(), _p.end(), '-' ), _p.end() );  // remove all '-' char.
      tree.Branch( ( "tomatchfilter_" + _p ).c_str(), &fMatchedCountsMap[ p ] );
    }
  }
}

void
ffNtupleTriggerObjectMatchingFilter::fill( const edm::Event& e, const edm::EventSetup& es ) {
  clear();

  edm::Handle<bool> triggerObjectMatchingHandle;
  e.getByToken( fTriggerObjectMatchingToken, triggerObjectMatchingHandle );
  assert( triggerObjectMatchingHandle.isValid() );

  fTriggerObjectMatchingResult = *triggerObjectMatchingHandle;

  if ( fDebug ) {
    for ( const auto& p : fTriggerNames ) {
      edm::Handle<int> trigObjMatchCountHandle;
      e.getByToken( fTokenMap[ p ], trigObjMatchCountHandle );
      assert( trigObjMatchCountHandle.isValid() );
      fMatchedCountsMap[ p ] = *trigObjMatchCountHandle;
    }
  }
}

void
ffNtupleTriggerObjectMatchingFilter::clear() {
  fTriggerObjectMatchingResult = false;
  if ( fDebug ) {
    for ( const auto& p : fTriggerNames ) {
      fMatchedCountsMap[ p ] = 0;
    }
  }
}