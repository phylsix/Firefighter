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
        ntuple_genjetmet,
        ntuple_hlt,
        ntuple_electron,
        ntuple_muon,
        # ntuple_pfmuon,
        ntuple_dsamuon,
        # ntuple_dsamuonextra,
        ntuple_photon,
        ntuple_triggerobjectmatching,
        ntuple_metfilters,
        ntuple_cosmicveto,
        ntuple_cosmicmuononeleg,
        ntuple_akjet,
        ntuple_hftagscore,
        # ntuple_akjetnolj,
        ntuple_pfjet,
        ntuple_pfjetextra,
        ntuple_leptonjetsrc,
        ntuple_proxymuon,
        ntuple_pfmet,
        # ntuple_genbkgcheck,
    ),
)

ntuple_pfjet.doVertexing=cms.bool(True)
ntuple_pfjet.doSubstructureVariables=cms.bool(False)
ntuple_pfjet.doMVA=cms.bool(False)

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
ffNtuplesStatSeq = cms.Sequence(ffNtupleStat)