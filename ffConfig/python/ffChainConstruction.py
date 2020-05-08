#!/usr/bin/env python
import FWCore.ParameterSet.Config as cms
from Firefighter.recoStuff.HLTFilter_cfi import hltfilter


def skimFullEvents(process, ffConfig, fileName):
    """skim off events with >0 leptonjets, track down stats also"""

    process.load("Firefighter.ffEvtFilters.EventFiltering_cff")
    # process.load("Firefighter.recoStuff.ffMetFilters_cff")
    process.load("Firefighter.recoStuff.DsaToPFCandidate_cff")
    process.load("Firefighter.recoStuff.LeptonjetClustering_cff")
    # process.load("Firefighter.recoStuff.ffDeepFlavour_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_v2_cff")
    process.load("Firefighter.recoStuff.skimOutput_cfi")
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,era='2018-Prompt', isMiniAOD=False)
    process.recoSeq = cms.Sequence(
        process.ffBeginEventFilteringSeq # cosmic + triggerobjectmatch
        # + process.ffMetFilterSeq
        + process.dSAToPFCandSeq
        + process.egammaPostRecoSeq
        + process.leptonjetClusteringSeq
        + process.leptonjetFilteringSeq
        + process.ffLeptonJetPairCountFilter
        # + process.ffDeepFlavourSeq
        + process.ffEndEventFilteringSeq
        )
    process.ntuple_step = cms.Path(process.recoSeq)
    process.stathistory = cms.Path(process.ffNtuplesStatSeq)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.fullOutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('ntuple_step'))
    process.fullOutput.fileName = cms.untracked.string(fileName)
    process.output_step = cms.EndPath(process.fullOutput)
    process.schedule = cms.Schedule(process.stathistory,
                                    process.ntuple_step,
                                    process.output_step,)


    if ffConfig["data-spec"]["dataType"] == "sigmc":
        raise NotImplementedError("signal MC does not need full event skimming")
    else: # bkg/data
        ## keep triggered events only ##
        process.hltfilter = hltfilter
        process.ffBeginEventFilteringSeq.insert(0, process.hltfilter)
        ## switch JEC label
        if ffConfig["data-spec"]["dataType"] == "data":
            process.recoSeq.insert(-1, process.ak4PFCHSL1FastL2L3ResidualCorrectorChain)
            process.leptonjetExtra.jetCorrector=cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector")
        else:
            process.recoSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)



    if ffConfig["reco-spec"]["eventRegion"] == "all":
        pass
    elif ffConfig["reco-spec"]["eventRegion"] == "single":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_single)
    elif ffConfig["reco-spec"]["eventRegion"] == "signal":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_signal)
    elif ffConfig["reco-spec"]["eventRegion"] == "control":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_control)
    elif ffConfig["reco-spec"]["eventRegion"] == "proxy":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_proxy)
    else:
        msg = "ffConfig['reco-spec']['eventRegion'] can only be \
            'all'/'single'/'signal'/'control'/'proxy'! --- {0} is given.".format(
            ffConfig["reco-spec"]["eventRegion"]
        )
        raise ValueError(msg)

    return process



def leptonjetStudyProcess(process, ffConfig, keepskim=0):
    """Attach leptonjet study sequence to `process`"""

    process.load("Firefighter.ffEvtFilters.EventFiltering_cff")
    process.load("Firefighter.recoStuff.ffMetFilters_cff")
    process.load("Firefighter.recoStuff.DsaToPFCandidate_cff")
    process.load("Firefighter.recoStuff.LeptonjetClustering_cff")
    # process.load("Firefighter.recoStuff.Ak4chsPostLeptonjets_cff")
    process.load("Firefighter.recoStuff.ffDeepFlavour_cff")
    process.load("Firefighter.recoStuff.ffMetCorrections_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_v2_cff")
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,era='2018-Prompt', isMiniAOD=False)
    process.recoSeq = cms.Sequence(
        process.ffBeginEventFilteringSeq # cosmic + triggerobjectmatch (neither blocking)
        + process.ffMetFilterSeq
        + process.dSAToPFCandSeq
        + process.egammaPostRecoSeq
        + process.leptonjetClusteringSeq
        + process.leptonjetFilteringSeq
        # + process.ffLeptonJetSingleCountFilter
        # + process.ak4chsPostLeptonjetsSeq
        + process.ffDeepFlavourSeq
        + process.ffMetCorrectionsSeq
        + process.ffEndEventFilteringSeq
        )

    process.ntuple_step = cms.Path(process.recoSeq+process.ffNtuplesSeq)
    process.stathistory = cms.Path(process.ffNtuplesStatSeq)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.schedule = cms.Schedule(process.stathistory,
                                    process.ntuple_step,
                                    process.endjob_step)
    if keepskim:
        process.load("Firefighter.recoStuff.skimOutput_cfi")
        from Firefighter.recoStuff.skimOutput_cfi import customizeSkimOutputContent
        process.skimOutput.fileName=cms.untracked.string(ffConfig["data-spec"]["outputFileName"].replace('ffNtuple', 'ffSkimV2'))
        process.skimOutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('ntuple_step'))
        customizeSkimOutputContent(process, modulename='skimOutput', level=keepskim)
        process.output_step = cms.EndPath(process.skimOutput)
        process.schedule = cms.Schedule(process.stathistory,
                                        process.ntuple_step,
                                        process.endjob_step,
                                        process.output_step,)

    ###########################################################################
    ##                             non signal-mc                             ##
    ###########################################################################

    if ffConfig["data-spec"]["dataType"] == "sigmc":

        ## exclude genbkg branches from ffNtupling ##
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if not x.NtupleName.value().startswith("ffNtupleGenBkg")
            ]
        )
        ## JEC
        process.recoSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    else: # bkgmc | data

        ## keep triggered events only ##
        process.hltfilter = hltfilter
        process.ffBeginEventFilteringSeq.insert(0, process.hltfilter)

        ## filter MC related modules out ##
        # mcmodules = list()
        # process.ffLeptonJetSeq.visit(cms.ModuleNodeVisitor(mcmodules))
        # mcmodules = [m for m in mcmodules if m.type_().startswith("MC")]
        # for m in mcmodules:
        #     process.ffLeptonJetSeq.remove(m)

        ## exclude gen branches from ffNtupling ##
        ## background MC will exclude gen particle part
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if x.NtupleName.value()!="ffNtupleGen"
            ]
        )

        ## data will exclude any gen- related branches
        if ffConfig["data-spec"]["dataType"] == "data":
            process.ffNtuplizer.Ntuples = cms.VPSet(
                [
                    x
                    for x in process.ffNtuplizer.Ntuples
                    if 'gen' not in x.NtupleName.value().lower()
                ]
            )
            ## switch JEC label
            process.recoSeq.insert(-1, process.ak4PFCHSL1FastL2L3ResidualCorrectorChain)
            process.leptonjetExtra.jetCorrector=cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector")
            for ps in process.ffNtuplizer.Ntuples:
                if ps.NtupleName.value()=="ffNtupleAKJet":
                    setattr(ps, "corrector", cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"))
        else:
            ## JEC
            process.recoSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    ###########################################################################
    ##                              event region                             ##
    ###########################################################################

    if ffConfig["reco-spec"]["eventRegion"] == "all":
        pass
    elif ffConfig["reco-spec"]["eventRegion"] == "single":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_single)
    elif ffConfig["reco-spec"]["eventRegion"] == "signal":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_signal)
    elif ffConfig["reco-spec"]["eventRegion"] == "control":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_control)
    elif ffConfig["reco-spec"]["eventRegion"] == "proxy":
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_proxy)
    elif ffConfig["reco-spec"]["eventRegion"] == "muonType":
        process.ffBeginEventFilteringSeq.remove(process.hltfilter)
        process.ffEndEventFilteringSeq = cms.Sequence(process.ffEndEventFilteringSeq_muontype)
    else:
        msg = "ffConfig['reco-spec']['eventRegion'] can only be \
            'all'/'single'/'signal'/'control'/'proxy'/'muonType'! --- {0} is given.".format(
            ffConfig["reco-spec"]["eventRegion"]
        )
        raise ValueError(msg)
    ###########################################################################
    return process



def decorateProcessFF(process, ffConfig, keepskim=False):
    """Attach Firefighter RECO, ntuple -specific to the `process`, configure
    them with `ffConfig`
    """

    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")
    process.load("Firefighter.recoStuff.DsaAdditionalValues_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_cff")
    process.load("Firefighter.recoStuff.ffMetFilters_cff")
    process.load("Firefighter.recoStuff.ffDeepFlavour_cff")
    process.load("Firefighter.ffEvtFilters.EventFiltering_cff")
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,era='2018-Prompt', isMiniAOD=False)

    process.leptonjetSourcePFMuon = cms.EDProducer("LeptonjetSourcePFMuonProducer")
    process.recofilterSeq = cms.Sequence(
        process.ffBeginEventFilteringSeq
        + process.ffLeptonJetSeq
        + process.ffMetFilterSeq
        + process.ffDeepFlavourSeq
        + process.ffEndEventFilteringSeq
        + process.egammaPostRecoSeq
        + process.leptonjetSourcePFMuon
        + process.dsamuonExtraSeq
    )
    process.filteredLeptonJet.cut=cms.string(" && ".join([
        process.filteredLeptonJet.cut.value(),
        "!test_bit(electronMultiplicity(), 0)"
        ]))

    process.ntuple_step = cms.Path(process.recofilterSeq + process.ffNtuplesSeq)
    process.stathistory = cms.Path(process.ffNtuplesStatSeq)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.schedule = cms.Schedule(
        process.stathistory,
        process.ntuple_step,
        process.endjob_step,
    )

    if keepskim:
        process.load("Firefighter.recoStuff.skimOutput_cfi")
        process.skimOutput.fileName=cms.untracked.string(ffConfig["data-spec"]["outputFileName"].replace('ffNtuple', 'ffSkim'))
        process.output_step = cms.EndPath(process.skimOutput)
        process.schedule = cms.Schedule(
            process.stathistory,
            process.ntuple_step,
            process.endjob_step,
            process.output_step,
        )

    ###########################################################################
    ##                             non signal-mc                             ##
    ###########################################################################

    if ffConfig["data-spec"]["dataType"] == "sigmc":

        ## exclude genbkg branches from ffNtupling ##
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if x.NtupleName.value()!="ffNtupleGenBkg"
            ]
        )
        ## JEC
        process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    else: # bkgmc | data

        ## keep triggered events only ##
        process.hltfilter = hltfilter
        process.ffLeptonJetSeq.insert(0, process.hltfilter)

        ## filter MC related modules out ##
        mcmodules = list()
        process.ffLeptonJetSeq.visit(cms.ModuleNodeVisitor(mcmodules))
        mcmodules = [m for m in mcmodules if m.type_().startswith("MC")]
        for m in mcmodules:
            process.ffLeptonJetSeq.remove(m)

        ## exclude gen branches from ffNtupling ##
        ## background MC will exclude gen particle part
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if x.NtupleName.value()!="ffNtupleGen"
            ]
        )

        ## data will exclude any gen- related branches
        if ffConfig["data-spec"]["dataType"] == "data":
            process.ffNtuplizer.Ntuples = cms.VPSet(
                [
                    x
                    for x in process.ffNtuplizer.Ntuples
                    if 'gen' not in x.NtupleName.value().lower()
                ]
            )
            ## switch JEC label
            process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3ResidualCorrectorChain)
            process.leptonjetExtra.jetCorrector=cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector")
            for ps in process.ffNtuplizer.Ntuples:
                if ps.NtupleName.value()=="ffNtupleAKJet":
                    setattr(ps, "corrector", cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"))
        else:
            ## JEC
            process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    ###########################################################################
    ##                              event region                             ##
    ###########################################################################

    if ffConfig["reco-spec"]["eventRegion"] == "all":
        pass
    elif ffConfig["reco-spec"]["eventRegion"] == "single":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_single
        )
    elif ffConfig["reco-spec"]["eventRegion"] == "signal":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_signal
        )
    elif ffConfig["reco-spec"]["eventRegion"] == "control":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_control
        )
    else:
        msg = "ffConfig['reco-spec']['eventRegion'] can only be 'all'/'single'/'signal'/'control'! --- {0} is given.".format(
            ffConfig["reco-spec"]["eventRegion"]
        )
        raise ValueError(msg)

    ###########################################################################
    ##                 leptonJet candidate selection strategy                ##
    ###########################################################################

    if ffConfig["reco-spec"]["leptonJetCandStrategy"] == "hadronFree":
        process.filteredPFCandsFwdPtr.src = cms.InputTag("particleFlowPtrs")
        process.filteredPFCandsFwdPtr.cut = cms.string(
            " && ".join(
                ["abs(eta)<2.5", "particleId!=1", "particleId!=5", "particleId!=6"]
            )
        )
        process.filteringPFCands = cms.Sequence(process.filteringPFCands_hadFree)
        process.ffLeptonJetFwdPtrs.src = cms.InputTag("ffLeptonJetHadFree")
    elif ffConfig["reco-spec"]["leptonJetCandStrategy"] == "CHS":
        process.filteringPFCands = cms.Sequence(process.filteringPFCands_CHS)
    else:
        msg = "ffConfig['reco-spec']['leptonJetCandStrategy'] can only be 'hadronFree'/'CHS'! --- {0} is given.".format(
            ffConfig["reco-spec"]["leptonJetCandStrategy"]
        )
        raise ValueError(msg)

    ###########################################################################

    if ffConfig["reco-spec"]["leptonJetCandStrategy"] == "hadronFree":
        process.ffLeptonJetProd = cms.Sequence(process.ffLeptonJetProd_hadFree)
    elif ffConfig["reco-spec"]["leptonJetCandStrategy"] == "CHS":
        pass
    else:
        msg = "ffConfig['reco-spec']['leptonJetCandStrategy'] can only be 'hadronFree'/'CHS'! --- {0} is given.".format(
            ffConfig["reco-spec"]["leptonJetCandStrategy"]
        )
        raise ValueError(msg)

    return process



###############################################################################


def decorateProcessFF_forTriggerStudy(process, ffConfig, keepskim=False,
                                      denompaths=['HLT_Mu17', 'HLT_IsoMu24']):
    """Attach Firefighter RECO, ntuple -specific to the `process`, configure
    them with `ffConfig`, for trigger study.
    """

    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")
    process.load("Firefighter.recoStuff.DsaAdditionalValues_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_cff")
    process.load("Firefighter.recoStuff.ffMetFilters_cff")
    process.load("Firefighter.recoStuff.ffDeepFlavour_cff")
    process.load("Firefighter.ffEvtFilters.EventFiltering_cff")
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,era='2018-Prompt', isMiniAOD=False)

    process.leptonjetSourcePFMuon = cms.EDProducer("LeptonjetSourcePFMuonProducer")
    process.recofilterSeq = cms.Sequence(
        process.ffBeginEventFilteringSeq
        + process.ffLeptonJetSeq
        + process.ffMetFilterSeq
        + process.ffDeepFlavourSeq
        + process.ffEndEventFilteringSeq
        + process.egammaPostRecoSeq
        + process.leptonjetSourcePFMuon
        + process.dsamuonExtraSeq
    )
    process.filteredLeptonJet.cut=cms.string(" && ".join([
        process.filteredLeptonJet.cut.value(),
        "!test_bit(electronMultiplicity(), 0)"
        ]))

    process.ntuple_step = cms.Path(process.recofilterSeq + process.ffNtuplesSeq)
    process.stathistory = cms.Path(process.ffNtuplesStatSeq)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.schedule = cms.Schedule(
        process.stathistory,
        process.ntuple_step,
        process.endjob_step,
    )

    if keepskim:
        process.load("Firefighter.recoStuff.skimOutput_cfi")
        process.skimOutput.fileName=cms.untracked.string(ffConfig["data-spec"]["outputFileName"].replace('ffNtuple', 'ffSkim'))
        process.output_step = cms.EndPath(process.skimOutput)
        process.schedule = cms.Schedule(
            process.stathistory,
            process.ntuple_step,
            process.endjob_step,
            process.output_step,
        )

    ###########################################################################
    ##                         denom trigger paths                           ##
    ###########################################################################

    _ffNtuplizerMods = process.ffNtuplizer.Ntuples
    ffNtupleHLTidx = _ffNtuplizerMods.index(
        [m for m in _ffNtuplizerMods if m.NtupleName=='ffNtupleHLT'][0])
    process.ffNtuplizer.Ntuples[ffNtupleHLTidx].TriggerPaths.extend(denompaths)

    ###########################################################################
    ##                             non signal-mc                             ##
    ###########################################################################

    if ffConfig["data-spec"]["dataType"] == "sigmc":

        ## exclude genbkg branches from ffNtupling ##
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if x.NtupleName.value()!="ffNtupleGenBkg"
            ]
        )
        ## JEC
        process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    else: # bkgmc | data

        ## keep triggered events only ##
        process.hltfilter = hltfilter
        ## keep denominator paths triggered events
        process.hltfilter.TriggerPaths.extend(denompaths)
        process.ffLeptonJetSeq.insert(0, process.hltfilter)

        ## filter MC related modules out ##
        mcmodules = list()
        process.ffLeptonJetSeq.visit(cms.ModuleNodeVisitor(mcmodules))
        mcmodules = [m for m in mcmodules if m.type_().startswith("MC")]
        for m in mcmodules:
            process.ffLeptonJetSeq.remove(m)

        ## exclude gen branches from ffNtupling ##
        ## background MC will exclude gen particle part
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if x.NtupleName.value()!="ffNtupleGen"
            ]
        )

        ## data will exclude any gen- related branches
        if ffConfig["data-spec"]["dataType"] == "data":
            process.ffNtuplizer.Ntuples = cms.VPSet(
                [
                    x
                    for x in process.ffNtuplizer.Ntuples
                    if 'gen' not in x.NtupleName.value().lower()
                ]
            )
            ## switch JEC label
            process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3ResidualCorrectorChain)
            process.leptonjetExtra.jetCorrector=cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector")
            for ps in process.ffNtuplizer.Ntuples:
                if ps.NtupleName.value()=="ffNtupleAKJet":
                    setattr(ps, "corrector", cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"))
        else:
            ## JEC
            process.recofilterSeq.insert(-1, process.ak4PFCHSL1FastL2L3CorrectorChain)

    ###########################################################################
    ##                              event region                             ##
    ###########################################################################

    if ffConfig["reco-spec"]["eventRegion"] == "all":
        pass
    elif ffConfig["reco-spec"]["eventRegion"] == "single":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_single
        )
    elif ffConfig["reco-spec"]["eventRegion"] == "signal":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_signal
        )
    elif ffConfig["reco-spec"]["eventRegion"] == "control":
        process.ffEndEventFilteringSeq = cms.Sequence(
            process.ffEndEventFilteringSeq_control
        )
    else:
        msg = "ffConfig['reco-spec']['eventRegion'] can only be 'all'/'single'/'signal'/'control'! --- {0} is given.".format(
            ffConfig["reco-spec"]["eventRegion"]
        )
        raise ValueError(msg)

    ###########################################################################
    ##                 leptonJet candidate selection strategy                ##
    ###########################################################################

    if ffConfig["reco-spec"]["leptonJetCandStrategy"] == "hadronFree":
        process.filteredPFCandsFwdPtr.src = cms.InputTag("particleFlowPtrs")
        process.filteredPFCandsFwdPtr.cut = cms.string(
            " && ".join(
                ["abs(eta)<2.5", "particleId!=1", "particleId!=5", "particleId!=6"]
            )
        )
        process.filteringPFCands = cms.Sequence(process.filteringPFCands_hadFree)
        process.ffLeptonJetFwdPtrs.src = cms.InputTag("ffLeptonJetHadFree")
    elif ffConfig["reco-spec"]["leptonJetCandStrategy"] == "CHS":
        process.filteringPFCands = cms.Sequence(process.filteringPFCands_CHS)
    else:
        msg = "ffConfig['reco-spec']['leptonJetCandStrategy'] can only be 'hadronFree'/'CHS'! --- {0} is given.".format(
            ffConfig["reco-spec"]["leptonJetCandStrategy"]
        )
        raise ValueError(msg)

    ###########################################################################

    if ffConfig["reco-spec"]["leptonJetCandStrategy"] == "hadronFree":
        process.ffLeptonJetProd = cms.Sequence(process.ffLeptonJetProd_hadFree)
    elif ffConfig["reco-spec"]["leptonJetCandStrategy"] == "CHS":
        pass
    else:
        msg = "ffConfig['reco-spec']['leptonJetCandStrategy'] can only be 'hadronFree'/'CHS'! --- {0} is given.".format(
            ffConfig["reco-spec"]["leptonJetCandStrategy"]
        )
        raise ValueError(msg)

    return process
