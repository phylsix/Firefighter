#!/usr/bin/env python
"""Given a condor submission directory in condorGarage, check the stderr file,
reubmit the ones with **Begin Fatal Exception**.
"""
from __future__ import print_function
import subprocess
import shlex
import os
import sys
from os.path import basename, join
from Firefighter.piedpiper.utils import get_voms_certificate


# replaced vars: FFSUPERCONFIGDIR, CMSSWVER, JOBIDLIST
CONDORRESUBJDL = """\
universe = vanilla
+REQUIRED_OS = "rhel6"
+DesiredOS = REQUIRED_OS
Executable = FFSUPERCONFIGDIR/ffCondorResub.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Transfer_Input_Files = FFSUPERCONFIGDIR/ffCondorResub.sh, FFSUPERCONFIGDIR/ffSuperConfig_$(jobid).yml, CMSSWVER.tar.gz
Output = FFSUPERCONFIGDIR/ffCondor_$(jobid).stdout
Error = FFSUPERCONFIGDIR/ffCondor_$(jobid).stderr
Log = FFSUPERCONFIGDIR/ffCondor_$(jobid).log
x509userproxy = $ENV(X509_USER_PROXY)
Arguments = FFSUPERCONFIGDIR/ffSuperConfig_$(jobid).yml
want_graceful_removal = true
on_exit_remove = (ExitBySignal == False) && (ExitCode == 0)
on_exit_hold = ( (ExitBySignal == True) || (ExitCode != 0) )
on_exit_hold_reason = strcat("Job held by ON_EXIT_HOLD due to ", ifThenElse((ExitBySignal == True), "exit by signal", strcat("exit code ",ExitCode)), ".")
queue 1 jobid in JOBIDLIST
"""


# replaced vars; CMSSWVER, OUTPUTBASE
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
YMLCFG=`basename $1`
echo "Argument ffSuperConfig is: $YMLCFG"
cmsRun $CMSSW_BASE/src/Firefighter/ffConfig/cfg/ffNtupleFromAOD_cfg.py config=$CWD/$YMLCFG

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


def getErrorJobId(jobpath):
    """Given a jobdir which store the output of condor submission, find the job
    ids w/ error keyword
    """

    cmd = 'grep -l "Begin Fatal Exception" {0}/*.stderr'.format(jobpath)
    errfilelist = subprocess.check_output(cmd, shell=True).split()
    errjobids = [int(basename(p).split('.')[0].split('_')[-1]) for p in errfilelist]
    return sorted(errjobids)

def renameSuperconfigByIds(jobpath, ids):
    """Given a list of ``ids``, rename the ffSuperconfig from
       ffSuperConfig_XXXXX.yml -> ffSuperConfig_INT.yml (remove the zero pad).
    """

    for jobid in ids:
        cmd = "mv {0}/ffSuperConfig_{1:05d}.yml {0}/ffSuperConfig_{1}.yml".format(jobpath, jobid)
        subprocess.check_call(shlex.split(cmd))


def rebuildExecFile(jobpath):
    subexecfile = join(jobpath, 'ffCondor.sh')
    resubexecfile = join(jobpath, 'ffCondorResub.sh')

    ## get outputbase
    outdirline = [l for l in open(subexecfile).readlines() if l.startswith('OUTDIR')]
    if not outdirline:
        print(open(subexecfile).read())
        sys.exit('Cannot find line startswith "OUTDIR", to locate OUTPUTBASE')
    outdirline = outdirline[0].strip()
    outputbase = outdirline.split('=')[-1].replace('root://cmseos.fnal.gov/', '')

    ## write resubmit exec file
    resubexec_ = EXECSHELL.replace('CMSSWVER', os.getenv('CMSSW_VERSION')).replace('OUTPUTBASE', outputbase)
    # print(resubexec_)
    with open(resubexecfile, 'w') as outf:
        outf.write(resubexec_)
    os.chmod(resubexecfile, os.stat(resubexecfile).st_mode | 0111)


def rebuildCondorJdl(jobpath, jobids):
    """Output a resubmit `condorResub.jdl` to ``jobpath``.
        ``jobids``: list of int
    """
    resubjdlfile = join(jobpath, 'ffCondorResub.jdl')
    resubjdl_ = CONDORRESUBJDL.replace('CMSSWVER', os.getenv('CMSSW_VERSION'))\
                              .replace('FFSUPERCONFIGDIR', jobpath)\
                              .replace('JOBIDLIST', str(tuple(jobids)) )
    # print(resubjdl_)
    with open(resubjdlfile, 'w') as outf:
        outf.write(resubjdl_)
    return resubjdlfile



if __name__ == "__main__":

    JOBDIR = '/uscms/home/wsi/nobackup/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/condorGarage/190703/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1'
    # JOBDIR = '/uscms/home/wsi/nobackup/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/condorGarage/190701/DoubleMuon/Run2018A-17Sep2018-v2'

    ## get error job ids
    errorjobids = getErrorJobId(JOBDIR)

    ## rename ffSuperConfig (temporately)
    # renameSuperconfigByIds(JOBDIR, errorjobids)

    # ## write resubmit exec file and condor jdl
    rebuildExecFile(JOBDIR)
    resubjdl = rebuildCondorJdl(JOBDIR, errorjobids)

    # ## resubmit!
    os.system("tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}")
    get_voms_certificate()
    print("$condor_submit", resubjdl)
    os.system("condor_submit {0}".format(resubjdl))
