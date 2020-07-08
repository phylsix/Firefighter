#ifndef ffEvtFilters_ffCosmicEventFilter_H
#define ffEvtFilters_ffCosmicEventFilter_H

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class ffCosmicEventFilter : public edm::one::EDFilter<> {
 public:
  explicit ffCosmicEventFilter( const edm::ParameterSet& );
  ~ffCosmicEventFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::TrackCollection> fCosmicToken;
  edm::Handle<reco::TrackCollection>      fCosmicHdl;

  StringCutObjectSelector<reco::Track> fTrackSelector;

  double fMinCosAlpha;
  int    fMaxPairCount;
  int    fNumParallelPairs;
  bool   fTaggingMode;
};

#endif