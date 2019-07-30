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

ecalBadCalibFilter.baddetEcallist = cms.vuint32(
    [
        872439604,
        872422825,
        872420274,
        872423218,
        872423215,
        872416066,
        872435036,
        872439336,
        872420273,
        872436907,
        872420147,
        872439731,
        872436657,
        872420397,
        872439732,
        872439339,
        872439603,
        872422436,
        872439861,
        872437051,
        872437052,
        872420649,
        872421950,
        872437185,
        872422564,
        872421566,
        872421695,
        872421955,
        872421567,
        872437184,
        872421951,
        872421694,
        872437056,
        872437057,
        872437313,
        872438182,
        872438951,
        872439990,
        872439864,
        872439609,
        872437181,
        872437182,
        872437053,
        872436794,
        872436667,
        872436536,
        872421541,
        872421413,
        872421414,
        872421031,
        872423083,
        872421439,
    ]
)

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
