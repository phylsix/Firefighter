# Firefighter -- for SIDM 

[![pipeline status](https://gitlab.cern.ch/wsi/Firefighter/badges/master/pipeline.svg)](https://gitlab.cern.ch/wsi/Firefighter/commits/master)

## Setup

---

```bash
export SCRAM_ARCH=slc6_amd64_gcc700
cmsrel CMSSW_10_2_14
cd CMSSW_10_2_14/src
cmsenv

git clone https://YOURCERNID@gitlab.cern.ch/wsi/Firefighter.git
cd Firefighter
scram b -j12
```
