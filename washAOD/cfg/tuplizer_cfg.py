import FWCore.ParameterSet.Config as cms
from Firefighter.washAOD.setupProcess_cff import setup_process

process = cms.Process("USER")
process, options = setup_process(process,
                                 testFileName='tuplizer.root')

from Firefighter.washAOD.genTuplizer_cfi import genTuplizer
process.GEN = genTuplizer.clone()

## reco efficiency
from Firefighter.washAOD.recoEffiForMuTrack_cfi import recoEffiForMuTrack
process.RECO_dsa = recoEffiForMuTrack.clone()
process.RECO_dgm = recoEffiForMuTrack.clone(muTrack = cms.InputTag("displacedGlobalMuons"))
process.RECO_rsa = recoEffiForMuTrack.clone(muTrack = cms.InputTag("refittedStandAloneMuons"))
process.RECO_gbm = recoEffiForMuTrack.clone(muTrack = cms.InputTag("globalMuons"))

## trigger efficiency in terms of events
from Firefighter.washAOD.trigEffiForMuTrack_cfi import trigEffiForMuTrack
## trigger efficiency itself
from Firefighter.washAOD.trigSelfEffiForMuTrack_cfi import trigSelfEffiForMuTrack

### 2017
if options.year == 2017:
    process.TRIG_dsa = trigEffiForMuTrack.clone(trigPath = cms.vstring('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'))
    process.TRIGself_dsa = trigSelfEffiForMuTrack.clone(trigPath = cms.vstring('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'), nMuons = cms.int32(3))

### 2018
if options.year == 2018:
    process.TRIG_dsa = trigEffiForMuTrack.clone(
        trigPath=cms.vstring("HLT_DoubleL2Mu23NoVtx_2Cha",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched")
    )
    process.TRIGself_dsa = trigSelfEffiForMuTrack.clone(
        trigPath=cms.vstring("HLT_DoubleL2Mu23NoVtx_2Cha",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
                             "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched")
    )


## constructing the path
process.p = cms.Path(process.GEN
                        + process.RECO_dsa
                    #  + process.RECO_dgm
                    #  + process.RECO_rsa
                    #  + process.RECO_gbm
                        + process.TRIG_dsa
                        + process.TRIGself_dsa
                        )