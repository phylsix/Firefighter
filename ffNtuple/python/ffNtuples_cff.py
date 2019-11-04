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

ffNtuplizer = cms.EDAnalyzer(
    "ffNtupleManager",
    HltProcName=cms.string("HLT"),
    Ntuples=cms.VPSet(
        ntuple_event,
        ntuple_genevent,
        ntuple_primaryvertex,
        ntuple_gen,
        ntuple_genbkg,
        ntuple_genjetmet,
        ntuple_hlt,
        ntuple_muon,
        ntuple_electron,
        ntuple_photon,
        ntuple_dsamuon,
        ntuple_pfjet,
        ntuple_akjet,
        ntuple_hftagscore,
        # ntuple_muontiming,
        # ntuple_beamhalo,
        ntuple_metfilters,
        ntuple_leptonjetsrc,
        ntuple_leptonjetmisc,
    ),
)

if year == 2017:
    ntuple_hlt.TriggerPaths = [
        "HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx",
        "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx",
    ]


if year == 2016:
    ntuple_hlt.TriggerPaths = [
        "HLT_L2DoubleMu23_NoVertex",
        "HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10",
        "HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10",
    ]


ffNtuplesSeq = cms.Sequence(ffNtuplizer)
ffNtuplesStatSeq = cms.Sequence(ffNtupleStat)