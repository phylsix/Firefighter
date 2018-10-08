#ifndef Firefighter_washAOD_mcGenFilter_H
#define Firefighter_washAOD_mcGenFilter_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"


class mcGenFilter : public edm::one::EDFilter<>
{
  public:
    explicit mcGenFilter(const edm::ParameterSet&);
    ~mcGenFilter() {};
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    bool filter(edm::Event&, const edm::EventSetup&) override;

    edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    const StringCutObjectSelector<reco::GenParticle,true> srcCut_;
    unsigned int nGoodDau_;
};

#endif