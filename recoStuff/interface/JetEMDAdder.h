#ifndef recoStuff_JetEMDAdder_H
#define recoStuff_JetEMDAdder_H

/**
 * \class JetEMDAdder
 * \description
 * producer to make leptonJet energy/momentum distribution
 * variables valueMap to characterize jet substructure.
 * ref: arXiv:1707.07084 pg.9
 */
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

class JetEMDAdder : public edm::stream::EDProducer<> {
 public:
  explicit JetEMDAdder( const edm::ParameterSet& );
  ~JetEMDAdder() override;

 private:
  void produce( edm::Event&, edm::EventSetup const& ) override;

  const edm::EDGetTokenT<reco::PFJetCollection> fJetToken;
  const edm::EDGetTokenT<reco::JetView>         fSjetToken;
  const edm::EDGetTokenT<edm::Association<reco::PFJetCollection>>
      fJetsjetMapToken;

  edm::Handle<reco::PFJetCollection>                   fJetHdl;
  edm::Handle<reco::JetView>                           fSjetHdl;
  edm::Handle<edm::Association<reco::PFJetCollection>> fJetsjetMapHdl;
};

#endif