#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "TH1.h"
#include "TTree.h"

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
                                   const edm::EventSetup& ) override;
  virtual void endRun( const edm::Run&, const edm::EventSetup& ) override {}
  virtual void endJob() override {}

  edm::EDGetToken fGenProductToken;
  edm::EDGetToken fPileupToken;

  edm::Service<TFileService> fs;

  TH1D*  fHisto;
  TH1D*  fPileup;
  TTree* fTree;
  TTree* fTree2;

  unsigned int fRun;
  unsigned int fLumi;
  float        fWeight;
};

ffNtupleProcessStats::ffNtupleProcessStats( const edm::ParameterSet& ps ) {
  usesResource( "TFileService" );
  TH1::SetDefaultSumw2();

  fGenProductToken = consumes<GenEventInfoProduct>( edm::InputTag( "generator" ) );
  fPileupToken     = consumes<std::vector<PileupSummaryInfo>>( edm::InputTag( "addPileupInfo" ) );
}

ffNtupleProcessStats::~ffNtupleProcessStats() {}

void
ffNtupleProcessStats::beginJob() {
  fHisto  = fs->make<TH1D>( "history", "processed statistics;run:lumi:event:genwgt;counts", 4, 0, 4 );
  fPileup = fs->make<TH1D>( "pileup", "pileup distribution;TrueInteraction;events", 100, 0, 100 );

  fTree = fs->make<TTree>( "runlumi", "" );
  fTree->Branch( "run", &fRun );
  fTree->Branch( "lumi", &fLumi );

  fTree2 = fs->make<TTree>( "weight", "" );
  fTree2->Branch( "weight", &fWeight );
}

void
ffNtupleProcessStats::beginRun( const edm::Run& r, const edm::EventSetup& es ) {
  fHisto->Fill( 0 );
  fRun = r.run();
}

void
ffNtupleProcessStats::beginLuminosityBlock( const edm::LuminosityBlock& lb,
                                            const edm::EventSetup&      es ) {
  fHisto->Fill( 1 );
  fLumi = lb.luminosityBlock();
}

void
ffNtupleProcessStats::analyze( const edm::Event&      e,
                               const edm::EventSetup& es ) {
  fHisto->Fill( 2 );

  edm::Handle<GenEventInfoProduct> genProductHdl;
  e.getByToken( fGenProductToken, genProductHdl );
  fWeight = genProductHdl.isValid() ? genProductHdl->weight() : 0;
  fHisto->Fill( 3, fWeight );  // this is filling sum of genwgt

  edm::Handle<std::vector<PileupSummaryInfo>> pileupSummaryInfoHdl;
  e.getByToken( fPileupToken, pileupSummaryInfoHdl );

  if ( pileupSummaryInfoHdl.isValid() ) {  // MC, produce pileup distribution
    for ( const auto& puinfo : *pileupSummaryInfoHdl ) {
      if ( puinfo.getBunchCrossing() == 0 ) {
        fPileup->Fill( puinfo.getTrueNumInteractions() );
        break;
      }
    }
  }

  fTree2->Fill();
}

void
ffNtupleProcessStats::endLuminosityBlock( const edm::LuminosityBlock& lb,
                                          const edm::EventSetup&      es ) {
  fTree->Fill();
}

DEFINE_FWK_MODULE( ffNtupleProcessStats );
