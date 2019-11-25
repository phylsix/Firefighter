#ifndef recoStuff_classes_def_H
#define recoStuff_classes_def_H

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "Firefighter/recoStuff/interface/ffLeptonJetMVAEstimator.h"

namespace Firefighter_recoStuff {
struct dictionary {
  edm::Association<edm::View<reco::Jet>>                           a_jv;
  edm::Wrapper<edm::Association<edm::View<reco::Jet>>>             w_a_jv;
  edm::RefProd<edm::View<reco::Jet>>                               rp_jv;
  edm::ValueMap<std::vector<reco::MuonChamberMatch>>               vm_mcmv;
  edm::Wrapper<edm::ValueMap<std::vector<reco::MuonChamberMatch>>> w_vm_mcmv;
  std::vector<std::vector<reco::MuonChamberMatch>>                 mcmvv;
};
}  // namespace Firefighter_recoStuff

#endif
