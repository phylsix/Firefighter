#ifndef recoStuff_MatcherByAssociatingRecoMuons_h
#define recoStuff_MatcherByAssociatingRecoMuons_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "Firefighter/recoStuff/interface/RecoHelpers.h"

#include <map>

namespace ff {
  
  template<typename T>
  float
  calcOverlap(const std::vector<T>& src,
              const std::vector<T>& comp);
  
  class MatcherByAssociatingRecoMuons
  {
    public:
      explicit MatcherByAssociatingRecoMuons(const edm::ParameterSet&,
                                             edm::ConsumesCollector&&);
      ~MatcherByAssociatingRecoMuons() { recoMuonTrackLink_.clear(); }

      void init(const edm::Event&);
      void reset();

      using MapLink = std::map<reco::PFCandidateRef, reco::PFCandidateRef>;
      using PFCandRefCollection = std::vector<reco::PFCandidateRef>;
      // match --> src
      MapLink match(const PFCandRefCollection& srcCol,
                    const PFCandRefCollection& matchCol) const;

      bool checkOverlap(const reco::TrackRef& src,
                        const reco::TrackRef& comp,
                        const float& minRatio) const;

      float getPtRel(const reco::TrackRef& src,
                     const reco::TrackRef& comp) const;
    
    private:
      edm::EDGetToken recoMuon_token_;
      float minHitsOverlapRatio_;
      std::map<reco::TrackRef, reco::TrackRef> recoMuonTrackLink_; // innerTrack --> outerTrack
  };

}

#endif