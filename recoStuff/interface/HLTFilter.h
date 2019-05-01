#ifndef recoStuff_HLTFilter_H
#define recoStuff_HLTFilter_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class HLTFilter : public edm::one::EDFilter<edm::one::WatchRuns> {
 public:
  explicit HLTFilter( const edm::ParameterSet& );
  ~HLTFilter() {}

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  bool filter( edm::Event&, const edm::EventSetup& ) override;
  void endRun( const edm::Run&, const edm::EventSetup& ) override {}

  edm::EDGetToken hlt_resultToken_;

  std::vector<std::string> hlt_pathsNoVer_;
  HLTConfigProvider        hltConfig_;
  std::string              hltProcessName_;
};

#endif