import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.MCGeometryFilter_cfi import mcgeometryfilter
from Firefighter.recoStuff.TrackSelections_cfi import selectedTracks
from Firefighter.recoStuff.TrackToPFCandProd_cfi import PFCandsFromTracks
# from Firefighter.recoStuff.ForkCandAgainstDsaMuon_cfi import forkCandAgainstDsaMuon
from Firefighter.recoStuff.SplitPFCandByMatchingDsaMuonProd_cfi import *
from Firefighter.recoStuff.CandMerger_cfi import candmerger


from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets


mcGeometryFilter = mcgeometryfilter.clone()
selectedDsaMuons = selectedTracks.clone()
dsaMuPFCand = PFCandsFromTracks.clone(
    src = cms.InputTag('selectedDsaMuons')
)
# dsaMuPFCandFork = forkCandAgainstDsaMuon.clone(
#     src     = cms.InputTag("particleFlow"),
#     matched = cms.InputTag("dsaMuPFCand")
# )
dsaMuPFCandFork = splitPFCandByMatchingDsaMuon.clone(
    src     = cms.InputTag("particleFlow"),
    matched = cms.InputTag("dsaMuPFCand")
)
particleFlowIncDSA = candmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("particleFlow"),
        cms.InputTag("dsaMuPFCandFork", "nonMatched")
    )
)
ffLeptonJet = ak4PFJets.clone(
    src = cms.InputTag('particleFlowIncDSA'),
    rParam = cms.double(0.1),
)


ffLeptonJetSeq = cms.Sequence(
    mcGeometryFilter
    + selectedDsaMuons
    + dsaMuPFCand
    + dsaMuPFCandFork
    + particleFlowIncDSA
    + ffLeptonJet
)