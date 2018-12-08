#ifndef recoStuff_SplitPFCandByMatchingDsaMuonProd
#define recoStuff_SplitPFCandByMatchingDsaMuonProd

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "Firefighter/recoStuff/interface/MatcherByExtrapolatingTracks.h"
#include "Firefighter/recoStuff/interface/MatcherByAssociatingRecoMuons.h"
#include "Firefighter/recoStuff/interface/RecoHelpers.h"

namespace ff {

  template<typename T>
  std::set<typename T::key_type>
  getMergedMapKeys(const T& A, const T& B);

}


class SplitPFCandByMatchingDsaMuonProd : public edm::stream::EDProducer<>
{
  public:
    explicit SplitPFCandByMatchingDsaMuonProd(const edm::ParameterSet&);
    ~SplitPFCandByMatchingDsaMuonProd() override;


  private:
    void beginRun(const edm::Run&, const edm::EventSetup&) override;
    void produce(edm::Event&, edm::EventSetup const&) override;
    const edm::EDGetTokenT<reco::PFCandidateCollection> srcToken_;
    const edm::EDGetTokenT<reco::PFCandidateCollection> matchedToken_;

    StringCutObjectSelector<reco::PFCandidate,true> srcCut_;
    ff::MatcherByExtrapolatingTracks matcherByTk_;
    ff::MatcherByAssociatingRecoMuons matcherByMu_;

    edm::Handle<reco::PFCandidateCollection> srcHdl_;
    edm::Handle<reco::PFCandidateCollection> matchedHdl_;
};


#endif