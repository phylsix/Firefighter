#ifndef recoStuff_MatcherByExtrapolatingTracks_h
#define recoStuff_MatcherByExtrapolatingTracks_h

#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include <map>

namespace ff {

class MatcherByExtrapolatingTracks {
 public:
  explicit MatcherByExtrapolatingTracks( const edm::ParameterSet& );
  ~MatcherByExtrapolatingTracks() {}

  void init( const edm::EventSetup& );

  using MapLink = std::map<reco::PFCandidatePtr, reco::PFCandidatePtr>;
  using PFCandPtrCollection = std::vector<reco::PFCandidatePtr>;
  // match --> src
  MapLink match( const PFCandPtrCollection& srcCol,
                 const PFCandPtrCollection& matchCol ) const;

 private:
  std::unique_ptr<ff::TrackExtrapolator> tkExtrp_;

  float maxLocalPosDiff_;
  float maxGlobalMomDeltaR_;
  float maxGlobalMomDeltaEta_;
  float maxGlobalMomDeltaPhi_;
  float maxGlobalDPtRel_;
  bool  requireSameCharge_;
  bool  requireOuterInRange_;

  enum SortBy { LocalPosDiff, GlobalMomDeltaR, GlobalDPtRel };
  SortBy sortBy_;
};
}  // namespace ff

#endif