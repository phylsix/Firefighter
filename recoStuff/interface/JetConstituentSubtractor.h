#ifndef recoStuff_JetConstituentSubtractor_H
#define recoStuff_JetConstituentSubtractor_H

/**
 * Take a jet collection, apply some cut on each jet's constituents,
 * generate a new jet with the updated constituents, then push
 * back the new collection
 */

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

class JetConstituentSubtractor : public edm::stream::EDProducer<> {
 public:
  explicit JetConstituentSubtractor( const edm::ParameterSet& );
  ~JetConstituentSubtractor() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;
  const edm::EDGetTokenT<reco::PFJetCollection> jetToken_;

  StringCutObjectSelector<reco::PFCandidate, true> candCut_;
  edm::Handle<reco::PFJetCollection>               jetH_;
};

#endif