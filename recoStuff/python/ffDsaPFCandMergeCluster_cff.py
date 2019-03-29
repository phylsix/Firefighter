import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.PFCandidateSelections_cff import *
from Firefighter.recoStuff.DsaMuonSelections_cff import *
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandmerger

from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets

particleFlowIncDSA = pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("filteredPFCands"),
        cms.InputTag("dsaMuPFCandFork", "nonMatched")))
ffLeptonJet = ak4PFJets.clone(
    src=cms.InputTag('particleFlowIncDSA'),
    rParam=cms.double(0.4),
)

ffLeptonJetSeq = cms.Sequence(filteringPFCands + filteringDsaMuAsPFCand +
                              particleFlowIncDSA + ffLeptonJet)
