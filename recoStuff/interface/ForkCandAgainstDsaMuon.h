#ifndef recoStuff_ForkCandAgainstDsaMuon_H
#define recoStuff_ForkCandAgainstDsaMuon_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

class ForkCandAgainstDsaMuon : public edm::stream::EDProducer<> {
public:
    explicit ForkCandAgainstDsaMuon(const edm::ParameterSet&);
    ~ForkCandAgainstDsaMuon() override;

    static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
    void produce(edm::Event&, edm::EventSetup const&) override;
    const edm::EDGetTokenT<reco::PFCandidateCollection> targetColToken_;
    const edm::EDGetTokenT<reco::PFCandidateCollection> toForkColToken_;
    const float maxDR_;
    const float maxPtReso_;

    edm::Handle<reco::PFCandidateCollection> targetColHdl_;
    edm::Handle<reco::PFCandidateCollection> toForkColHdl_;

};

#endif