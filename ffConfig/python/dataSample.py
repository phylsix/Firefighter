#!/usr/bin/env python
import os

CMSSW_BASE = os.environ["CMSSW_BASE"]
GLOBAL_DIRECTOR = "root://cmsxrootd.fnal.gov/"
LOCAL_DIRECTOR = "root://cmseos.fnal.gov/"

samples = {
    "signal-4mu": LOCAL_DIRECTOR
    + "/store/group/lpcmetx/MCSIDM/AODSIM/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250/181228_055735/0000/SIDM_AODSIM_1.root",
    "signal-2mu2e": LOCAL_DIRECTOR
    + "/store/group/lpcmetx/MCSIDM/AODSIM/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-5_ctau-250/181228_061110/0000/SIDM_AODSIM_1.root",
    "DYTo2L_M10To50": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/DYJetsToLL_M-10to50_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/270000/FE785A80-4AF8-4740-9C23-42E4F5CD1D48.root",
    "DYTo2L_M50": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/00001/6B626859-4FE0-3143-8CEA-5A4A836214E4.root",
    "JpsiToMuMu": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/JpsiToMuMu_JpsiPt8_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/1110000/50BAD636-D0A9-1249-8D41-85F3FD348064.root",
    "tW": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/20000/FFFBE773-3FE4-3142-B20C-A8986ECBE6CB.root",
    "WJetsToLNu_HT400To600": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/90000/FE3914C6-7FEE-DD45-89E9-E0C60FA1640F.root",
    "WWZ": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/WWZ_TuneCP5_13TeV-amcatnlo-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/80000/29FAA129-6A94-6144-BBEA-246829F24253.root",
    "WZ": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/WZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v3/10000/FF2C4164-56F2-864F-A25A-AA2C7F54F802.root",
    "ZZ": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/ZZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/110000/1BE38E5F-9F8A-3C47-9AEE-0CFD8D5E2EF1.root",
    "ZZTo2L2Nu": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/20000/FDF58147-9D2C-804E-9865-77691C66636C.root",
    "ZZZ": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/100000/EE36B8E0-B9E2-7943-9DD4-DBD9D0573AD7.root",
    "ZZTo4L":
    # GLOBAL_DIRECTOR+'/store/mc/RunIIAutumn18DRPremix/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/10000/02230ABB-29B0-464B-8A12-C208717C96DF.root',
    "/uscms/home/wsi/nobackup/lpcdm/CMSSW_10_2_8/src/Firefighter/ffLite/PFEnergy/ZZTo4L/pickevents.root",
    "Cosmics": GLOBAL_DIRECTOR
    + "/store/data/Run2018A/Cosmics/AOD/06Jun2018-v1/80000/FEBEAF7F-FD71-E811-86DA-782BCB3BCA77.root",
    "QCD_Pt-300to470": GLOBAL_DIRECTOR
    + "/store/mc/RunIIAutumn18DRPremix/QCD_Pt-300to470_MuEnrichedPt5_TuneCP5_13TeV_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext3-v1/90000/F522418C-3CA0-5949-A14E-33576ECB15D4.root",
}

###############################################################################
## DoubleMuon Run2018 data samples
###############################################################################

samples.update(
    {
        "data_abc": GLOBAL_DIRECTOR
        + "/store/data/Run2018C/DoubleMuon/AOD/17Sep2018-v1/60000/594FF9F2-9547-F84E-A730-EC5587267D36.root",
        "data_d": GLOBAL_DIRECTOR
        + "/store/data/Run2018D/DoubleMuon/AOD/PromptReco-v2/000/325/022/00000/E8C70B3B-F319-8642-BBC1-E39535EFBDA9.root",
    }
)

###############################################################################
## NoBPTX data samples
###############################################################################

samples.update(
    {
        "NoBPTX": GLOBAL_DIRECTOR
        + "/store/data/Run2018A/NoBPTX/AOD/17Sep2018-v1/120000/6BB7B1EE-6C61-074F-823B-CFF5830BEEBF.root",
        "NoBPTXSkim": GLOBAL_DIRECTOR
        + "/store/data/Run2018A/NoBPTX/USER/EXONoBPTXSkim-17Sep2018-v1/120000/AB1F983B-E2F2-EB4B-AE96-A4A837B612F5.root",
    }
)


###############################################################################
## mu gun samples
###############################################################################

samples.update(
    {
        "muplus1000": [
            GLOBAL_DIRECTOR + x
            for x in """\
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/0170D3AA-919B-DC4C-846A-F75808EB682E.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/698851FD-51D0-2F48-A69B-F7FE1DFEB71F.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/1F0BB767-EB1B-6A46-902A-0A97FB83F6B4.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/924407D9-6ED1-4E41-95E7-1BE703B44DC0.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/29F4FFFA-C471-4E43-8A45-67B3D9409160.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/34698102-6AC4-7C48-A6AF-251EE4648B2E.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/5574D2BD-ECF2-5449-B276-BA85F81226B3.root
""".split()
        ],
        "muplus100": [
            GLOBAL_DIRECTOR + x
            for x in """\
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/FEED2F5E-2BD5-6C45-BDDD-F34DF72386E0.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/29C042E5-7E3B-194E-B15B-1CC9EEB0F050.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/760431DF-1B83-D54C-BFA6-7B2C19267FB3.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/802AA73A-748E-5247-9066-C5900D8A3424.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/DA148C46-0CAD-0343-8940-6C46EA161211.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/F3E80C0C-4214-3247-B377-7605E5C255E9.root
/store/mc/RunIIAutumn18DRPremix/Muplus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/A1CD85C5-E323-F342-8999-B95EF8E86242.root
""".split()
        ],
        "muminus1000": [
            GLOBAL_DIRECTOR + x
            for x in """\
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/6021883A-FD31-6A46-A89A-8588885645F4.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/0A70C8DA-550E-D64B-BE31-6C4598FC3F8B.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/76095377-4FC5-D141-8078-0C2F856933D7.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/D573EC27-C250-C14A-A4B2-2CDEEA71856D.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/A37A83CE-1135-2B49-9664-0A26FCFFF12F.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/8C6CA158-9BA1-724F-8261-4AB43B49A06C.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt1000-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/100000/856F82EC-722B-1646-AF6D-C65A9165131B.root
""".split()
        ],
        "muminus100": [
            GLOBAL_DIRECTOR + x
            for x in """\
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/ED1C9FE5-9F81-E445-B301-8C2DFA39E097.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/B17E45A0-9549-2B44-9E18-611168604A20.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/ACE33BB6-4F7D-F344-9FBE-163A196FFE25.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/7E9271F3-122D-9040-BBF7-4D944A338DB4.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/10129201-5774-004A-B695-B345F5944480.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/58171D8D-F374-DB46-843D-A9021E5C3399.root
/store/mc/RunIIAutumn18DRPremix/Muminus_Pt100-gun/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/120000/08F64FB7-425F-CB40-86C3-78D24AD6913D.root
""".split()
        ],
    }
)


###############################################################################
## for gitlab ci
###############################################################################

samples.update(
    {
        "ci-signal-4mu": CMSSW_BASE + "/src/Firefighter/ffNtuple/test/SIDM_4mu.root",
        "ci-signal-2mu2e": CMSSW_BASE
        + "/src/Firefighter/ffNtuple/test/SIDM_2mu2e.root",
        "ci-ZZ": CMSSW_BASE + "/src/Firefighter/ffNtuple/test/ZZ_numEvent300.root",
        "ci-data_abc": CMSSW_BASE
        + "/src/Firefighter/ffNtuple/test/DoubleMuon2018C_numEvent300.root",
    }
)


###############################################################################
## ffNtuple samples
###############################################################################


ffSamples = {
    "signal-4mu": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/ffNtuple_signal-4mu.root",
    "signal-2mu2e": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/ffNtuple_signal-2mu2e.root",
    "ZZ": CMSSW_BASE + "/src/Firefighter/ffNtuple/test/ffNtuple_ZZ.root",
    "QCD_Pt-300to470": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/ffNtuple_QCD_Pt-300to470.root",
    "Cosmics": CMSSW_BASE + "/src/Firefighter/ffNtuple/test/ffNtuple_Cosmics.root",
    "NoBPTXSkim": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/ffNtuple_NoBPTXSkim.root",
    "muplus1000": CMSSW_BASE
    + "/src/Firefighter/leptonJetGun/test/ffNtuple_muplus1000.root",
}


###############################################################################
## skimmed samples
###############################################################################


skimmedSamples = {
    "signal-4mu": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/skimffNtuple_signal-4mu.root",
    "signal-2mu2e": CMSSW_BASE
    + "/src/Firefighter/ffNtuple/test/skimffNtuple_signal-2mu2e.root",
}
