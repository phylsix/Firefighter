import FWCore.ParameterSet.Config as cms

ffLeptonJetFwdPtrs = cms.EDProducer(
    "PFJetFwdPtrProducer", src=cms.InputTag("leptonjet")
)

filteredLeptonJet = cms.EDFilter(
    "ffLeptonJetFwdPtrSelector",
    src=cms.InputTag("ffLeptonJetFwdPtrs"),
    cut=cms.string(
        " && ".join(
            [
                "pt>20.",
                "abs(eta)<2.4",
                "numberOfDaughters>0",
                "!test_bit(muonMultiplicity(), 0)",  # even multiplicity
                #"!test_bit(electronMultiplicity(), 0)",  # even multiplicity
            ]
        )
    ),
    useChargedMass=cms.bool(False),
    minChargedMass=cms.double(0.0),
    useMuonTime=cms.bool(False),
    maxTimeLimit=cms.double(20.0),
    makeClones=cms.bool(True),
)

# filteredLeptonJet = cms.EDProducer(
#     "PFJetFromFwdPtrProducer", src=cms.InputTag("filteredLeptonJetPtrs")
# )

ffLeptonJetCountFilter = cms.EDFilter(
    "CandViewCountFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minNumber=cms.uint32(1),
)
