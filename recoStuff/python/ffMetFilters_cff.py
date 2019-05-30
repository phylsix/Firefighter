import FWCore.ParameterSet.Config as cms

# https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#2018_data
# https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/PhysicsTools/PatAlgos/python/slimming/metFilterPaths_cff.py

from RecoMET.METFilters.metFilters_cff import (
    HBHENoiseFilterResultProducer,
    HBHENoiseFilter,
    HBHENoiseIsoFilter,
    primaryVertexFilter,
    globalSuperTightHalo2016Filter,
    globalTightHalo2016Filter,
    CSCTightHalo2015Filter,
    EcalDeadCellTriggerPrimitiveFilter,
    BadPFMuonFilter,
    ecalBadCalibFilter,
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
