import FWCore.ParameterSet.Config as cms
from Firefighter.washAOD.setupProcess_cff import setup_process
from Firefighter.washAOD.mergeDSAwithPFCand_cff import addDSAmergeSeq

from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
from Firefighter.washAOD.pfJetAnalysis_cfi import pfjetana

process = cms.Process("USER")
process = setup_process(process,
                        testFileName='jetTuplizer.root')

process = addDSAmergeSeq(process)

# AK4
jetToolbox( process, 'ak4', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            # addSoftDrop=True, addPruning=True, addTrimming=True, saveJetCollection=True,
            verbosity=0 )
# AK2
jetToolbox( process, 'ak2', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            verbosity=0 )
# AK1
jetToolbox( process, 'ak1', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            verbosity=0 )

pfjetana.kvfParam.maxDistance = 0.1
pfjetana.dsa = cms.InputTag("selectedDsaMuons")
process.ak4PfJet = pfjetana.clone( src=cms.InputTag("ak4PFJetsComb", "", "USER") )
process.ak2PfJet = pfjetana.clone( src=cms.InputTag("ak2PFJetsComb", "", "USER") )
process.ak1PfJet = pfjetana.clone( src=cms.InputTag("ak1PFJetsComb", "", "USER") )

process.p2 = cms.Path(
    process.ak4PfJet +
    process.ak2PfJet +
    process.ak1PfJet
)