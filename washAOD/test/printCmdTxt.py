#!/usr/bin/env python2.7
from __future__ import print_function
import os
import subprocess
import argparse

year = '2017'
frag = 'tuplizer' #'jetTuplizer'
# suffixTag = '100k' # [100k, Pythia, PythiaTest2]
prefixTag = 'SIDM_BsTo2DpTo4Mu'
grepKeyword = 'Begin Fatal' # used to grep logs


parser = argparse.ArgumentParser(description='Print out cmds to run.')
parser.add_argument('-a', '--startAll', action="store_true", default=False)
args = parser.parse_args()

def make_exited_list(grepword, year):
    """ return a list of exited jobs through grepping logs """

    grepcmd = 'grep -rl "{0}" Logs/{1}'.format(grepword, year)
    try:
        shelloutput = subprocess.check_output(grepcmd, shell=True)
        rawlist = shelloutput.strip().split('\n')
        cleanlist = [os.path.basename(x.strip()) for x in rawlist]
        tagremovedlist = [x.replace('.log', '').split('_', 1)[1] for x in cleanlist]
    except subprocess.CalledProcessError:
        print('Yay! There was no jobs finished abnormally!')
        tagremovedlist = []

    return tagremovedlist

def make_datalink_list(prefix):
    return [f for f in os.listdir('../data/'+year) if f.startswith(prefix)]

def lookup_file(tag, filelist):
    matched = [f for f in filelist if tag in f]
    if len(matched):
        return matched[0]
    else:
        return ''

def lookup_files(tolookup, pool):
    if len(tolookup):
        matched = []
        for tl in tolookup:
            result = lookup_file(tl, pool)
            if result:
                matched.append(result)
            else:
                print("No file found matched with: '"+tl+"'.")
        return matched
    else:
        return pool

def main():

    print()
    print("#"*79)
    print("YEAR = ", year)
    print("FRAG = ", '%s_cfg.py'%frag)
    print("#"*79)
    print()

    #ctaus = ['1p20e-03', '0p012', '0p12', '0p6', '1p2', '3p6']
    #for c in ctaus:
    #    datalistF = 'SIDMmumu_Mps-200_MZp-1p2_ctau-{0}'.format(c)
    #    if suffixTag:
    #        datalistF = '_'.join([datalistF, suffixTag])
    #    if not os.path.isfile('../data/{0}/{1}.list'.format(year, datalistF)): continue
    #    print("nohup ./mkNtuple.sh {0} {1} {2} &".format(datalistF, frag, year))

    allList = make_datalink_list(prefixTag)

    if args.startAll:
        print('Printing all datalinks >>>\n')
        datalinkFilelist = lookup_files([], allList)
    else:
        exitedList = make_exited_list(grepKeyword, year)
        if len(exitedList):
            datalinkFilelist = lookup_files(exitedList, allList)
        else:
            datalinkFilelist = []

    for f in datalinkFilelist:
        datalistF = f.split('.')[0]
        print("nohup ./mkNtuple.sh {0} {1} {2} &".format(datalistF, frag, year))

    print()
    print('-'*79)
    print('Total: ', len(datalinkFilelist), ' datalinks.')
    print('-'*79)

if __name__ == '__main__':
    main()
