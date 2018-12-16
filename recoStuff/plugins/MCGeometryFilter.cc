#include "Firefighter/recoStuff/interface/MCGeometryFilter.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include <algorithm>

MCGeometryFilter::MCGeometryFilter(const edm::ParameterSet& ps) :
  gen_token_(consumes<reco::GenParticleCollection>(ps.getParameter<edm::InputTag>("GenParticles"))),
  pdgId_(ps.getParameter<std::vector<int>>("pdgId")),
  boundR_(ps.getParameter<double>("boundR")),
  boundZ_(ps.getParameter<double>("boundZ")),
  maxEta_(ps.getParameter<double>("maxEta"))
{
  bound_ = ff::GeometryBoundary(maxEta_, boundR_, boundZ_); 
}

bool
MCGeometryFilter::filter(edm::Event& e, const edm::EventSetup& es)
{
  using namespace std;
  using namespace edm;

  Handle<reco::GenParticleCollection> gen_h;
  e.getByToken(gen_token_, gen_h);
  assert(gen_h.isValid());

  const reco::GenParticleCollection& genparticles = *gen_h;

  auto daughterParticleInRange =
    [this](const reco::GenParticle& p)
    {
      return find(pdgId_.cbegin(), pdgId_.cend(), abs(p.pdgId())) != pdgId_.cend()
        and p.isHardProcess()
        and bound_.inRegionByRZ(p.vertex().rho(), fabs(p.vz()));
    };

  return count_if(genparticles.begin(), genparticles.end(), daughterParticleInRange) >=4;
}

#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(MCGeometryFilter);