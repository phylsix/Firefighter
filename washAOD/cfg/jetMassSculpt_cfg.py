import FWCore.ParameterSet.Config as cms
from Firefighter.washAOD.setupProcess_cff import setup_process
from Firefighter.washAOD.mergeDSAwithPFCand_cff import addDSAmergeSeq

from JMEAnalysis.JetToolbox.jetToolbox_cff    import jetToolbox
from Firefighter.washAOD.jetMassSculpting_cfi import jetmasssculpt

process = cms.Process("USER")
process = setup_process(process,
                        testFileName='jetMassSculpt.root')

process = addDSAmergeSeq(process)

# AK4
jetToolbox( process, 'ak4', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            # addSoftDrop=True, addPruning=True, addTrimming=True, saveJetCollection=True,
            verbosity=0 )
process.ak4JetMass = jetmasssculpt.clone( src=cms.InputTag("ak4PFJetsComb", "", "USER") )

process.p1 = cms.Path( process.ak4JetMass )