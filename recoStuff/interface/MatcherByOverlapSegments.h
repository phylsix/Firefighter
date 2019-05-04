#ifndef recoStuff_MatcherByOverlapSegments_h
#define recoStuff_MatcherByOverlapSegments_h

#include "Firefighter/recoStuff/interface/ffCandMatcher.h"

namespace ff {
class MatcherByOverlapSegments : public ffCandMatcher {
 public:
  explicit MatcherByOverlapSegments( const edm::ParameterSet& ps );
  ~MatcherByOverlapSegments() {}
  void init( const edm::Event& e, const edm::EventSetup& es ) {}
  void init( const edm::Run& r, const edm::EventSetup& es ) {}

  MapLink match( const PFCandPtrCollection& srcCol,
                 const PFCandPtrCollection& matchCol ) const;

 private:
  float fDeltaRMax;
  float fOverlapRatioMin;
  float fDeltaTMax;
};
}  // namespace ff

#endif
