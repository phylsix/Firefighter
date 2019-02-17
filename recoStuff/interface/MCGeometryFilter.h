#ifndef recoStuff_MCGeometryFilter_H
#define recoStuff_MCGeometryFilter_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Firefighter/recoStuff/interface/GeometryBoundary.h"

class MCGeometryFilter : public edm::one::EDFilter<> {
 public:
  explicit MCGeometryFilter( const edm::ParameterSet& );
  ~MCGeometryFilter() {}

 private:
  bool filter( edm::Event&, const edm::EventSetup& ) override;

  edm::EDGetToken  gen_token_;
  std::vector<int> pdgId_;
  float            boundR_;
  float            boundZ_;
  float            maxEta_;

  ff::GeometryBoundary bound_;
};

#endif