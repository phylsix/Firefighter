import FWCore.ParameterSet.Config as cms

from LeptonJet.ntupleBuilder.SelectTrack_cfi               import goodDsaMuon
from LeptonJet.ntupleBuilder.TrackPFCandidateProducer_cfi  import trackPFCand
from Firefighter.recoStuff.ForkCandAgainstDsaMuon_cfi      import forkcandagainstdsamuon
from LeptonJet.ntupleBuilder.CandMerger_cfi                import candmerger

def addDSAmergeSeq(proc):

    proc.selectedDsaMuons = goodDsaMuon.clone()
    proc.dSAmuPFCand      = trackPFCand.clone(src=cms.InputTag("selectedDsaMuons"))
    proc.dSAmuPFCandFork  = forkcandagainstdsamuon.clone()
    proc.combPFdSA        = candmerger.clone()

    proc.mergePFdSASeq = cms.Sequence(
        proc.selectedDsaMuons +
        proc.dSAmuPFCand +
        proc.dSAmuPFCandFork +
        proc.combPFdSA
    )

    proc.p0 = cms.Path( proc.mergePFdSASeq )

    return proc