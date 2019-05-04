#ifndef recoStuff_ffCandMatcher_h
#define recoStuff_ffCandMatcher_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include <map>

namespace ff {

using MapLink = std::map<reco::PFCandidatePtr, reco::PFCandidatePtr>;
using PFCandPtrCollection = std::vector<reco::PFCandidatePtr>;

class ffCandMatcher {
 public:
  ffCandMatcher()          = default;
  virtual ~ffCandMatcher() = default;

  virtual void init( const edm::Event& e, const edm::EventSetup& es ) = 0;
  virtual void init( const edm::Run& r, const edm::EventSetup& es )   = 0;

  // match --> src
  virtual MapLink match( const PFCandPtrCollection& srcCol,
                         const PFCandPtrCollection& matchCol ) const = 0;

  enum InitBy { run, event, none };
  inline void   setInitBy( InitBy ib ) { fInitby = ib; }
  inline InitBy getInitBy() const { return fInitby; }

 private:
  InitBy fInitby;
};

}  // namespace ff

#endif
