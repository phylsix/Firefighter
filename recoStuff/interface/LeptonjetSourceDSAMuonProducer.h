#ifndef recoStuff_LeptonjetSourceDSAMuonProducer_H
#define recoStuff_LeptonjetSourceDSAMuonProducer_H

/**
 * \class LeptonjetSourceDSAMuonProducer
 * \description
 * producer to filter out DSAMuon satisfying DSA ID & loose Iso,
 * keep the non-isolated ones as leptonjet source for clustering.
 */

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "Firefighter/objects/interface/DSAExtra.h"

class LeptonjetSourceDSAMuonProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& );
  ~LeptonjetSourceDSAMuonProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fDSACandsToken;
  const edm::EDGetTokenT<edm::ValueMap<DSAExtra>>       fDSAExtraToken;

  edm::Handle<reco::PFCandidateFwdPtrVector> fDSACandsHdl;
  edm::Handle<edm::ValueMap<DSAExtra>>       fDSAExtraHdl;

  const float fMinDTTimeDiff;
  const float fMinRPCTimeDiff;
};

#endif