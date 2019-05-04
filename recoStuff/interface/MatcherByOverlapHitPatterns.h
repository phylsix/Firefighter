#ifndef recoStuff_MatcherByOverlapHitPatterns_h
#define recoStuff_MatcherByOverlapHitPatterns_h

#include "Firefighter/recoStuff/interface/ffCandMatcher.h"

namespace ff {
class MatcherByOverlapHitPatterns : public ffCandMatcher {
 public:
  explicit MatcherByOverlapHitPatterns( const edm::ParameterSet& );
  ~MatcherByOverlapHitPatterns(){};
  void init( const edm::Event& e, const edm::EventSetup& es ) {}
  void init( const edm::Run& r, const edm::EventSetup& es ) {}

  MapLink match( const PFCandPtrCollection& srcCol,
                 const PFCandPtrCollection& matchCol ) const;

 private:
  float fOverlapRatioMin;
};
}  // namespace ff

#endif
