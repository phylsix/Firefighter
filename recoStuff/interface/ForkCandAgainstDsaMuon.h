#ifndef recoStuff_ForkCandAgainstDsaMuon_H
#define recoStuff_ForkCandAgainstDsaMuon_H

/**
 * This is using the old (CMSSW) **MatcherUsingTracksAlgorithm**
 * For what I implement, refer to __SplitPFCandByMatchingDsaMuonProd__,
 * with **MatcherByExtrapolatingTracks**
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "Firefighter/recoStuff/interface/MatcherUsingTracksAlgorithm.h"

class ForkCandAgainstDsaMuon : public edm::stream::EDProducer<> {
public:
    explicit ForkCandAgainstDsaMuon(const edm::ParameterSet&);
    ~ForkCandAgainstDsaMuon() override;


private:
    void produce(edm::Event&, edm::EventSetup const&) override;
    const edm::EDGetTokenT<edm::View<reco::Candidate>> srcToken_;
    const edm::EDGetTokenT<edm::View<reco::Candidate>> matchedToken_;

    StringCutObjectSelector<reco::Candidate,true> srcCut_;
    ff::MatcherUsingTracksAlgorithm algo_;

    edm::Handle<edm::View<reco::Candidate>> srcHdl_;
    edm::Handle<edm::View<reco::Candidate>> matchedHdl_;

};

#endif