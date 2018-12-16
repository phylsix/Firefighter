#ifndef recoStuff_MCKinematicFilter_H
#define recoStuff_MCKinematicFilter_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class MCKinematicFilter : public edm::one::EDFilter<>
{
  public:
    explicit MCKinematicFilter(const edm::ParameterSet&);
    ~MCKinematicFilter() {}
  
  private:
    bool filter(edm::Event&, const edm::EventSetup&) override;

    edm::EDGetToken gen_token_;
    std::vector<int> pdgId_;
    float minPt_;

};

#endif