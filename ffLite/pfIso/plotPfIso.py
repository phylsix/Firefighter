#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import math
import ROOT
from Firefighter.ffConfig.dataSample import ffSamples

import matplotlib.pyplot as plt
plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"
plt.rcParams["axes.titleweight"] = "semibold"
plt.rcParams["font.family"] = ["Ubuntu", "sans-serif"]


ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
try:
    fn = ffSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(ffSamples.keys())
    )
    sys.exit(msg)

def main():
    f = ROOT.TFile.Open(fn)

    dName = f.GetListOfKeys()[0].GetName()
    tName = f.Get(dName).GetListOfKeys()[0].GetName()
    t = f.Get("{}/{}".format(dName, tName))

    pfiso05 = []

    for i, event in enumerate(t, 1):

        pfiso05.extend(event.pfjet_pfIsolation05)

    fig, ax = plt.subplots(figsize=(8, 6))

    ax.hist(pfiso05, bins=50, range=[0, 1], histtype='step', normed=True, log=False)
    ax.set_xlabel('pfIso', x=1.0, ha='right')
    ax.set_ylabel('A.U.', y=1.0, ha='right')
    ax.grid()
    ax.set_title('[{}] leptonJet pfIsolation'.format(dataType), x=0., ha='left')

    outfn = 'pfiso_{}.png'.format(dataType)
    fig.savefig(outfn)


if __name__ == "__main__":
    main()
