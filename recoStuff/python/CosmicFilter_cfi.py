import FWCore.ParameterSet.Config as cms

cosmicmuonfilter = cms.EDFilter(
    "CosmicMuonFilter",
    muons=cms.InputTag("muons"),
    cut=cms.string(
        " && ".join(
            [
                "pt>20.",
                "abs(eta)<2.4",
                "outerTrack.isNonnull()",
                "outerTrack.hitPattern.muonStationsWithValidHits>1",
                "outerTrack.normalizedChi2<10.",
            ]
        )
    ),
)
