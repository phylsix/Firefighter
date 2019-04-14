#include "CommonTools/UtilAlgos/interface/Merger.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"

using PFCandMerger       = Merger<reco::PFCandidateCollection>;
using PFCandFwdPtrMerger = Merger<reco::PFCandidateFwdPtrVector>;

DEFINE_FWK_MODULE( PFCandMerger );
DEFINE_FWK_MODULE( PFCandFwdPtrMerger );
