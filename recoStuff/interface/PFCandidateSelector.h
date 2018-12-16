#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

using PFCandidateSelector =
      SingleObjectSelector< reco::PFCandidateCollection, StringCutObjectSelector<reco::PFCandidate, true> >;