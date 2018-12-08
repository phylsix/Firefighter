import FWCore.ParameterSet.Config as cms


selectedTracks = cms.EDFilter(
    "TrackSelector",
    src = cms.InputTag("displacedStandAloneMuons"),
    cut = cms.string(
        "pt>5 \
        && abs(eta)<2.4 \
        && hitPattern.numberOfValidMuonHits>16 \
        && hitPattern.muonStationsWithValidHits>2 \
        && normalizedChi2<4"
    )
)