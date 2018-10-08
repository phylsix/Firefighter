import FWCore.ParameterSet.Config as cms

mcgenfilter = cms.EDFilter("mcGenFilter",
                           srcCut = cms.string("abs(pdgId)==13 \
                                                && isHardProcess \
                                                && abs(eta)<2.4 \
                                                && vertex.rho<740 \
                                                && abs(vz)<960")
                           )