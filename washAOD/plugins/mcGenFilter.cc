#include "Firefighter/washAOD/interface/mcGenFilter.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

mcGenFilter::mcGenFilter(const edm::ParameterSet& ps) :
  srcCut_(ps.getParameter<std::string>("srcCut"))
{
  genParticleToken_  = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
}


void
mcGenFilter::fillDescriptions(edm::ConfigurationDescriptions& ds)
{
  edm::ParameterSetDescription desc;
  desc.add<std::string>("srcCut", "");
  ds.add("mcgenfilter", desc);
}

bool
mcGenFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  assert(genParticleHandle_.isValid());

  nGoodDau_ = 0;
  for (reco::GenParticleCollection::const_iterator it = genParticleHandle_->begin(), ed = genParticleHandle_->end();
       it!=ed; ++it)
  {
    if (srcCut_(*it)) { ++nGoodDau_; }
  }

  return (nGoodDau_>=4);
}