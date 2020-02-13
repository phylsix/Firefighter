#ifndef ffEvtFilters_ProxyEventFilter_H
#define ffEvtFilters_ProxyEventFilter_H

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class ProxyEventFilter : public edm::one::EDFilter<> {
 public:
  explicit ProxyEventFilter( const edm::ParameterSet& );
  ~ProxyEventFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::PFJetCollection>         fJetToken;
  edm::Handle<reco::PFJetCollection>              fJetHdl;
  edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fLJSrcToken;
  edm::Handle<reco::PFCandidateFwdPtrVector>      fLJSrcHdl;
  edm::EDGetTokenT<reco::TrackCollection>         fTrackToken;
  edm::Handle<reco::TrackCollection>              fTrackHdl;
};

#endif