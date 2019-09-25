#ifndef recoStuff_LeptonjetSourcePFElectronProducer_H
#define recoStuff_LeptonjetSourcePFElectronProducer_H

/**
 * \class LeptonjetSourcePFElectronProducer
 * \description
 * producer to filter out PFElectron satisfying loose ID,
 * keep the non-isolated ones for clustering
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

class LeptonjetSourcePFElectronProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourcePFElectronProducer( const edm::ParameterSet& );
  ~LeptonjetSourcePFElectronProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFCandsToken;
  edm::Handle<reco::PFCandidateFwdPtrVector>            fPFCandsHdl;

  const edm::EDGetTokenT<edm::ValueMap<bool>> fPFElectronIDToken;
  edm::Handle<edm::ValueMap<bool>> fPFElectronIDHdl;
};

#endif