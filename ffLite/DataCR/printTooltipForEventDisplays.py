#!/usr/bin/env python
"""
print tooltip for event displays, inc:
- each leptonjet pT, eta, phi, energy, mass
- each leptonjet source type, pT, eta, phi, energy
"""
from __future__ import print_function
import argparse
import os

import pandas as pd
import Firefighter.ffLite.utils as fu

parser = argparse.ArgumentParser(description="Display events tooltip")
parser.add_argument("--input", "-i", type=str, help='input ntuple ROOT file')
parser.add_argument("--outdir", "-o", type=str, help='output directory to store plots')
parser.add_argument("--maxsize", '-n', type=int, default=-1, help="max size of event to process, default -1 means all")
args = parser.parse_args()

import ROOT
ROOT.gROOT.SetBatch()

if __name__ == "__main__":
    assert(os.path.isfile(args.input))
    if not os.path.isdir(args.outdir):
        print('{} does not exist, making one for you'.format(args.outdir))
        os.makedirs(args.outdir)

    PTYPES = {
        2: 'electron',
        3: 'pfmu',
        4: 'photon',
        8: 'dsamu',
    }

    try:
        f = ROOT.TFile.Open(args.input)
        t = f.Get("ffNtuplizer/ffNtuple")
    except:
        print("cannot get tree `ffNtuplizer/ffNtuple` from {}!".format(args.input))
        raise

    for i, event in enumerate(t, 1):
        if args.maxsize!=-1 and i>args.maxsize: break
        outname = '{}-{}-{}.txt'.format(event.run, event.lumi, event.event)
        outname = os.path.join(args.outdir, outname)

        leptonjetPool = []
        for lj in event.pfjet_p4:
            leptonjetPool.append(dict(
                pt=lj.pt(),
                eta=lj.eta(),
                phi=lj.phi(),
                energy=lj.energy(),
                mass=lj.mass(),
            ))
        print('## leptonjet')
        df_lj = pd.DataFrame(leptonjetPool)
        df_lj = df_lj[['energy', 'pt', 'eta', 'phi', 'mass']]
        print(df_lj.to_string())

        if len(leptonjetPool)>=2:
            ljp4s = [j for j in event.pfjet_p4]
            lj0, lj1 = ljp4s[0], ljp4s[1]
            leptonjetPair = [dict(
                deta=abs(lj0.eta()-lj1.eta()),
                dphi=abs(fu.delta_phi(lj0, lj1)),
                dr=fu.delta_r(lj0, lj1),
                invm=(lj0+lj1).mass(),
            )]
            df_ljp = pd.DataFrame(leptonjetPair)
            print('## leptonjet pair (leading/subleading)')
            print(df_ljp.to_string(index=False))

        leptonjetSourcePool = []
        for p, t in zip(event.ljsource_p4, event.ljsource_type):
            leptonjetSourcePool.append(dict(
                type=PTYPES[t],
                pt=p.pt(),
                eta=p.eta(),
                phi=p.phi(),
                energy=p.energy(),
            ))
        print('## leptonjet sources')
        df_ljs = pd.DataFrame(leptonjetSourcePool)
        df_ljs = df_ljs[['type', 'energy', 'pt', 'eta', 'phi']]
        print(df_ljs.to_string())

        print('\n')

        with open(outname, 'w') as outf:
            outf.write('[leptonjet]\n')
            outf.write(df_lj.to_string())
            outf.write('\n\n')
            if len(leptonjetPool)>=2:
                outf.write('[leptonjet pair]\n')
                outf.write(df_ljp.to_string(index=False))
                outf.write('\n\n')

            outf.write('[leptonjet source]\n')
            outf.write(df_ljs.to_string())
