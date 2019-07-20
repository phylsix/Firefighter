Firefighter/piedpiper
=====================

Package for submitting private signal MC jobs to CRAB.

MC generations consist of three major steps:
1. GEN-SIM
2. PREMIX-RAW-HLT
3. AODSIM

So we also submiting jobs in steps, under `Firefighter/piedpier/crab` directory:

### GEN-SIM
```bash
python multicrab-0.py config.yml
```

`config.yml` has following structure:
```yml
gridpacks:
    - gridpack_A
    - gridpack_B
    - ...
    - gridpack_Z
njobs: 5
year: 2018
lxy: 3
ctaus:
    - ctau_A
    - ctau_B
    - ...
    - ctau_Z
```

**lxy** is the target mean lxy of darkphotons. **ctaus** can be different due to different mass combinations/boost factors, but one must ensure one-to-one correspondance. Base formular is
> mXX/2/mA * ctau * 0.8 = lxy * 10

### PREMIX-RAW-HLT
```bash
python multicrab-1.py config.yml
```

`config.yml` has following structure:
```yml
gensimdatasets:
    - gendataset_A
    - gendataset_B
    - ...
    - gendataset_Z
year: 2018
```

### AODSIM
```bash
python multicrab-2.py config.yml
```

`config.yml` has following structure:
```yml
premixdigihltdatasets:
    - premixdigihltdataset_A
    - premixdigihltdataset_B
    - ...
    - premixdigihltdataset_Z
year: 2018
```
