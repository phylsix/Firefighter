import FWCore.ParameterSet.Config as cms


selectedPFCandidates = cms.EDFilter(
    "PFCandidateSelector",
    src = cms.InputTag("particleFlow"),
    cut = cms.string(
        "charge!=0 \
        && pt>5. \
        && abs(eta)<2.5"
    )
)