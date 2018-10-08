#include "CommonTools/UtilAlgos/interface/ObjectCountFilter.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

using genParticleFilter =
  ObjectCountFilter< reco::GenParticleCollection, StringCutObjectSelector<reco::GenParticle, true> >::type;