#!/usr/bin/env python
"""condorConfigBuilder takes an ffDataSet config and addtional kwargs. It will
spawn ready-to-submit condor job configs per ffDataSet.
"""
from __future__ import print_function

import os
import time
from os.path import join, basename, dirname, abspath

import yaml
from Firefighter.ffConfig.datasetUtils import (get_datasetType, get_nametag,
                                               get_primaryDatasetName,
                                               get_submissionSites)

# replaced vars: FFSUPERCONFIGDIR, CMSSWBASENAME, JOBIDLIST
CONDORJDL = """\
universe = vanilla
+REQUIRED_OS = "rhel6"
+DesiredOS = REQUIRED_OS
Executable = FFSUPERCONFIGDIR/ffCondor.sh
request_memory = 4 GB
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Transfer_Input_Files = FFSUPERCONFIGDIR/ffCondor.sh, FFSUPERCONFIGDIR/ffSuperConfig_$(jobid).yml, CMSSWBASENAME.tar.gz
Output = FFSUPERCONFIGDIR/ffCondor_$(jobid).stdout
Error = FFSUPERCONFIGDIR/ffCondor_$(jobid).stderr
Log = FFSUPERCONFIGDIR/ffCondor_$(jobid).log
x509userproxy = $ENV(X509_USER_PROXY)
Arguments = FFSUPERCONFIGDIR/ffSuperConfig_$(jobid).yml
want_graceful_removal = true
on_exit_remove = (ExitBySignal == False) && (ExitCode == 0)
on_exit_hold = ( (ExitBySignal == True) || (ExitCode != 0) )
on_exit_hold_reason = strcat("Job held by ON_EXIT_HOLD due to ", ifThenElse((ExitBySignal == True), "exit by signal", strcat("exit code ",ExitCode)), ".")
queue 1 jobid in JOBIDLIST"""


# replaced vars; CMSSWBASENAME, FFCONFIGNAME, OUTPUTBASE
EXECSHELL = r"""#!/bin/bash
set -x
echo "Starting Firefighter job on " `date`
echo "Running on: `uname -a`"
echo "System software: `cat /etc/redhat-release`"
echo "Current directory: `pwd`"
echo "Containing files:"
ls -alrth
CWD=`pwd`
source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc700
tar xzf CMSSWBASENAME.tar.gz
cd CMSSWBASENAME/src/
echo ">> scram b ProjectRename"
scram b ProjectRename
eval `scramv1 runtime -sh`
echo "CMSSW: "$CMSSW_BASE
YMLCFG=`basename $1`
echo "Argument ffSuperConfig is: $YMLCFG"
cmsRun $CMSSW_BASE/src/Firefighter/ffConfig/cfg/FFCONFIGNAME config=$CWD/$YMLCFG 2>&1

CMSEXIT=$?
if [[ $CMSEXIT -ne 0 ]]; then
	rm *.root
	echo "exit code $CMSEXIT, skipping xrdcp"
	exit $CMSEXIT
fi

echo "List all root files = "
ls -alrth *.root
echo "List all files"
ls -alrth
echo "*******************************************"
OUTDIR=root://cmseos.fnal.gov/OUTPUTBASE
echo "xrdcp output for condor"
for FILE in *.root
do
  echo "xrdcp -f ${FILE} ${OUTDIR}/${FILE}"
  xrdcp -f ${FILE} ${OUTDIR}/${FILE} 2>&1
  XRDEXIT=$?
  if [[ $XRDEXIT -ne 0 ]]; then
    rm *.root
    echo "exit code $XRDEXIT, failure in xrdcp"
    exit $XRDEXIT
  fi
  rm ${FILE}
done"""


class configBuilder:
    def __init__(self, ffdataset, **kwargs):
        self.ffdataset_ = ffdataset
        self.specs_ = dict(
            numThreads=1,
            maxMemory=2000,
            reportEvery=10000,
            outputFileName='ffNtuple.root',
            skimFileName='ffAOD.root',
            leptonJetCandStrategy='hadronFree',
            eventRegion='control',
            workArea=join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/condorGarage/', time.strftime("%y%m%d")),
            unitsPerJob=10,
            year=2018,
            redirector='',
            ffConfigName='ffNtupleFromAOD_v2_cfg.py',
            outbase='/store/group/lpcmetx/SIDM/ffNtupleV2/'
        )
        self.specs_.update(kwargs)
        self.specs_['outLFNDirBase'] = join(self.specs_['outbase'], str(self.specs_['year']))

    def build(self):

        res = [] # hold paths to condorJDL files

        ## create workarea dir ##
        try:
            os.makedirs(self.specs_['workArea'])
        except:
            pass

        ## ffSuperConfig ##
        ffsc = {
            "job-spec": dict(
                numThreads=self.specs_['numThreads'],
                reportEvery=self.specs_['reportEvery'],
            ),
            "condition-spec": dict(
                globalTag=self.ffdataset_['globalTag']
            ),
            "data-spec": dict(
                maxEvents=self.ffdataset_['maxEvents'],
                lumiMask=self.ffdataset_.get('lumiMask', None),
            ),
            "reco-spec": dict(
                eventRegion=self.specs_['eventRegion'],
                leptonJetCandStrategy=self.specs_['leptonJetCandStrategy']
            )
        }

        if 'denomTriggerPaths' in self.specs_:
            ffsc['data-spec']['denomPaths'] = self.specs_['denomTriggerPaths']

        for ds, filelist in zip(self.ffdataset_['datasetNames'], self.ffdataset_['fileList']):
            ## set up job work dir ##
            primarydatasetname = get_primaryDatasetName(ds)
            nametag = get_nametag(ds)
            jobdir = join(self.specs_['workArea'], primarydatasetname, nametag)
            try:
                os.makedirs(jobdir)
            except:
                pass

            ## update ffSuerConfig ##
            datatype = get_datasetType(ds)
            ffsc['data-spec']['dataType'] = datatype
            if datatype=='sigmc' and ds.endswith('USER'): self.specs_['redirector']='root://cmseos.fnal.gov/'

            ## split filtlist into job units ##
            splitunit = self.specs_['unitsPerJob']
            splittedjobs = [filelist[i:i + splitunit] for i in range(0, len(filelist), splitunit)]

            ## set up condor jdl ##
            condorjdlFn = join(jobdir, 'condor.jdl')
            condorjdl = CONDORJDL.replace('FFSUPERCONFIGDIR', jobdir)\
                                 .replace('CMSSWBASENAME', join(dirname(abspath(__file__)), basename(os.getenv('CMSSW_BASE'))) )\
                                 .replace('JOBIDLIST', str(tuple(range(len(splittedjobs)))) )
            with open(condorjdlFn, 'w') as outf: outf.write(condorjdl)

            ## set up executable script ##
            execshellFn = join(jobdir, 'ffCondor.sh')
            outputdir = join(self.specs_['outLFNDirBase'], primarydatasetname, nametag, time.strftime("%y%m%d_%H%M%S"))
            execshell = EXECSHELL.replace('CMSSWBASENAME', basename(os.getenv('CMSSW_BASE')))\
                                 .replace('FFCONFIGNAME', self.specs_.get('ffConfigName', 'ffNtupleFromAOD_v2_cfg.py'))\
                                 .replace('OUTPUTBASE', outputdir)
            with open(execshellFn, 'w') as outf: outf.write(execshell)
            os.chmod(execshellFn, os.stat(execshellFn).st_mode | 0111)

            for i, jf in enumerate(splittedjobs):
                ## finalize ffSuperConfig for each jobs ##
                ffscFn = join(jobdir, 'ffSuperConfig_{0}.yml'.format(i))
                ffsc['data-spec']['inputFileList'] = [self.specs_['redirector'] + f for f in jf]
                ffsc['data-spec']['outputFileName'] = self.specs_['outputFileName'].split('.')[0] + '_{0}.root'.format(i)
                ffsc['data-spec']['skimFileName'] = self.specs_['skimFileName'].split('.')[0] + '_{0}.root'.format(i)
                with open(ffscFn, 'w') as outf:
                    outf.write(yaml.dump(ffsc, default_flow_style=False))

            res.append(condorjdlFn)

        return res

    @staticmethod
    def submit(condorjdl):
        print("$condor_submit", condorjdl)
        os.system("condor_submit {0}".format(condorjdl))
