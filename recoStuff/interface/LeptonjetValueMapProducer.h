#ifndef recoStuff_LeptonjetValueMapProducer_H
#define recoStuff_LeptonjetValueMapProducer_H

/**
 * \class LeptonjetValueMapProducer
 * \description
 * Producer to make leptonjet-associated values.
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
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"

class LeptonjetValueMapProducer : public edm::stream::EDProducer<> {
 public:
  explicit LeptonjetValueMapProducer( const edm::ParameterSet& );
  ~LeptonjetValueMapProducer() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFJetCollection>         fLeptonjetToken;
  const edm::EDGetTokenT<reco::PFJetCollection>         fAKJetToken;
  const edm::EDGetTokenT<reco::JetCorrector>            fJetCorrectorToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFChargedHadronToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFNeutralHadronToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFPhotonToken;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> fPFPileUpToken;

  StringCutObjectSelector<reco::PFJet> fJetIdSelector;

  edm::Handle<reco::PFJetCollection>         fLeptonjetHdl;
  edm::Handle<reco::PFJetCollection>         fAK4JetHdl;
  edm::Handle<reco::JetCorrector>            fJetCorrectorHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector> fPFChargedHadronHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector> fPFNeutralHadronHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector> fPFPhotonHdl;
  edm::Handle<reco::PFCandidateFwdPtrVector> fPFPileUpHdl;
};

#endif