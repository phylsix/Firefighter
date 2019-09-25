# Firefighter -- for SIDM

[![pipeline status](https://gitlab.cern.ch/wsi/Firefighter/badges/master/pipeline.svg)](https://gitlab.cern.ch/wsi/Firefighter/commits/master)

## Setup

---

```bash
export SCRAM_ARCH=slc6_amd64_gcc700
cmsrel CMSSW_10_2_14
cd CMSSW_10_2_14/src
cmsenv

# setup EGamma ID. check
# https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPostRecoRecipes#2018_Data_MC
# in case of updates. This is slow..
git cms-init
git cms-merge-topic cms-egamma:EgammaPostRecoTools
git cms-merge-topic cms-egamma:PhotonIDValueMapSpeedup1029
git cms-merge-topic cms-egamma:slava77-btvDictFix_10210
git cms-addpkg EgammaAnalysis/ElectronTools
rm EgammaAnalysis/ElectronTools/data -rf
git clone git@github.com:cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data

# checkout leptonjet reco+ntuple code
git clone https://YOURCERNID@gitlab.cern.ch/wsi/Firefighter.git
scram b -j12
```
