import FWCore.ParameterSet.Config as cms
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
        ntuple_genjet,
        ntuple_hlt,
        ntuple_dsamuon,
        ntuple_dsamuonextra,
        ntuple_triggerobjectmatching,
        ntuple_metfilters,
    ),
)


ffNtuplesSeq = cms.Sequence(ffNtuplizer)
