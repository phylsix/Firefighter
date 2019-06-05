import FWCore.ParameterSet.Config as cms

# https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#2018_data
# https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/PhysicsTools/PatAlgos/python/slimming/metFilterPaths_cff.py

from RecoMET.METFilters.metFilters_cff import (
    HBHENoiseFilterResultProducer,
    HBHENoiseFilter,
    HBHENoiseIsoFilter,
    globalSuperTightHalo2016Filter,
    globalTightHalo2016Filter,
    CSCTightHalo2015Filter,
    EcalDeadCellTriggerPrimitiveFilter,
    BadPFMuonFilter,
    ecalBadCalibFilter,
)
from Firefighter.recoStuff.ffPrimaryVertexFilter_cfi import primaryVertexFilter

globalSuperTightHalo2016Filter.taggingMode = cms.bool(True)
globalTightHalo2016Filter.taggingMode = cms.bool(True)
CSCTightHalo2015Filter.taggingMode = cms.bool(True)
EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)
BadPFMuonFilter.taggingMode = cms.bool(True)
ecalBadCalibFilter.taggingMode = cms.bool(True)
primaryVertexFilter.taggingMode = cms.bool(True)

ffMetFilterSeq = cms.Sequence(
    HBHENoiseFilterResultProducer
    # + HBHENoiseFilter + HBHENoiseIsoFilter # this will actually do filtering
    + primaryVertexFilter
    + globalSuperTightHalo2016Filter
    + globalTightHalo2016Filter
    + CSCTightHalo2015Filter
    + EcalDeadCellTriggerPrimitiveFilter
    + BadPFMuonFilter
    + ecalBadCalibFilter
)
