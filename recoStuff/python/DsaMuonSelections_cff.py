import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.MCGeometryFilter_cfi import (
    mcgeometryfilter as _mcgeometryfilter,
)
from Firefighter.recoStuff.MCKinematicFilter_cfi import (
    mckinematicfilter as _mckinematicfilter,
)
from Firefighter.recoStuff.TrackSelections_cfi import selectedTracks
from Firefighter.recoStuff.TrackToPFCandProd_cfi import PFCandsFromTracks

# from Firefighter.recoStuff.ForkCandAgainstDsaMuon_cfi import forkCandAgainstDsaMuon
from Firefighter.recoStuff.SplitPFCandByMatchingDsaMuonProd_cfi import *

mcGeometryFilter = _mcgeometryfilter.clone()
mcKinematicFilter = _mckinematicfilter.clone()
selectedDsaMuons = selectedTracks.clone(
    cut=cms.string(
        " && ".join(
            [
                "pt>5.",
                "abs(eta)<2.4",
                "hitPattern.muonStationsWithValidHits>1",
                "normalizedChi2<10.",
            ]
        )
    )
)
dsaMuPFCand = PFCandsFromTracks.clone(src=cms.InputTag("selectedDsaMuons"))

# dsaMuPFCandFork = forkCandAgainstDsaMuon.clone(
#     src     = cms.InputTag("selectedPFCands"),
#     matched = cms.InputTag("dsaMuPFCand")
# )
dsaMuPFCandFork = splitPFCandByMatchingDsaMuon.clone(
    src=cms.InputTag("filteredPFCands"), matched=cms.InputTag("dsaMuPFCand")
)

filteringDsaMuAsPFCand = cms.Sequence(
    mcGeometryFilter
    + mcKinematicFilter
    + selectedDsaMuons
    + dsaMuPFCand
    + dsaMuPFCandFork
)
