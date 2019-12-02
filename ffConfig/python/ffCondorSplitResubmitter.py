#!/usr/bin/env python
"""split continueously failed condor job for each file,
when one of the files is problematic.

$ python ffCondorSplitResubmitter.py -p 191127/DoubleMuon/Run2018C-17Sep2018-v1 -j 82 304
"""
from __future__ import print_function

import argparse
from copy import deepcopy
import os
import shutil
from os.path import join

import yaml

## parser
parser = argparse.ArgumentParser(description="Resubmit failed condor job in a file-split way.")
parser.add_argument("--jobpath", '-p', type=str, help='relative path job path wrt `condorGarage`.')
parser.add_argument("--jobids", '-j', type=str, nargs='*', help='failed job id(s)')
args = parser.parse_args()


if __name__ == "__main__":
    fulljobpath = join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/condorGarage', args.jobpath)
    assert(os.path.isdir(fulljobpath))
    ffconfigs = [join(fulljobpath, 'ffSuperConfig_{}.yml'.format(i)) for i in args.jobids]
    assert(all([os.path.isfile(f) for f in ffconfigs]))

    orig_condorjdl = join(fulljobpath, 'condor.jdl')
    orig_ffcondorsh = join(fulljobpath, 'ffCondor.sh')
    assert(os.path.isfile(orig_condorjdl))
    assert(os.path.isfile(orig_ffcondorsh))

    jdlpool = []
    for jid, ffconfig in zip(args.jobids, ffconfigs):
        newjobpath = join(fulljobpath, jid)
        new_condorjdl = join(newjobpath, 'condor.jdl')
        new_ffcondorsh  = join(newjobpath, 'ffCondor.sh')

        ## make subfolder
        try:
            os.makedirs(newjobpath)
        except:
            pass

        ## ffCondor.sh
        shutil.copyfile(orig_ffcondorsh, new_ffcondorsh)
        print('--> ffCondor.sh copied to: {}'.format(new_ffcondorsh))

        ## ffSuperconfig_{i}.yml
        _ffconfig = yaml.load(open(ffconfig), Loader=yaml.Loader)
        inputfiles = _ffconfig['data-spec']['inputFileList']
        njobs_split = len(inputfiles)
        print('--> split {} jobs for: {}'.format(njobs_split, ffconfig))
        for i, f in enumerate(inputfiles):
            i_ffconfig = deepcopy(_ffconfig)
            i_ffconfig['data-spec']['inputFileList'] = [f,]
            i_ffconfig['data-spec']['outputFileName'] = '{}_{}.root'.format(_ffconfig['data-spec']['outputFileName'].split('.')[0], i)
            i_ffconfig['data-spec']['skimFileName'] = '{}_{}.root'.format(_ffconfig['data-spec']['skimFileName'].split('.')[0], i)
            with open(join(newjobpath, 'ffSuperConfig_{}.yml'.format(i)), 'w') as outf:
                outf.write(yaml.dump(i_ffconfig, default_flow_style=False))
            print('++ {}'.format(join(newjobpath, 'ffSuperConfig_{}.yml'.format(i))))

        ## condor.jdl
        with open(new_condorjdl, 'w') as outf:
            for line in open(orig_condorjdl).readlines():
                if '=' in line:
                    outf.write(line.replace(fulljobpath, newjobpath))
            outf.write('queue 1 jobid in {}'.format(str(tuple(range(njobs_split)))))
        print('--> {}'.format(new_condorjdl))
        jdlpool.append(new_condorjdl)

    ## print condor submit cmd
    print(' condor_submit '.center(80, '_'))
    for j in jdlpool:
        print('condor_submit {}'.format(j))
