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
        ntuple_muon,
        ntuple_pfmuon,
        ntuple_dsamuon,
        ntuple_dsamuonextra,
        ntuple_photon,
        ntuple_triggerobjectmatching,
        ntuple_metfilters,
        ntuple_cosmicveto,
        ntuple_akjet,
        # ntuple_akjetnolj,
        ntuple_pfjet,
        ntuple_pfjetextra,
        ntuple_leptonjetsrc,
        # ntuple_genbkgcheck,
    ),
)

ntuple_pfjet.doVertexing=cms.bool(False)
ntuple_pfjet.doSubstructureVariables=cms.bool(False)
ntuple_pfjet.doMVA=cms.bool(False)
ntuple_leptonjetsrc.src=cms.InputTag("leptonjetSources")

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
ffNtuplesStatSeq = cms.Sequence(ffNtupleStat)