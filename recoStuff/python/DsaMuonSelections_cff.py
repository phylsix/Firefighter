import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.MCGeometryFilter_cfi import (
    mcgeometryfilter as _mcgeometryfilter,
)
from Firefighter.recoStuff.MCKinematicFilter_cfi import (
    mckinematicfilter as _mckinematicfilter,
)

from Firefighter.recoStuff.DsaToPFCandidate_cff import *

# from Firefighter.recoStuff.ForkCandAgainstDsaMuon_cfi import forkCandAgainstDsaMuon


mcGeometryFilter = _mcgeometryfilter.clone()
mcKinematicFilter = _mckinematicfilter.clone()

filteredPFCanddSAPtrs = cms.EDFilter(
    "PFCandidateFwdPtrCollectionStringFilter",
    src=cms.InputTag("pfcandsFromMuondSAPtr"),
    cut=cms.string(
        " && ".join(
            [
                "pt>5.",
                "abs(eta)<2.4",
                "trackRef.hitPattern.muonStationsWithValidHits>1",
                "trackRef.normalizedChi2<10.",
            ]
        )
    ),
    makeClones=cms.bool(True),
)


# dsaMuPFCandFork = forkCandAgainstDsaMuon.clone(
#     src     = cms.InputTag("selectedPFCands"),
#     matched = cms.InputTag("dsaMuPFCand")
# )

filteringDsaMuAsPFCand = cms.Sequence(
    mcGeometryFilter + mcKinematicFilter + dSAToPFCandSeq + filteredPFCanddSAPtrs
)
