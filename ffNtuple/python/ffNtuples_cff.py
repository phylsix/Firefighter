import FWCore.ParameterSet.Config as cms

import os
import sys

cmsrel = os.environ["CMSSW_VERSION"]

if cmsrel.startswith("CMSSW_8"):
    year = 2016
elif cmsrel.startswith("CMSSW_9"):
    year = 2017
elif cmsrel.startswith("CMSSW_10"):
    year = 2018
else:
    sys.exit("Wrong release! Not in the year of [2016, 2017, 2018]")

from Firefighter.ffNtuple.ffNtuples_cfi import *

if year == 2018:
    ntuple_hlt.TriggerPaths.extend(
        [
            "HLT_DoubleL2Mu23NoVtx_2Cha",
            "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
            "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
            "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
            "HLT_DoubleL2Mu25NoVtx_2Cha",
            "HLT_DoubleL2Mu25NoVtx_2Cha_NoL2Matched",
            "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed",
            "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_NoL2Matched",
            "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
            "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",
        ]
    )

if year == 2016:
    ntuple_hlt.TriggerPaths = [
        "HLT_L2DoubleMu23_NoVertex",
        "HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10",
        "HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10",
    ]

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
