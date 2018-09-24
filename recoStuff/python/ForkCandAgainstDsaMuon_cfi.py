import FWCore.ParameterSet.Config as cms

forkcandagainstdsamuon = cms.EDProducer("ForkCandAgainstDsaMuon",
                                        targetcol=cms.InputTag("particleFlow"),
                                        toforkcol=cms.InputTag("dSAmuPFCand"),
                                        maxDR=cms.double(0.1),
                                        maxPtReso=cms.double(0.5)
                                        )