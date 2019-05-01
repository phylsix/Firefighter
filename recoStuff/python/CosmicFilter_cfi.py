import FWCore.ParameterSet.Config as cms

cosmicmuonfilter = cms.EDFilter(
    "CosmicMuonFilter",
    muons=cms.InputTag("muons"),
    cut=cms.string(
        " && ".join(
            [
                "pt>5.",
                "abs(eta)<2.4",
                "outerTrack.hitPattern.muonStationsWithValidHits>1",
                "outerTrack.normalizedChi2<10.",
            ]
        )
    ),
)
