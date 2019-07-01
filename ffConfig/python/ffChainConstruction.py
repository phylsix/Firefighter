#!/usr/bin/env python
import FWCore.ParameterSet.Config as cms
from Firefighter.recoStuff.HLTFilter_cfi import hltfilter


def decorateProcessFF(process, ffConfig):
    """Attach Firefighter RECO, ntuple -specific to the `process`, configure
    them with `ffConfig`
    """

    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_cff")
    process.load("Firefighter.recoStuff.ffMetFilters_cff")
    process.load("Firefighter.ffEvtFilters.EventFiltering_cff")

    process.recofilterSeq = cms.Sequence(
        process.ffBeginEventFilteringSeq
        + process.ffLeptonJetSeq
        + process.ffMetFilterSeq
        + process.ffEndEventFilteringSeq
    )

    process.ntuple_step = cms.Path(process.recofilterSeq + process.ffNtuplesSeq)
    process.stathistory = cms.Path(process.ffNtuplesStatSeq)
    process.endjob_step = cms.EndPath(process.endOfProcess)

    process.schedule = cms.Schedule(
        process.stathistory, process.ntuple_step, process.endjob_step
    )

    ###########################################################################
    ##                             non signal-mc                             ##
    ###########################################################################

    if ffConfig["data-spec"]["dataType"] != "sigmc":

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
        process.ffNtuplizer.Ntuples = cms.VPSet(
            [
                x
                for x in process.ffNtuplizer.Ntuples
                if "gen" not in x.NtupleName.value().lower()
            ]
        )

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
