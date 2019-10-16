#ifndef recoStuff_LeptonjetSourcePFMuonProducer_H
#define recoStuff_LeptonjetSourcePFMuonProducer_H

/**
 * \class LeptonjetSourcePFMuonProducer
 * \description
 * producer to filter out PFMuon satisfying loose ID & loose Iso
 * for LeptonjetSourceDSAMuonProducer and keep the non-isolated
 * ones as leptonjet source for clustering.
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

class LeptonjetSourcePFMuonProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourcePFMuonProducer( const edm::ParameterSet& );
  ~LeptonjetSourcePFMuonProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFCandsToken;
  edm::Handle<reco::PFCandidateFwdPtrVector>            fPFCandsHdl;
};

#endif