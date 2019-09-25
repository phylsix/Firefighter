#ifndef recoStuff_LeptonjetSourcePFPhotonProducer_H
#define recoStuff_LeptonjetSourcePFPhotonProducer_H

/**
 * \class LeptonjetSourcePFPhotonProducer
 * \description
 * producer to filter out PFPhoton satisfying loose ID.
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

class LeptonjetSourcePFPhotonProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourcePFPhotonProducer( const edm::ParameterSet& );
  ~LeptonjetSourcePFPhotonProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFCandsToken;
  edm::Handle<reco::PFCandidateFwdPtrVector>            fPFCandsHdl;

  const edm::EDGetTokenT<edm::ValueMap<bool>> fPFPhotonIDToken;
  edm::Handle<edm::ValueMap<bool>> fPFPhotonIDHdl;
};

#endif