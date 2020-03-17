#!/usr/bin/env python
"""Run GenXSecAnalyzer through condor

$ python condorXSecSubmitter.py <description.yml>
"""
from __future__ import print_function

import importlib
import os
import sys
import time
from os.path import join, basename

import yaml
from Firefighter.piedpiper.utils import get_voms_certificate

# replaced vars: FFSUPERCONFIGDIR, CMSSWVER, JOBIDLIST
CONDORJDL = """\
universe = vanilla
+REQUIRED_OS = "rhel6"
+DesiredOS = REQUIRED_OS
Executable = FFSUPERCONFIGDIR/ffCondor.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Transfer_Input_Files = FFSUPERCONFIGDIR/ffCondor.sh, FFSUPERCONFIGDIR/$(jobid).list, CMSSWVER.tar.gz
Output = FFSUPERCONFIGDIR/$(jobid).stdout
Error = FFSUPERCONFIGDIR/$(jobid).stderr
Log = FFSUPERCONFIGDIR/$(jobid).log
x509userproxy = $ENV(X509_USER_PROXY)
Arguments = FFSUPERCONFIGDIR/$(jobid).list
want_graceful_removal = true
on_exit_remove = (ExitBySignal == False) && (ExitCode == 0)
on_exit_hold = ( (ExitBySignal == True) || (ExitCode != 0) )
on_exit_hold_reason = strcat("Job held by ON_EXIT_HOLD due to ", ifThenElse((ExitBySignal == True), "exit by signal", strcat("exit code ",ExitCode)), ".")
queue 1 jobid in JOBIDLIST"""


# replaced vars; CMSSWVER
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
tar xzf CMSSWVER.tar.gz
cd CMSSWVER/src/
echo ">> scram b ProjectRename"
scram b ProjectRename
eval `scramv1 runtime -sh`
echo "CMSSW: "$CMSSW_BASE
FILELIST=`basename $1`
cmsRun $CMSSW_BASE/src/Firefighter/ffConfig/cfg/GenXSecAna_cfg.py  inputFiles_load=$CWD/$FILELIST maxEvents=-1 2>&1

CMSEXIT=$?
exit $CMSEXIT"""




if __name__ == "__main__":

    datasetdescription = sys.argv[1]
    assert os.path.exists(datasetdescription)
    datasetdescription = yaml.load(open(datasetdescription), Loader=yaml.Loader)

    jobdir = join(
        os.getenv("CMSSW_BASE"),
        "src/Firefighter/ffConfig/condorGarage/",
        time.strftime("%y%m%d"),
        "GenXsecAna",
    )
    if not os.path.isdir(jobdir):
        os.makedirs(jobdir)


    # input file lists
    datasettags = []
    for dataset in datasetdescription:
        datasettag = basename(dataset).split('.')[0]
        datasettags.append(datasettag)
        datasetconfig = yaml.load(open(join(os.getenv('CMSSW_BASE'), dataset)), Loader=yaml.Loader)
        datafilelist = [f for fl in datasetconfig["fileList"] for f in fl]

        ## write inputfileList
        with open(join(jobdir, "{}.list".format(datasettag)), "w") as outf:
            if "sigmc" in dataset and "private" in dataset:
                outf.write("\n".join(["root://cmseos.fnal.gov/" + f for f in datafilelist]))
            else:
                outf.write("\n".join(datafilelist))


    ## set up condor jdl
    condorjdlFn = join(jobdir, "condor.jdl")
    condorjdl = CONDORJDL.replace('FFSUPERCONFIGDIR', jobdir)\
                         .replace('CMSSWVER', basename(os.getenv('CMSSW_BASE')))\
                         .replace('JOBIDLIST', ', '.join(datasettags))
    with open(condorjdlFn, 'w') as outf: outf.write(condorjdl)


    ## set up executable script
    execshellFn = join(jobdir, 'ffCondor.sh')
    execshell = EXECSHELL.replace('CMSSWVER', basename(os.getenv('CMSSW_BASE')))
    with open(execshellFn, 'w') as outf: outf.write(execshell)
    os.chmod(execshellFn, os.stat(execshellFn).st_mode | 0111)


    # tar cmssw; voms certificate
    os.system("tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`")
    get_voms_certificate()


    ## submit
    print("$condor_submit", condorjdlFn)
    os.system("condor_submit {0}".format(condorjdlFn))
