/**
 * \class L1TriggerResultsAnalyzer
 * \description
 *
 * unpack L1T seed names and results
 */

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

// l1T
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMask.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

class L1TriggerResultsAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
 public:
  explicit L1TriggerResultsAnalyzer( const edm::ParameterSet& );
  ~L1TriggerResultsAnalyzer() = default;
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
  void endRun( const edm::Run&, const edm::EventSetup& ) override {}

  const edm::EDGetTokenT<edm::TriggerResults> token_;

  std::vector<std::string>  names_;
  std::vector<unsigned int> indices_;
};

L1TriggerResultsAnalyzer::L1TriggerResultsAnalyzer( const edm::ParameterSet& ps )
    : token_( consumes<edm::TriggerResults>( ps.getParameter<edm::InputTag>( "src" ) ) ) {}

void
L1TriggerResultsAnalyzer::beginRun( const edm::Run& run, const edm::EventSetup& setup ) {
  names_.clear();
  indices_.clear();

  edm::ESHandle<L1TUtmTriggerMenu> menu;
  setup.get<L1TUtmTriggerMenuRcd>().get( menu );

  auto const& mapping = menu->getAlgorithmMap();

  for ( auto const& keyval : mapping ) {
    names_.push_back( keyval.first );
    indices_.push_back( keyval.second.getIndex() );
  }
}

void
L1TriggerResultsAnalyzer::analyze( const edm::Event& e, const edm::EventSetup& es ) {
  using namespace std;
  using namespace edm;

  Handle<TriggerResults> handleResults;
  e.getByToken( token_, handleResults );
  assert(handleResults.isValid());

  cout<<"size: "<<handleResults->size()<<" accept any? "<<handleResults->accept()<<endl;

  unsigned indices_size = indices_.size();
  for ( size_t nidx = 0; nidx < indices_size; nidx++ ) {
    cout << names_[ nidx ] << "  " << handleResults->accept(nidx) << endl;
  }
}

void
L1TriggerResultsAnalyzer::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( L1TriggerResultsAnalyzer );