import FWCore.ParameterSet.Config as cms

from Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff import *
from Firefighter.recoStuff.HLTFilter_cfi import hltfilter

import os
import sys
cmsrel = os.environ['CMSSW_VERSION']

if cmsrel.startswith('CMSSW_8'):
    year = 2016
elif cmsrel.startswith('CMSSW_9'):
    year = 2017
elif cmsrel.startswith('CMSSW_10'):
    year = 2018
else:
    sys.exit('Wrong release! Not in the year of [2016, 2017, 2018]')

if year == 2016:
    hltfilter.TriggerPaths = cms.vstring(
        'HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10',
        'HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10'
    )
if year == 2017:
    hltfilter.TriggerPaths = cms.vstring(
        'HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx',
        'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'
    )

ffLeptonJetSeq._seq._collection.insert(0, hltfilter)

for _m in ffLeptonJetSeq._seq._collection:
    if _m._TypedParameterizable__type.startswith('MC'):
        ffLeptonJetSeq.remove(_m)
