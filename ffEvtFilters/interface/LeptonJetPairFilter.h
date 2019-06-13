#ifndef ffEvtFilters_LeptonJetPairFilter_H
#define ffEvtFilters_LeptonJetPairFilter_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

class LeptonJetPairFilter : public edm::one::EDFilter<> {
 public:
  explicit LeptonJetPairFilter( const edm::ParameterSet& );
  ~LeptonJetPairFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetTokenT<reco::PFJetCollection> fJetToken;
  edm::Handle<reco::PFJetCollection>      fJetHdl;

  double fDPhiMin;
};

#endif