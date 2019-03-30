from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection


addJetCollection(
    proc,
    labelName=mod["PATJetsLabel"],
    jetSource=cms.InputTag(mod["PFJets"]),
    postfix=postFix,
    algo=jetalgo,
    rParam=jetSize,
    jetCorrections=JEC if JEC is not None else None,
    pfCandidates=cms.InputTag(pfCand),
    svSource=cms.InputTag(svLabel),
    genJetCollection=cms.InputTag(mod["GenJetsNoNu"]),
    pvSource=cms.InputTag(pvLabel),
    muSource=cms.InputTag(muLabel),
    elSource=cms.InputTag(elLabel),
    btagDiscriminators=bTagDiscriminators,
    btagInfos=bTagInfos,
    getJetMCFlavour=GetJetMCFlavour,
    genParticles=cms.InputTag(genParticlesLabel),
    outputModules=["outputFile"],
)
