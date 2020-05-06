# Firefighter -- for SIDM

[![pipeline status](https://gitlab.cern.ch/wsi/Firefighter/badges/master/pipeline.svg)](https://gitlab.cern.ch/wsi/Firefighter/commits/master)


## Ntuple branch documentation

<table style='font-family: monospace;'><tr>
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

## Official ID applied
| name | version | TWiki | code |
| ---- | ------- | ----- | ---- |
| electron ID | `cutBasedElectronID-Fall17-94X-V2-loose` | [CutBasedElectronIdentificationRun2](https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_for_92X) |  [`/recoStuff/python/LeptonjetClustering_cff.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/recoStuff/python/LeptonjetClustering_cff.py#L16)<br />[`/ffNtuple/python/ffNtuples_cfi.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffNtuple/python/ffNtuples_cfi.py#L81-82) |
| photon ID | `cutBasedPhotonID-Fall17-94X-V2-loose` | [CutBasedPhotonIdentificationRun2](https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2) | [`/recoStuff/python/LeptonjetClustering_cff.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/recoStuff/python/LeptonjetClustering_cff.py#L21)<br />[`/ffNtuple/python/ffNtuples_cfi.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffNtuple/python/ffNtuples_cfi.py#L99-100) |
| muon ID | `CutBasedIdLoose` | [MuonIdRun2](https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Muon_selectors_Since_9_4_X) | [`/recoStuff/plugins/LeptonjetSourcePFMuonProducer.cc`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffNtuple/python/ffNtuples_cfi.py#L99-100) |
| muon Iso | `PFIsoLoose` | [MuonIdRun2](https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Muon_selectors_Since_9_4_X) | [`/recoStuff/plugins/LeptonjetSourcePFMuonProducer.cc`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/recoStuff/plugins/LeptonjetSourcePFMuonProducer.cc#L44-45)<br />[`/recoStuff/plugins/LeptonjetSourceDSAMuonProducer.cc`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/recoStuff/plugins/LeptonjetSourceDSAMuonProducer.cc#L65-66) |
| jet ID | `jetId` | [JetID13TeVRun2018](https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2018) | [`/recoStuff/python/jetIdentificationDefs_cfi.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/recoStuff/python/jetIdentificationDefs_cfi.py) |
| jet JEC | MC: `Autumn18_v19`/`102X_upgrade2018_realistic_v20`<br/>Data: `Autumn18_RunABCD_v19`/`102X_dataRun2_v12` (2018ABC),`102X_dataRun2_Prompt_v15` (2018D) | [JECDataMC](https://twiki.cern.ch/twiki/bin/view/CMS/JECDataMC) |**NOTE**: ffSuperConfigs will need to be re-generated to reflect updates on globalTags<br />  [`/ffConfig/python/datasetUtils.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffConfig/python/datasetUtils.py)<br/>[`/ffConfig/python/production/Autumn18/data/generateyaml.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffConfig/python/production/Autumn18/data/generateyaml.py)<br />[`/ffConfig/python/production/Skim2Lj18/data/generateyaml.py`](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffConfig/python/production/Skim2LJ18/data/generateyaml.py) |


## centrally produced scale factors
- electron: [Link](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2#102X_series_Dataset_2018_Autumn)
- photon: [Link](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2#102X_series_Fall17V2_IDs_Sca_AN1)
- muon: [Link](https://gitlab.cern.ch/cms-muonPOG/MuonReferenceEfficiencies/-/tree/master/EfficienciesStudies%2F2018)