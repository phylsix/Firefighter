# Firefighter -- for SIDM

[![pipeline status](https://gitlab.cern.ch/wsi/Firefighter/badges/master/pipeline.svg)](https://gitlab.cern.ch/wsi/Firefighter/commits/master)


## Ntuple branch documentation

<table><tr>
<td><a href='https://wsi.web.cern.ch/wsi/Firefighter/sigmc.html'>signal MC</a></td>
<td><a href='https://wsi.web.cern.ch/wsi/Firefighter/bkgmc.html'>background MC</a></td>
<td><a href='https://wsi.web.cern.ch/wsi/Firefighter/data.html'>data</a></td>
</tr></table>


## Setup

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
git clone https://github.com/cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data

# checkout leptonjet reco+ntuple code
git clone https://YOURCERNID@gitlab.cern.ch/wsi/Firefighter.git
scram b -j12
```
