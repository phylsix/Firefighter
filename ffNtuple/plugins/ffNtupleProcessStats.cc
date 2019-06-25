#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TH1.h"

class ffNtupleProcessStats
    : public edm::one::EDAnalyzer<edm::one::WatchRuns,
                                  edm::one::WatchLuminosityBlocks,
                                  edm::one::SharedResources> {
 public:
  explicit ffNtupleProcessStats( const edm::ParameterSet& );
  ~ffNtupleProcessStats();

 private:
  virtual void beginJob() override;
  virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void beginLuminosityBlock( const edm::LuminosityBlock&,
                                     const edm::EventSetup& ) override;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
  virtual void endLuminosityBlock( const edm::LuminosityBlock&,
                                   const edm::EventSetup& ) override {}
  virtual void endRun( const edm::Run&, const edm::EventSetup& ) override {}
  virtual void endJob() override {}

  edm::Service<TFileService> fs;
  TH1I*                      fHisto;
};

ffNtupleProcessStats::ffNtupleProcessStats( const edm::ParameterSet& ps ) {
  usesResource( "TFileService" );
}

ffNtupleProcessStats::~ffNtupleProcessStats() {}

void
ffNtupleProcessStats::beginJob() {
  fHisto = fs->make<TH1I>(
      "history", "processed statistics;run:lumi:event;counts", 3, 0, 3 );
}

void
ffNtupleProcessStats::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  fHisto->Fill( 0 );
}

void
ffNtupleProcessStats::beginLuminosityBlock( const edm::LuminosityBlock& lb,
                                            const edm::EventSetup&      es ) {
  fHisto->Fill( 1 );
}

void
ffNtupleProcessStats::analyze( const edm::Event&      e,
                               const edm::EventSetup& es ) {
  fHisto->Fill( 2 );
}

DEFINE_FWK_MODULE( ffNtupleProcessStats );
