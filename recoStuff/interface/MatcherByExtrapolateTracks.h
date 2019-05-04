#ifndef recoStuff_MatcherByExtrapolateTracks_h
#define recoStuff_MatcherByExtrapolateTracks_h

#include "Firefighter/recoStuff/interface/TrackExtrapolator.h"
#include "Firefighter/recoStuff/interface/ffCandMatcher.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include <map>

namespace ff {

class MatcherByExtrapolateTracks : public ffCandMatcher {
 public:
  explicit MatcherByExtrapolateTracks( const edm::ParameterSet& );
  ~MatcherByExtrapolateTracks() {}

  void init( const edm::Event& e, const edm::EventSetup& es ){};
  void init( const edm::Run& r, const edm::EventSetup& es );

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
