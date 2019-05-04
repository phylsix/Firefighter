#ifndef recoStuff_SplitPFCandsFromDsaMuon_h
#define recoStuff_SplitPFCandsFromDsaMuon_h

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "Firefighter/recoStuff/interface/ffCandMatcher.h"

class SplitPFCandsFromDsaMuon : public edm::stream::EDProducer<> {
 public:
  explicit SplitPFCandsFromDsaMuon( const edm::ParameterSet& );
  ~SplitPFCandsFromDsaMuon() override;

 private:
  void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  void produce( edm::Event&, edm::EventSetup const& ) override;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> srcToken_;
  const edm::EDGetTokenT<reco::PFCandidateFwdPtrVector> matchedToken_;

  StringCutObjectSelector<reco::PFCandidate, true> srcCut_;
  std::vector<std::shared_ptr<ff::ffCandMatcher>>  fMatchers;

  edm::Handle<reco::PFCandidateFwdPtrVector> srcHdl_;
  edm::Handle<reco::PFCandidateFwdPtrVector> matchedHdl_;
};

#endif
