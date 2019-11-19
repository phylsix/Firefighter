#!/usr/bin/env python
"""pick events from a list of files"""
from __future__ import print_function

import argparse
import json
import os
import sys
from os.path import join, splitext, basename

import yaml

parser = argparse.ArgumentParser(description="print edmPickMerge cmd for skim data")
parser.add_argument("-i", "--input", help="text file contains `run:lumi:event`")
args = parser.parse_args()

assert os.path.exists(args.input)

def make_run_event_input(runlumieventlistfile, runeventoutname):
    """
    dump file containing run:lumi:event into a file containing run:event

    :param str runlumieventlistfile: file name which having run:lumi:event per line
    :param str runeventoutname: output file name which will have run:event per line
    :return: none
    """
    runlumievents = open(runlumieventlistfile).read().split('\n')
    runlumievents = [l for l in runlumievents if l.count(':')==2]
    runevents = []
    for l in runlumievents:
        run, lumi, event = l.split(':')
        runevents.append('{}:{}'.format(run, event))
    with open(runeventoutname, 'w') as outf:
        outf.write('\n'.join(runevents))


def make_filelist_input(fileoutname, era):
    """
    dump inputfile list of skimmed data samples to a file.

    :param str fileoutname: output file name
    :param str era: era, 'ABC' or 'D'
    :return: none
    """
    desc = join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/production/Skim2LJ18/data/description.json')
    dataDescList = [join(os.getenv('CMSSW_BASE'), l) for l in json.load(open(desc)) if l.split('.')[0][-1] in era]
    res = []
    for dd in dataDescList:
        dd_ = yaml.load(open(dd), Loader=yaml.Loader)
        res.extend([f for l in dd_['fileList'] for f in l])
    with open(fileoutname, 'w') as outf:
        outf.write('\n'.join(res))


def make_cmd(runeventFilename, inputFilename, outputFilename):
    """make `edmCopyPickMerge` command

    :param str runeventFilename: runevent filename
    :param str inputFilename: input filename
    :param str outputFilename: output filename
    :return: command to execute
    :rtype: str
    """
    return 'edmCopyPickMerge eventsToProcess_load={} inputFiles_load={} outputFile={} &'.format(runeventFilename,
                                                                                                inputFilename,
                                                                                                outputFilename)


if __name__ == "__main__":

    runlumievent_file = args.input
    runlumievent_filebase = basename(runlumievent_file)
    outdir = splitext(runlumievent_filebase)[0]
    try:
        os.makedirs(outdir)
    except:
        pass
    runevent_file = join(outdir, 'runevents.txt')
    make_run_event_input(runlumievent_file, runevent_file)

    ## era ABC
    inputlist_file = join(outdir, 'skimmed_data18CR_ABC.txt')
    outroot_file = join(outdir, 'skimmed_data18CR_ABC_AOD.root')

    make_filelist_input(inputlist_file, 'ABC')
    cmd = make_cmd(runevent_file, inputlist_file, outroot_file)
    print('$', cmd)
    os.system(cmd)

    ## era D
    inputlist_file = join(outdir, 'skimmed_data18CR_D.txt')
    outroot_file = join(outdir, 'skimmed_data18CR_D_AOD.root')

    make_filelist_input(inputlist_file, 'D')
    cmd = make_cmd(runevent_file, inputlist_file, outroot_file)
    print('$', cmd)
    os.system(cmd)
