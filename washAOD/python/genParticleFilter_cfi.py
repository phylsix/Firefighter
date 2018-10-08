import FWCore.ParameterSet.Config as cms

genFilter = cms.EDFilter("genParticleFilter",
                         src = cms.InputTag("genParticles"),
                         minNumber = cms.uint32(4),
                         cut = cms.string("abs(pdgId)==13 \
                                            && isHardProcess \
                                            && abs(eta)<2.4 \
                                            && vertex.rho<740 \
                                            && abs(vz)<960")
                        )