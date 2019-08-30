#ifndef ffEvtFilters_ffCosmicEventFilter_H
#define ffEvtFilters_ffCosmicEventFilter_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

class ffCosmicEventFilter : public edm::one::EDFilter<> {
 public:
  explicit ffCosmicEventFilter( const edm::ParameterSet& );
  ~ffCosmicEventFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::TrackCollection> fCosmicToken;
  edm::Handle<reco::TrackCollection>      fCosmicHdl;

  double       fMinCosAlpha;
  unsigned int fMaxPairCount;
  bool         fTaggingMode;
};

#endif