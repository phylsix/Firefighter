import FWCore.ParameterSet.Config as cms

from Firefighter.ffNtuple.ffNtuples_cfi import *

ffNtuplesSeq = cms.Sequence(ffNtuplizer)