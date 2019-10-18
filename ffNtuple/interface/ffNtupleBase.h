#ifndef Firefighter_ffNtupleBase_H
#define Firefighter_ffNtupleBase_H

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "TTree.h"

class ffNtupleBase {
 public:
  ffNtupleBase( const edm::ParameterSet& ){};
  virtual ~ffNtupleBase(){};
  virtual void initialize( TTree&,
                           const edm::ParameterSet&,
                           edm::ConsumesCollector&& )            = 0;
  virtual void fill( const edm::Event&, const edm::EventSetup& ) = 0;
  virtual void fill( const edm::Event&,
                     const edm::EventSetup&,
                     HLTConfigProvider& )                        = 0;

  inline std::string getName() { return name_; }
  inline void        setName( const std::string& ntuple_name ) { name_ = ntuple_name; }

 protected:
  virtual void clear() = 0;

 private:
  std::string name_;
};

class ffNtupleBaseHLT : public ffNtupleBase {
 public:
  ffNtupleBaseHLT( const edm::ParameterSet& ps )
      : ffNtupleBase( ps ){};
  void fill( const edm::Event&, const edm::EventSetup& ) override {}
};

class ffNtupleBaseNoHLT : public ffNtupleBase {
 public:
  ffNtupleBaseNoHLT( const edm::ParameterSet& ps )
      : ffNtupleBase( ps ){};
  void fill( const edm::Event&, const edm::EventSetup&, HLTConfigProvider& ) override {}
};

#include "FWCore/PluginManager/interface/PluginFactory.h"
using ffNtupleFactory =
    edmplugin::PluginFactory<ffNtupleBase*( const edm::ParameterSet& )>;

#endif