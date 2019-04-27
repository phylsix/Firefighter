#ifndef recoStuff_CosmicMuonFilter_H
#define recoStuff_CosmicMuonFilter_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

class CosmicMuonFilter : public edm::one::EDFilter<> {
 public:
  explicit CosmicMuonFilter( const edm::ParameterSet& );
  ~CosmicMuonFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::MuonCollection> fMuonToken;
  edm::Handle<reco::MuonCollection>      fMuonHdl;
};

#endif