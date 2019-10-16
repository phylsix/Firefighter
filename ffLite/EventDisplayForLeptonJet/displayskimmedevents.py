#!/usr/bin/env python
"""
Draw energy scatter plots on eta-phi plane, inc.
left:
- pfcandidates
right:
- leptonjet sources
- leptonjets
"""

from __future__ import print_function
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import os
import sys
from collections import defaultdict
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from Firefighter.ffLite.utils import colors, pType

ROOT.gROOT.SetBatch()
plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"

def makeplot(ntuplefile, outputdir):

    f = ROOT.TFile.Open(ntuplefile)
    t = f.Get("ffNtuplizer/ffNtuple")

    for i, event in enumerate(t, 1):

        ## data
        eventId = '{}-{}-{}'.format(int(event.run), int(event.lumi), int(event.event))

        leptonjetSrc = {}
        leptonjetSrc['type'] = list(event.ljsource_type)
        leptonjetSrc['energy'] = [p.energy() for p in list(event.ljsource_p4)]
        leptonjetSrc['eta'] = [float(p.eta()) for p in list(event.ljsource_p4)]
        leptonjetSrc['phi'] = [float(p.phi()) for p in list(event.ljsource_p4)]
        leptonjetSrc['color'] = [colors[t] for t in leptonjetSrc['type']]

        leptonjet = {}
        leptonjet['energy'] = [p.energy() for p in list(event.pfjet_p4)]
        leptonjet['eta'] = [float(p.eta()) for p in list(event.pfjet_p4)]
        leptonjet['phi'] = [float(p.phi()) for p in list(event.pfjet_p4)]

        pfcands = {}
        pfcands['type'] = list(event.allpfcand_pid)
        pfcands['energy'] = [p.energy() for p in list(event.allpfcand_p4)]
        pfcands['eta'] = [float(p.eta()) for p in list(event.allpfcand_p4)]
        pfcands['phi'] = [float(p.phi()) for p in list(event.allpfcand_p4)]
        pfcands['color'] = [colors[t] for t in pfcands['type']]

        ## plot
        fig, axes = plt.subplots(1,2,figsize=(8*2,6*1), gridspec_kw=dict(wspace=0.1, hspace=0.15))

        ljsrcdf = pd.DataFrame(leptonjetSrc)
        if not ljsrcdf.empty:
            ljsrcdf.plot.scatter(x='eta', y='phi', s=ljsrcdf['energy']*50, c=ljsrcdf['color'], ax=axes[0], alpha=0.8)
        ljdf = pd.DataFrame(leptonjet)
        if not ljdf.empty:
            ljdf.plot.scatter(x='eta', y='phi', s=ljdf['energy']*50, c="None", ax=axes[0], edgecolors='k', linewidth=2, linestyle='--')
        pfcandsdf = pd.DataFrame(pfcands)
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
        outfilename = '{}/{}.png'.format(outputdir, eventId)
        fig.savefig(outfilename)
        plt.close()


if __name__ == "__main__":

    for dirname in os.listdir('skimmedevents/SkimmedBkgMC'):
        f = os.path.join('skimmedevents/SkimmedBkgMC', dirname, 'ffNtuple.root')
        d = os.path.join('skimmedevents/SkimmedBkgMC', dirname, 'img')
        if not os.path.isdir(d):
            os.makedirs(d)
        makeplot(f, d)
        print(f, '-->done')

    # skimmedDataDirBase = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffLite/DataCR'
    # f = os.path.join(skimmedDataDirBase, 'ffNtuple_DoubleMuon2018C_CR.root')
    # d = os.path.join(skimmedDataDirBase, 'img', 'C')
    # if not os.path.isdir(d):
    #     os.makedirs(d)
    # makeplot(f, d)
    # print(f, '-->done')
    # f = os.path.join(skimmedDataDirBase, 'ffNtuple_DoubleMuon2018D_CR.root')
    # d = os.path.join(skimmedDataDirBase, 'img', 'D')
    # if not os.path.isdir(d):
    #     os.makedirs(d)
    # makeplot(f, d)
    # print(f, '-->done')