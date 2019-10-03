import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.pfNoPileUpJME_cff import *

# produces source FwdPtrs
# pfcandsLeptonjetsSubtracted = cms.EDProducer(
#     "TPPFJetsOnPFCandidates",
#     enable=cms.bool(True),
#     topCollection=cms.InputTag("ffLeptonJetFwdPtrs"),
#     bottomCollection=cms.InputTag("pfNoPileUpJME"),
# )

pfcandsLeptonjetsSubtracted = cms.EDProducer(
    "TPPFCandidatesOnPFCandidates",
    enable=cms.bool(True),
    topCollection=cms.InputTag("leptonjetSources"),
    bottomCollection=cms.InputTag("pfNoPileUpJME"),
)


# cluster as AK4PFCHS
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJetsCHS as _ak4chs
ak4chsNoLeptonjets = _ak4chs.clone(src=cms.InputTag("pfcandsLeptonjetsSubtracted"))

ak4chsPostLeptonjetsSeq = cms.Sequence(
    pfNoPileUpJMESequence
    + pfcandsLeptonjetsSubtracted
    + ak4chsNoLeptonjets
)