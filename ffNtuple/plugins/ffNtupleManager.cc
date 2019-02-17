#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class ffNtupleManager : public edm::one::EDAnalyzer<edm::one::WatchRuns,
                                                    edm::one::SharedResources> {
 public:
  using ntuple_ptr = std::unique_ptr<ffNtupleBase>;

  explicit ffNtupleManager( const edm::ParameterSet& );
  ~ffNtupleManager() override{};
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
  void endRun( const edm::Run&, const edm::EventSetup& ) override {}

 private:
  edm::Service<TFileService> fs_;
  std::vector<ntuple_ptr>    ff_ntuples_;
  HLTConfigProvider          hltConfig_;
  std::string                hltProcessName_;
  TTree*                     tree_;
};

DEFINE_FWK_MODULE( ffNtupleManager );

ffNtupleManager::ffNtupleManager( const edm::ParameterSet& ps ) {
  usesResource( "TFileService" );

  tree_ = fs_->make<TTree>( "ffNtuple", "ffNtuple" );
  const std::vector<edm::ParameterSet>& ntuple_cfgs =
      ps.getParameterSetVector( "Ntuples" );
  for ( const auto& ntuple_cfg : ntuple_cfgs ) {
    const std::string& ntuple_name =
        ntuple_cfg.getParameter<std::string>( "NtupleName" );
    ff_ntuples_.emplace_back(
        ffNtupleFactory::get()->create( ntuple_name, ntuple_cfg ) );
    ff_ntuples_.back()->initialize( *tree_, ntuple_cfg, consumesCollector() );
    ff_ntuples_.back()->setName( ntuple_name );
  }
  hltProcessName_ = ps.getParameter<std::string>( "HltProcName" );
}

void
ffNtupleManager::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  bool changed( true );
  if ( hltConfig_.init( r, es, hltProcessName_, changed ) ) {
  } else {
    throw cms::Exception( "config extraction failure with processName -> " +
                          hltProcessName_ );
  }
}

void
ffNtupleManager::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  for ( auto& ff_ntuple : ff_ntuples_ ) {
    if ( ff_ntuple->getName().find( "HLT" ) == std::string::npos ) {
      ff_ntuple->fill( e, es );
    } else {
      ff_ntuple->fill( e, es, hltConfig_ );
    }
  }
  tree_->Fill();
}