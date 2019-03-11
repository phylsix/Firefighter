#!/usr/bin/env python

samples = {
    'signal':
    'root://cmseos.fnal.gov//store/group/lpcmetx/MCSIDM/AODSIM/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250/181228_055735/0000/SIDM_AODSIM_1.root',
    'DYTo2L_M10To50':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/DYJetsToLL_M-10to50_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/270000/FE785A80-4AF8-4740-9C23-42E4F5CD1D48.root',
    'DYTo2L_M50':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/00001/6B626859-4FE0-3143-8CEA-5A4A836214E4.root',
    'JpsiToMuMu':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/JpsiToMuMu_JpsiPt8_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/1110000/50BAD636-D0A9-1249-8D41-85F3FD348064.root',
    'tW':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v1/20000/FFFBE773-3FE4-3142-B20C-A8986ECBE6CB.root',
    'WJetsToLNu_HT400To600':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/90000/FE3914C6-7FEE-DD45-89E9-E0C60FA1640F.root',
    'WWZ':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/WWZ_TuneCP5_13TeV-amcatnlo-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/80000/29FAA129-6A94-6144-BBEA-246829F24253.root',
    'WZ':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/WZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v3/10000/FF2C4164-56F2-864F-A25A-AA2C7F54F802.root',
    'ZZ':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/ZZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/110000/1BE38E5F-9F8A-3C47-9AEE-0CFD8D5E2EF1.root',
    'ZZTo2L2Nu':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/20000/FDF58147-9D2C-804E-9865-77691C66636C.root',
    'ZZZ':
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/100000/EE36B8E0-B9E2-7943-9DD4-DBD9D0573AD7.root',
    'ZZTo4L':
    # 'root://xrootd.unl.edu//store/mc/RunIIAutumn18DRPremix/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/60000/3248C8A6-00AC-E14A-B832-4E8B81E81BA2.root',
    '/uscms/home/wsi/nobackup/lpcdm/CMSSW_10_2_8/src/Firefighter/ffLite/PFEnergy/ZZTo4L/pickevents.root'
}

ffSamples = {
    'signal':
    # 'root://cmseos.fnal.gov///store/group/lpcmetx/MCSIDM/ffNtuple/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250/190227_043028/0000/ffNtuple_1.root',
    '/uscms/home/wsi/nobackup/lpcdm/CMSSW_10_2_8/src/Firefighter/ffNtuple/test/testffNtuple.root',
    'ZZTo4L':
    'root://cmseos.fnal.gov//store/group/lpcmetx/MCSIDM/ffNtuple/2018/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/190227_043143/0000/ffNtuple_1.root',
}
