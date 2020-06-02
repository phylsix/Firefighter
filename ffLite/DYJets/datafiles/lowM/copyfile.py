

import os

inputs='''\
1:90061:204798108 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_1000.root
1:75879:172548337 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_1004.root
1:57823:131488809 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_122.root
1:96800:220122374 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_158.root
1:58349:132683492 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_16.root
1:90350:205453932 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_204.root
1:49206:111893014 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_263.root
1:42661:97010313  root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_266.root
1:71334:162211873 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_294.root
1:72428:164700367 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_298.root
1:78664:178881086 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_303.root
1:88539:201337467 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_317.root
1:60986:138681185 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_588.root
1:19441:44208166  root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_591.root
1:62505:142134674 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_619.root
1:70304:159869961 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_688.root
1:75509:171706907 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_765.root
1:76489:173934403 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_79.root
1:45909:104394887 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_853.root
1:59905:136223608 root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_96.root
1:3682:8371771    root://cmseos.fnal.gov//eos/uscms/store/group/lpcmetx/SIDM/Skim/2018/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/191127_170326/ffAOD_999.root'''

for l in inputs.split('\n'):
    src = l.split(' ')[-1].replace('root://cmseos.fnal.gov/', '')
    cmd = 'cp {} .'.format(src)
    os.system(cmd)