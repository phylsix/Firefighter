import FWCore.ParameterSet.Config as cms

ffLeptonJetMVAEstimatorParam = cms.PSet(
    estimatorName=cms.string("leptonJetXGB"),
    mvaTag=cms.string("default"),
    nCategories=cms.int32(1),
    weightFileNames=cms.vstring("Firefighter/ffConfig/data/leptonJetXBG.xml"),
    categoryCuts=cms.vstring("pt>=0."),
)
