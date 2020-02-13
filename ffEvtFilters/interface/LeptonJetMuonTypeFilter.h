#ifndef ffEvtFilters_LeptonJetMuonTypeFilter_H
#define ffEvtFilters_LeptonJetMuonTypeFilter_H

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class LeptonJetMuonTypeFilter : public edm::one::EDFilter<> {
 public:
  explicit LeptonJetMuonTypeFilter( const edm::ParameterSet& );
  ~LeptonJetMuonTypeFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::PFJetCollection> fJetToken;
  edm::Handle<reco::PFJetCollection>      fJetHdl;
  edm::EDGetTokenT<reco::TrackCollection> fTrackToken;
  edm::Handle<reco::TrackCollection>      fTrackHdl;

  unsigned int fMinCount;
};

#endif