#ifndef recoStuff_ffTriggerObjectsMatchingFilter_H
#define recoStuff_ffTriggerObjectsMatchingFilter_H

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

class ffTriggerObjectsMatchingFilter
    : public edm::one::EDFilter<edm::one::WatchRuns> {
 public:
  explicit ffTriggerObjectsMatchingFilter( const edm::ParameterSet& );
  ~ffTriggerObjectsMatchingFilter() override;

  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  bool filter( edm::Event&, const edm::EventSetup& ) override;
  void endRun( const edm::Run&, const edm::EventSetup& ) override;
  math::XYZTLorentzVectorFCollection triggerObjectsFromPath(
      const std::string&,
      const HLTConfigProvider& ) const;

 private:
  const std::string              fProcessName;
  const std::vector<std::string> fTriggerNames;

  const edm::EDGetTokenT<edm::TriggerResults>   fTriggerResultsToken;
  const edm::EDGetTokenT<trigger::TriggerEvent> fTriggerEventToken;
  const edm::EDGetTokenT<reco::TrackCollection> fTracksToken;

  edm::Handle<edm::TriggerResults>   fTriggerResultsHandle;
  edm::Handle<trigger::TriggerEvent> fTriggerEventHandle;
  edm::Handle<reco::TrackCollection> fTracksHandle;
  HLTPrescaleProvider                fHLTPrescaleProvider;

  double       fMinDr;
  unsigned int fMinCounts;
};

#endif