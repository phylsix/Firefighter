import FWCore.ParameterSet.Config as cms

from Firefighter.ffNtuple.ffNtuples_cff import *

ffNtuples = ffNtuplizer.clone()
ffNtuples.Ntuples = cms.VPSet(
    [
        x for x in ffNtuples.Ntuples \
        if 'gen' not in x.NtupleName.value().lower()
    ]
)

ffNtuplesSeq = cms.Sequence(ffNtuples)