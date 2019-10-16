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

class LeptonjetSourceDSAMuonProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetSourceDSAMuonProducer( const edm::ParameterSet& );
  ~LeptonjetSourceDSAMuonProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fDSACandsToken;
  const edm::EDGetTokenT<edm::ValueMap<float>>          fSegOverlapRatioToken;
  const edm::EDGetTokenT<edm::ValueMap<float>>          fExtrapolatedDrToken;
  const edm::EDGetTokenT<edm::ValueMap<bool>>           fIsSubsetAnyPFMuonToken;
  const edm::EDGetTokenT<edm::ValueMap<float>>          fPfIsoValToken;
  const edm::EDGetTokenT<edm::ValueMap<reco::MuonRef>>  fOppositeMuonToken;
  const edm::EDGetTokenT<edm::ValueMap<float>>          fTimeDiffDTCSCToken;
  const edm::EDGetTokenT<edm::ValueMap<float>>          fTimeDiffRPCToken;

  edm::Handle<reco::PFCandidateFwdPtrVector> fDSACandsHdl;
  edm::Handle<edm::ValueMap<float>>          fSegOverlapRatioHdl;
  edm::Handle<edm::ValueMap<float>>          fExtrapolatedDrHdl;
  edm::Handle<edm::ValueMap<bool>>           fIsSubsetAnyPFMuonHdl;
  edm::Handle<edm::ValueMap<float>>          fPfIsoValHdl;
  edm::Handle<edm::ValueMap<reco::MuonRef>>  fOppositeMuonHdl;
  edm::Handle<edm::ValueMap<float>>          fTimeDiffDTCSCHdl;
  edm::Handle<edm::ValueMap<float>>          fTimeDiffRPCHdl;

  const float fMinDTTimeDiff;
  const float fMinRPCTimeDiff;
};

#endif