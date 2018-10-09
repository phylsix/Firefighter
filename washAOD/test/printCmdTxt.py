#!/usr/bin/env python
from __future__ import print_function
import os

year = '2017'
frag = 'tuplizer' #'jetTuplizer'
suffixTag = '100k' # [100k, Pythia, PythiaTest2]
prefixTag = 'SIDM_BsTo2DpTo4Mu'

exited = '''SIDM_BsTo2DpTo4Mu_MBs-200_MDp-0p3_ctau-1p2_15k
    SIDM_BsTo2DpTo4Mu_MBs-40_MDp-1p2_ctau-2p4_15k
    SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-0p3_ctau-0p24_15k
    SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-0p3_ctau-0p024_15k
    SIDM_BsTo2DpTo4Mu_MBs-40_MDp-0p3_ctau-0p6_15k
    SIDM_BsTo2DpTo4Mu_MBs-10_MDp-1p2_ctau-96_15k
    SIDM_BsTo2DpTo4Mu_MBs-10_MDp-0p3_ctau-2p4_15k
    SIDM_BsTo2DpTo4Mu_MBs-200_MDp-0p3_ctau-0p12_15k
    SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48_15k
    SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-1p2_ctau-0p96_15k'''

def make_datalink_list(prefix):
    return [f for f in os.listdir('../data/'+year) if f.startswith(prefix)]

def lookup_file(tag, filelist):
    matched = [f for f in filelist if tag in f]
    if len(matched):
        return matched[0]
    else:
        return ''

def lookup_files(tolookup, pool):
    matched = []
    for tl in tolookup:
        result = lookup_file(tl, pool)
        if result:
            matched.append(result)
        else:
            print("No file found matched with: '"+tl+"'.")
    return matched

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

    exitedList = [e.strip() for e in exited.split('\n')]
    datalinkFilelist = lookup_files(exitedList, allList)
    for f in datalinkFilelist:
        datalistF = f.split('.')[0]
        print("nohup ./mkNtuple.sh {0} {1} {2} &".format(datalistF, frag, year))

    print()
    print('-'*79)
    print('Total: ', len(datalinkFilelist), ' datalinks.')
    print('-'*79)

if __name__ == '__main__':
    main()
