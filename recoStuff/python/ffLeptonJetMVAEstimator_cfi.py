import FWCore.ParameterSet.Config as cms

ffLeptonJetMVAEstimatorParam = cms.PSet(
    estimatorName=cms.string("leptonJetXGB"),
    mvaTag=cms.string("default"),
    nCategories=cms.int32(2),
    weightFileNames=cms.vstring(
        "Firefighter/ffConfig/data/weights_notrack.xml.gz",
        "Firefighter/ffConfig/data/weights_tracked.xml.gz",
    ),
    categoryCuts=cms.vstring(
        "(neutralEmEnergy+neutralHadronEnergy)/energy>=0.999",
        "(neutralEmEnergy+neutralHadronEnergy)/energy<0.999",
    ),
)
