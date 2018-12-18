import FWCore.ParameterSet.Config as cms

import os
import sys
if os.environ['CMSSW_VERSION'].startswith('CMSSW_9'):
    year = 2017
elif os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
    year = 2018
else:
    sys.exit('Wrong release!')

from Firefighter.ffNtuple.ffNtuples_cfi import *

if year==2018:
    ntuple_hlt.TriggerPaths.extend([
        'HLT_DoubleL2Mu23NoVtx_2Cha',
        'HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched',
        'HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed',
        'HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched'
    ])

ffNtuplesSeq = cms.Sequence(ffNtuplizer)