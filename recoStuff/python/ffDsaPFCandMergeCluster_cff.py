import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.MCGeometryFilter_cfi import mcgeometryfilter
from Firefighter.recoStuff.MCKinematicFilter_cfi import mckinematicfilter
from Firefighter.recoStuff.TrackSelections_cfi import selectedTracks
from Firefighter.recoStuff.TrackToPFCandProd_cfi import PFCandsFromTracks
from Firefighter.recoStuff.PFCandidateSelections_cfi import selectedPFCandidates
# from Firefighter.recoStuff.ForkCandAgainstDsaMuon_cfi import forkCandAgainstDsaMuon
from Firefighter.recoStuff.SplitPFCandByMatchingDsaMuonProd_cfi import *
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandmerger


from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets


mcGeometryFilter = mcgeometryfilter.clone()
mcKinematicFilter = mckinematicfilter.clone()
selectedDsaMuons = selectedTracks.clone(
    cut=cms.string(' && '.join([
        'pt>5.',
        'abs(eta)<2.4',
        'hitPattern.muonStationsWithValidHits>2',
        'normalizedChi2<4.'
    ]))
)
dsaMuPFCand = PFCandsFromTracks.clone(
    src=cms.InputTag('selectedDsaMuons')
)
selectedPFCands = selectedPFCandidates.clone()
# dsaMuPFCandFork = forkCandAgainstDsaMuon.clone(
#     src     = cms.InputTag("selectedPFCands"),
#     matched = cms.InputTag("dsaMuPFCand")
# )
dsaMuPFCandFork = splitPFCandByMatchingDsaMuon.clone(
    src=cms.InputTag("selectedPFCands"),
    matched=cms.InputTag("dsaMuPFCand")
)
particleFlowIncDSA = pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("selectedPFCands"),
        cms.InputTag("dsaMuPFCandFork", "nonMatched")
    )
)
ffLeptonJet = ak4PFJets.clone(
    src=cms.InputTag('particleFlowIncDSA'),
    rParam=cms.double(0.4),
)


ffLeptonJetSeq = cms.Sequence(
    mcGeometryFilter
    + mcKinematicFilter
    + selectedDsaMuons
    + dsaMuPFCand
    + selectedPFCands
    + dsaMuPFCandFork
    + particleFlowIncDSA
    + ffLeptonJet
)
