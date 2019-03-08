#!/usr/bin/env python
from __future__ import print_function
import os
import sys
from collections import defaultdict

import pandas as pd
import matplotlib.pyplot as plt
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.dataSample import ffSamples
from Firefighter.ffLite.utils import colors, pType
import Firefighter.ffLite.utils as fu

import ROOT
ROOT.gROOT.SetBatch()

plt.style.use('default')
plt.rcParams['grid.linestyle'] = ':'
plt.rcParams['savefig.dpi'] = 120
plt.rcParams['savefig.bbox'] = 'tight'

dataType = sys.argv[1]
try:
    fn = ffSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(ffSamples.keys()))
    sys.exit(msg)

outdir = os.path.join(os.path.dirname(
    os.path.abspath(__file__)), 'PFEnergy', dataType)
if not os.path.exists(outdir):
    os.makedirs(outdir)


def main():

    f = ROOT.TFile.Open(fn)
    dName = f.GetListOfKeys()[0].GetName()
    tName = f.Get(dName).GetListOfKeys()[0].GetName()
    t = f.Get('{}/{}'.format(dName, tName))

    bNames = [b.GetName() for b in t.GetListOfBranches()]

    sigMC = any([b.startswith('gen') for b in bNames])
    nonSig = not sigMC

    wentThroughEvents = 0
    for i, event in enumerate(t, 1):

        if wentThroughEvents > 10:
            break

        if sigMC:
            darkphotons = []
            for pid, dp in zip(list(event.gen2_pid), list(event.gen2_p4)):
                if abs(pid) != 13:
                    continue
                darkphotons.append((dp.eta(), dp.phi(), dp.energy()))
            if not all(map(lambda p: abs(p[0]) < 2.4, darkphotons)):
                continue
            desc_ = '({:.3f}, {:.3f}) pT: {:.3f} GeV'
            desc = '\n'.join(['darkphotons']+[desc_.format(*dp)
                                              for dp in darkphotons])
            print(desc)

        candsData = defaultdict(list)
        candsData['type'] = [
            x for j in event.pfjet_pfcand_type for x in list(j)]
        candsData['energy'] = [
            x for j in event.pfjet_pfcand_energy for x in list(j)]
        candsData['eta'] = [x for j in event.pfjet_pfcand_eta for x in list(j)]
        candsData['phi'] = [x for j in event.pfjet_pfcand_phi for x in list(j)]
        candsData['color'] = [colors[t] for t in candsData['type']]

        if not candsData['type']:
            continue

        candsDf = pd.DataFrame(candsData)
        fig, ax = plt.subplots(figsize=(8, 6))

        ax = candsDf.plot.scatter(
            x='eta', y='phi', s=candsDf['energy'] * 50, c=candsDf['color'],
            ax=ax, alpha=0.8
        )
        ax.set_xlim([-2.4, 2.4])
        ax.set_ylim([-3.142, 3.142])
        ax.grid()
        ax.set_title('Energy spread of PFCandidates: event {}'.format(i))

        if sigMC:
            bprops = dict(facecolor='w', alpha=0.75)
            ax.text(0, 0, desc, ha='left', va='bottom',
                    transform=ax.transAxes, fontsize=9, bbox=bprops)
            ax.scatter(
                [d[0] for d in darkphotons],
                [d[1] for d in darkphotons],
                c='k', marker='D', s=100)

        for ic, color in enumerate(colors[:9]):
            ax.scatter([], [], s=100, c=color, label=pType[ic])
            ax.legend(scatterpoints=1, title='type',
                      fontsize=9, framealpha=0.75, labelspacing=0.2)

        outfn = os.path.join(outdir, 'ffNtuple_{}.png'.format(i))
        fig.savefig(outfn)

        wentThroughEvents += 1


if __name__ == "__main__":
    main()
