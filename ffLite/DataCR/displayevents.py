#!/usr/bin/env python
"""draw energy scatter plots on eta-phi plane from skimmed file, inc.
- PFCandidates
- leptonjets

$ python displayevents.py -i <skimmedFile> -o <outdir>
"""
from __future__ import print_function

import argparse
import os
import sys
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.utils import colors, pType

ROOT.gROOT.SetBatch()
plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"


parser = argparse.ArgumentParser(description="Display events in eta-phi map")
parser.add_argument("--input", "-i", type=str)
parser.add_argument("--outdir", "-o", type=str)
args = parser.parse_args()

def makeplot(f, dirname):

    events = Events(f)
    for i, event in enumerate(events, 1):
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        eventId = '{}-{}-{}'.format(_run, _lumi, _event)

        # pfcandsHdl = Handle("vector<reco::PFCandidate>")
        # event.getByLabel(("particleFlow", "", "RECO"), pfcandsHdl)
        pfcandsHdl = Handle("vector<edm::FwdPtr<reco::PFCandidate> >")
        event.getByLabel(("pfNoPileUpIso", "", "FFNTP"), pfcandsHdl)

        assert(pfcandsHdl.isValid())
        leptonjetsHdl = Handle("vector<reco::PFJet>")
        event.getByLabel(("filteredLeptonJet", "", "FFNTP"), leptonjetsHdl)
        assert(leptonjetsHdl.isValid())
        if len(leptonjetsHdl.product())==0:
            continue

        ljsourceHdls = {
            'pfmuon': Handle("vector<edm::FwdPtr<reco::PFCandidate> >"),
            'pfelectron': Handle("vector<edm::FwdPtr<reco::PFCandidate> >"),
            'pfphoton': Handle("vector<edm::FwdPtr<reco::PFCandidate> >"),
            'dsamuon': Handle("vector<edm::FwdPtr<reco::PFCandidate> >"),
        }
        event.getByLabel(("leptonjetSourcePFMuon", "inclusive", "FFNTP"), ljsourceHdls['pfmuon'])
        event.getByLabel(("leptonjetSourcePFElectron", "inclusive", "FFNTP"), ljsourceHdls['pfelectron'])
        event.getByLabel(("leptonjetSourcePFPhoton", "", "FFNTP"), ljsourceHdls['pfphoton'])
        event.getByLabel(("leptonjetSourceDSAMuon", "inclusive", "FFNTP"), ljsourceHdls['dsamuon'])
        for v in ljsourceHdls.values():
            assert(v.isValid())

        pfcands = pfcandsHdl.product()
        pfcandsData = defaultdict(list)
        for c in pfcands:
            pfcandsData['eta'].append(c.eta())
            pfcandsData['phi'].append(c.phi())
            pfcandsData['energy'].append(c.energy())
            pfcandsData['color'].append(colors[int(c.particleId())])
        pfcandsdf = pd.DataFrame(pfcandsData)

        ljsourcesData = defaultdict(list)
        for c in ljsourceHdls['pfmuon'].product():
            ljsourcesData['eta'].append(c.eta())
            ljsourcesData['phi'].append(c.phi())
            ljsourcesData['energy'].append(c.energy())
            ljsourcesData['color'].append(colors[3])
        for c in ljsourceHdls['pfelectron'].product():
            ljsourcesData['eta'].append(c.eta())
            ljsourcesData['phi'].append(c.phi())
            ljsourcesData['energy'].append(c.energy())
            ljsourcesData['color'].append(colors[2])
        for c in ljsourceHdls['pfphoton'].product():
            ljsourcesData['eta'].append(c.eta())
            ljsourcesData['phi'].append(c.phi())
            ljsourcesData['energy'].append(c.energy())
            ljsourcesData['color'].append(colors[4])
        for c in ljsourceHdls['dsamuon'].product():
            ljsourcesData['eta'].append(c.eta())
            ljsourcesData['phi'].append(c.phi())
            ljsourcesData['energy'].append(c.energy())
            ljsourcesData['color'].append(colors[8])
        ljsrcdf = pd.DataFrame(ljsourcesData)

        leptonjets = leptonjetsHdl.product()
        leptonjetsData = defaultdict(list)
        for j in leptonjets:
            leptonjetsData['eta'].append(j.eta())
            leptonjetsData['phi'].append(j.phi())
            leptonjetsData['energy'].append(j.energy())
        ljdf = pd.DataFrame(leptonjetsData)

        ## plot
        fig, axes = plt.subplots(1,2,figsize=(8*2,6*1), gridspec_kw=dict(wspace=0.1, hspace=0.15))

        if not ljsrcdf.empty:
            ljsrcdf.plot.scatter(x='eta', y='phi', s=ljsrcdf['energy']*50, c=ljsrcdf['color'], ax=axes[0], alpha=0.8)
        if not ljdf.empty:
            ljdf.plot.scatter(x='eta', y='phi', s=ljdf['energy']*50, c="None", ax=axes[0], edgecolors='k', linewidth=2, linestyle='--')
        if not pfcandsdf.empty:
            pfcandsdf.plot.scatter(x='eta', y='phi', s=pfcandsdf['energy']*50, c=pfcandsdf['color'], ax=axes[1], alpha=0.8)
        for ax in axes.flat:
            ax.set_xlim([-2.5, 2.5])
            ax.set_ylim([-np.pi, np.pi])
            ax.set_xlabel("eta", x=1.0, ha="right")
            ax.set_ylabel("phi", y=1.0, ha="right")
            ax.text(1,1,eventId, ha='right', va='bottom', fontsize=9, transform=ax.transAxes,)
        axes[0].set_title('leptonJets+source energy map', x=0, ha='left')
        axes[1].set_title('PFCandidates energy map', x=0, ha='left')

        ## save
        outfilename = '{}/{}.png'.format(dirname, eventId)
        fig.savefig(outfilename)
        plt.close()


if __name__ == "__main__":

    assert(os.path.isfile(args.input))
    if not os.path.isdir(args.outdir):
        print("output dir *{}* not exist, I'm going to make one for you.".format(args.outdir))
        os.makedirs(args.outdir)
    makeplot(args.input, args.outdir)
    print(args.input, '-->done')
