import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.PFCandidateSelections_cff import *
from Firefighter.recoStuff.DsaMuonSelections_cff import *
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandmerger as _pfcandmerger
from Firefighter.recoStuff.JetConstituentSubtractor_cfi import (
    jetconstituentsubtractor as _jetconstituentsubtractor,
)

from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets

particleFlowIncDSA = _pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("filteredPFCands"),
        cms.InputTag("dsaMuPFCandFork", "nonMatched")
    )
)

ffLeptonJetCHS = ak4PFJets.clone(
    src=cms.InputTag("particleFlowIncDSA"),
    # jetAlgorithm=cms.string('Kt'),
    rParam=cms.double(0.4),
    # useFiltering=cms.bool(True),
    # nFilt=cms.int32(3),
    # rFilt=cms.double(0.3),
    # usePruning=cms.bool(True),
    # useTrimming=cms.bool(True),
    # rFilt=cms.double(0.2),
    # trimPtFracMin=cms.double(0.03),
)

# ffLeptonJetCHSConstituents = cms.EDProducer(
#     "PFJetConstituentSelector",
#     src=cms.InputTag('ffLeptonJetCHS'),
#     cut=cms.string('abs(eta)<2.5'))

# ffLeptonJetCHSConstituentsNoHadron = cms.EDFilter(
#     "PFCandidateFwdPtrCollectionStringFilter",
#     src=cms.InputTag("ffLeptonJetCHSConstituents", "constituents"),
#     cut=cms.string(' && '.join([
#         'particleId!=1',
#         'particleId!=5',
#     ])))

# ffLeptonJet = ak4PFJets.clone(
#     src=cms.InputTag('ffLeptonJetCHSConstituentsNoHadron'),
#     rParam=cms.double(0.4),
#     jetAlgorithm=cms.string('Kt'),
# )

ffLeptonJet = _jetconstituentsubtractor.clone()

ffLeptonJetSeq = cms.Sequence(
    filteringPFCands
    + filteringDsaMuAsPFCand
    + particleFlowIncDSA
    + ffLeptonJetCHS
    + ffLeptonJet
)
