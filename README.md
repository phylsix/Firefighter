Firefighter -- ntuplizer for SIDM
---------------------------------

## Setup

---

- 2017
```bash
export SCRAM_ARCH=slc6_amd64_gcc630
cmsrel CMSSW_9_4_12
cd CMSSW_9_4_12/src
cmsenv
```

- 2018
```bash
export SCRAM_ARCH=slc6_amd64_gcc700
cmsrel CMSSW_10_2_8
cd CMSSW_10_2_8/src
cmsenv
```

---

```bash
git clone https://YOURCERNID@gitlab.cern.ch/wsi/Firefighter.git
cd Firefighter
scram b -j12
```
