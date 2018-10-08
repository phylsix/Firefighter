import FWCore.ParameterSet.Config as cms
from Firefighter.washAOD.setupProcess_cff import setup_process
from Firefighter.washAOD.mcGenFilter_cfi   import mcgenfilter


process = cms.Process("USER")
process = setup_process(process,
                        testFileName='mcGenFilter.root')

process.mcgenfilter = mcgenfilter.clone()
process.p = cms.Path( process.mcgenfilter )