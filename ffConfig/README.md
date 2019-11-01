# Firefighter/ffConfig

This is the subpackage managing the **configuration** and **job submission** (to crab and condor) system.

## ffSuperConfig.yml

Carrying the spirit of maximum separation of configuration files from application's functionality
implementation, the configurations fed to Firefighter reco/ntuple package have been redesigned. A
`ffSuperConfig.yml` config file will contain all necessary parameter settings needed to run a job.
As the suffix suggest, the config file is in YAML. Parameter settings will be parsed into python dict
object, and further distributed to the places where needed. A [default ffSuperConfig](https://gitlab.cern.ch/wsi/Firefighter/blob/master/ffConfig/cfg/ffSuperConfig.yml)
file can be seen:

```yml
job-spec:
  numThreads: 1
  reportEvery: 100

condition-spec:
  globalTag: "102X_upgrade2018_realistic_v18"

data-spec:
  inputFileList: []
  maxEvents: -1
  outputFileName: "ffNtuple.root"
  lumiMask: ""
  dataType: "bkgmc"

reco-spec:
  eventRegion: "control"
  leptonJetCandStrategy: "hadronFree"
```

There are 4 groups of parametersets: `job-spec`, `condition-spec`, `data-spec` and `reco-spec`.
They work together to describe the job in a complete way with customization open.

## ffDataSet

Large data samples grouped as dataset which jobs are running over are characterized by a python dict object,
serialized from yaml config files. A typical `ffDataSet.yml` has the following structure:

```yml
datasetNames: <list[str]>
maxEvents: <int>
globalTag: <str>
fileList: <list[list[str]]>
```

Configs for datasets used for analysis are organized under `Firefighter/ffConfig/python/production`,
a `description.json` is present under each leaf directory, which contains a list of file paths relative to
environment variable `CMSSW_BASE`. Values pertaining to a dataset will be used to construct `ffSuperConfig.yml`
and the following job(crab/condor) description file.

For other studies, sample configs can also be structured in a similar fashion, for easy management.

## crabConfigBuilder, condorConfigBuilder

CRAB and LPC condor are two batch job submission system available to use. Configuration options have a lot
in common but somewhat not set in a universal way.

`crabConfigBuilder` makes use of `CRABClient` python libarary with a well constructed `CRABClient.UserUtilities.config`
object to submit. Only dataset name need to be fill in, can split jobs by number of luminosity for data.

`condorConfigBuilder` will need to construct `condor.jdl` and `ffCondor.sh` to direct command execution
explicitly. The submission is achieved with command line tool `condor_submit` with the location of `condor.jdl`
as argument called in python. Jobs will be split based on number of files, list of input files are mannually
split and written into the `ffSuperConfig.yml`.

Both configBuilders will construct `ffSuperConfig.yml` for each job and expose it in the job submmission
processes. For CRAB, parameters are taken in and understood locally then converted to a post-configured pickled
pset and uploaded to crab server; while for condor, the physical files are uploaded to the container together
with the `ffCondor.sh`, the parsing and understanding happens in the worker nodes.

Two scripts, `ffBatchJobSubmitter_v2.py` and `ffGrandJobSubmitter.py` are provided for convenience.
