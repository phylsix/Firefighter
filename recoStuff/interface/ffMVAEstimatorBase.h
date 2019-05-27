#ifndef recoStuff_ffMVAEstimatorBase_h
#define recoStuff_ffMVAEstimatorBase_h

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/Candidate.h"

class ffMVAEstimatorBase {
 public:
  ffMVAEstimatorBase( const edm::ParameterSet& ps )
      : fName( ps.getParameter<std::string>( "estimatorName" ) ),
        fTag( ps.getParameter<std::string>( "mvaTag" ) ),
        fnCategories( ps.getParameter<int>( "nCategories" ) ) {}
  virtual ~ffMVAEstimatorBase() {}

  virtual float mvaValue(
      const reco::Candidate*              candidate,
      const std::map<std::string, float>& variableMap ) const   = 0;
  virtual int findCategory( const reco::Candidate* cand ) const = 0;

  const int&         getNCategories() const { return fnCategories; }
  const std::string& getName() const { return fName; }
  const std::string& getTag() const { return fTag; }

 private:
  const std::string fName;
  const std::string fTag;
  const int         fnCategories;
};

#include "FWCore/PluginManager/interface/PluginFactory.h"
using ffMVAEstimatorFactory =
    edmplugin::PluginFactory<ffMVAEstimatorBase*( const edm::ParameterSet& )>;

#endif