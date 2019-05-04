import FWCore.ParameterSet.Config as cms

muoncut = cms.string(
    " && ".join(
        [
            "pt>20.",
            "abs(eta)<2.4",
            "outerTrack.isNonnull()",
            "outerTrack.hitPattern.muonStationsWithValidHits>1",
            "outerTrack.normalizedChi2<10.",
        ]
    )
)

###############################################################################

recoMuonSelected = cms.EDFilter("MuonSelector", src=cms.InputTag("muons"), cut=muoncut)
recoMuonFiltered = cms.EDFilter(
    "MuonCountFilter", src=cms.InputTag("recoMuonSelected"), minNumber=cms.uint32(1)
)

dsaMuonSelected = cms.EDFilter(
    "MuonSelector", src=cms.InputTag("muonsFromdSA"), cut=muoncut
)
dsaMuonFiltered = cms.EDFilter(
    "MuonCountFilter", src=cms.InputTag("dsaMuonSelected"), minNumber=cms.uint32(1)
)

###############################################################################

singlemuFilterSeq = cms.Sequence(
    recoMuonSelected + recoMuonFiltered + dsaMuonSelected + dsaMuonFiltered
)
