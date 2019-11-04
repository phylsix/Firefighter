import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Type1MET.correctionTermsPfMetType1Type2_cff import *
from JetMETCorrections.Type1MET.correctionTermsPfMetType0PFCandidate_cff import *
from JetMETCorrections.Type1MET.correctionTermsPfMetMult_cff import *
from JetMETCorrections.Type1MET.correctedMet_cff import pfMetT1, pfMetT0pcT1, pfMetT0pcT1Txy


correctionTermsPfMetType1Task = cms.Task(ak4PFCHSL1FastL2L3ResidualCorrectorTask, #Data full chain
                                         ak4PFCHSL1FastL2L3CorrectorTask, #MC last corrector, previous are already in the data chain
                                         corrPfMetType1,)
ffMetCorrectionsSeq = cms.Sequence(correctionTermsPfMetType0PFCandidate
                                   + correctionTermsPfMetMult
                                   + pfMetT1 + pfMetT0pcT1 + pfMetT0pcT1Txy,
                                   correctionTermsPfMetType1Task)