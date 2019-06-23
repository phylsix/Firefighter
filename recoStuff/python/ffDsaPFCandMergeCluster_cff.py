import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.PFCandidateSelections_cff import *
from Firefighter.recoStuff.DsaMuonSelections_cff import *
from Firefighter.recoStuff.SplitPFCandsFromDsaMuon_cfi import *
from Firefighter.recoStuff.ffLeptonJetSubjet_cff import *
from Firefighter.recoStuff.ffLeptonJetFiltering_cff import *
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandfwdptrmerger as _pfcandmerger
from Firefighter.recoStuff.JetConstituentSubtractor_cfi import (
    jetconstituentsubtractor as _jetconstituentsubtractor,
)
from Firefighter.ffConfig.ffConfigSwitch import switches


from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets

###############################################################################

mcSignalParamProd = cms.EDProducer("MCSignalParamProducer")

###############################################################################

dsaMuPFCandFork = splitPFCandsFromDsaMuon.clone(
    src=cms.InputTag("filteredPFCandsFwdPtr"),
    matched=cms.InputTag("filteredPFCanddSAPtrs"),
)

particleFlowIncDSA = _pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("filteredPFCandsFwdPtr"),
        cms.InputTag("dsaMuPFCandFork", "nonMatched"),
    )
)

ffPFCandsMatchAndMergeSeq = cms.Sequence(dsaMuPFCandFork + particleFlowIncDSA)

###############################################################################

ffLeptonJetCHS = ak4PFJets.clone(
    src=cms.InputTag("particleFlowIncDSA"),
    rParam=cms.double(0.4),
    jetAlgorithm=cms.string("AntiKt"),
    # radiusPU=cms.double(0.4),
    # useFiltering=cms.bool(True),
    # nFilt=cms.int32(3),
    # rFilt=cms.double(0.3),
    # usePruning=cms.bool(True),
    # useTrimming=cms.bool(True),
    # rFilt=cms.double(0.2),
    # trimPtFracMin=cms.double(0.03),
)


ffLeptonJet = _jetconstituentsubtractor.clone()

ffLeptonJetProd = cms.Sequence(ffLeptonJetCHS + ffLeptonJet)
ffLeptonJetFilter = cms.Sequence(
    ffLeptonJetFwdPtrs + filteredLeptonJet + ffLeptonJetCountFilter
)

if switches["recoStuff"]["usingCHS"] == False:
    ffLeptonJet = ffLeptonJetCHS.clone()
    ffLeptonJetProd = cms.Sequence(ffLeptonJet)

###############################################################################

ffLeptonJetSeq = cms.Sequence(
    mcSignalParamProd
    + (filteringPFCands + filteringDsaMuAsPFCand)
    * ffPFCandsMatchAndMergeSeq
    * ffLeptonJetProd
    * ffLeptonJetFilter
    * ffLeptonJetSubjetSeq
)
