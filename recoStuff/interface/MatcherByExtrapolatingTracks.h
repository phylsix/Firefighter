#ifndef recoStuff_MatcherByExtrapolatingTracks_h
#define recoStuff_MatcherByExtrapolatingTracks_h

#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include <map>


namespace ff {

  class MatcherByExtrapolatingTracks
  {
    public:
      explicit MatcherByExtrapolatingTracks(const edm::ParameterSet&);
      ~MatcherByExtrapolatingTracks() {}

      void init(const edm::EventSetup&);

      using MapLink = std::map<reco::PFCandidateRef, reco::PFCandidateRef>;
      using PFCandRefCollection = std::vector<reco::PFCandidateRef>;
      // match --> src
      MapLink match(const PFCandRefCollection& srcCol,
                    const PFCandRefCollection& matchCol) const;
    
    private:
      std::unique_ptr<ff::TrackExtrapolator> tkExtrp_;

      float maxLocalPosDiff_     ;
      float maxGlobalMomDeltaR_  ;
      float maxGlobalMomDeltaEta_;
      float maxGlobalMomDeltaPhi_;
      float maxGlobalDPtRel_     ;
      bool requireSameCharge_    ;
      bool requireOuterInRange_  ;
      
      enum  SortBy { LocalPosDiff, GlobalMomDeltaR, GlobalDPtRel };
      SortBy sortBy_;
  };
}

#endif