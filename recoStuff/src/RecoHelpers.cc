#include "Firefighter/recoStuff/interface/RecoHelpers.h"

bool
ff::genAccept(reco::GenParticle const& g)
{
  return (abs(g.pdgId())==13 or abs(g.pdgId())==11)  // is muon OR electron
          and g.isHardProcess()                      // final states
          and abs(g.eta())<2.4                       // |eta|<2.4
          and g.pt()>5.                              // pT>5
          and abs(g.vertex().rho())<740              // decay inside CMS
          and abs(g.vz())<960;
}