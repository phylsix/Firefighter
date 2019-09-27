#!/usr/bin/env python
"""
Draw energy scatter plots on eta-phi plane, inc.
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


def main():

    fn = sys.argv[1]
    assert(fn.endswith('.root'))
    fns = fn.split('/')
    if 'SIDM' in fns[-4]:
        outputdir_ = '__'.join(fns[-4: -2])
    elif 'SIDM' in fns[-3]:
        outputdir_ = fns[-3]
    else:
        outputdir_ = fns[-4]
    outputdir = 'img/{}'.format(outputdir_)
    try:
        os.makedirs(outputdir)
    except:
        pass
    f = ROOT.TFile.Open(fn)
    t = f.Get("ffNtuplizer/ffNtuple")

    totalEventsInfo = []

    ## loop over events, collect information
    for i, event in enumerate(t, 1):
        eventsummary = {}
        eventId = '{}-{}-{}'.format(int(event.run), int(event.lumi), int(event.event))
        eventsummary['eventId'] = eventId

        ## gen infomation
        XX_mass = None
        A_mass = None
        descfmt = "({:-5.3f}, {:-5.3f}) {:7.2f} GeV {:7.2f} cm"
        darkphotons = []

        if 35 in list(event.gen_pid):
            for pid, p4, dvtx in zip(list(event.gen_pid), list(event.gen_p4), list(event.gen_dauvtx)):
                if not XX_mass and pid==35:
                    XX_mass = p4.mass()
                if not A_mass and pid==32:
                    A_mass = p4.mass()
                if pid==32:
                    darkphotons.append((p4.eta(), p4.phi(), p4.energy(), dvtx.rho()))

            gendesc = "\n".join(
                #["mXX: {:6.1f} GeV, mA: {:4.2f} GeV".format(XX_mass, A_mass), '\n']
                []
                + ["darkphotons [(eta, phi) energy lxy]"]
                + [descfmt.format(*dp) for dp in darkphotons]
            )
            eventsummary['darkphotons'] = darkphotons
            eventsummary['gendesc'] = gendesc

        leptonjetSrc = {}
        leptonjetSrc['type'] = list(event.ljsource_type)
        leptonjetSrc['energy'] = [p.energy() for p in list(event.ljsource_p4)]
        leptonjetSrc['eta'] = [float(p.eta()) for p in list(event.ljsource_p4)]
        leptonjetSrc['phi'] = [float(p.phi()) for p in list(event.ljsource_p4)]
        leptonjetSrc['color'] = [colors[t] for t in leptonjetSrc['type']]
        eventsummary['leptonjetSrc'] = leptonjetSrc

        leptonjet = {}
        leptonjet['energy'] = [p.energy() for p in list(event.pfjet_p4)]
        leptonjet['eta'] = [float(p.eta()) for p in list(event.pfjet_p4)]
        leptonjet['phi'] = [float(p.phi()) for p in list(event.pfjet_p4)]
        eventsummary['leptonjet'] = leptonjet

        dsamuon = {}
        dsamuon_p4 = list(event.dsamuon_p4)
        dsamuon_selectors = list(event.dsamuon_selectors)
        dsamuon_passIso = [(s&(1<<6))==(1<<6) for s in dsamuon_selectors]
        dsamuon['energy'] = [p.energy() for i, p in enumerate(dsamuon_p4) if dsamuon_passIso[i]]
        dsamuon['eta'] = [p.eta() for i, p in enumerate(dsamuon_p4) if dsamuon_passIso[i]]
        dsamuon['phi'] = [p.phi() for i, p in enumerate(dsamuon_p4) if dsamuon_passIso[i]]
        eventsummary['dsamuon'] = dsamuon

        totalEventsInfo.append(eventsummary)

    ## making plots
    fig, axes = plt.subplots(2,3,figsize=(8*3,6*2), gridspec_kw=dict(wspace=0.1, hspace=0.15))

    for i in range(len(axes.flat)):
        ax = axes.flat[i]
        eventinfo = totalEventsInfo[i+6]
        ljsrcdf = pd.DataFrame(eventinfo['leptonjetSrc'])
        if not ljsrcdf.empty:
            ljsrcdf.plot.scatter(x='eta', y='phi', s=ljsrcdf['energy']*50, c=ljsrcdf['color'], ax=ax, alpha=0.8)
        ljdf = pd.DataFrame(eventinfo['leptonjet'])
        if not ljdf.empty:
            ljdf.plot.scatter(x='eta', y='phi', s=ljdf['energy']*50, c="None", ax=ax, edgecolors='k', linewidth=2, linestyle='--')
        dsadf = pd.DataFrame(eventinfo['dsamuon'])
        if not dsadf.empty:
            dsadf.plot.scatter(x='eta', y='phi', s=dsadf['energy']*50, c='#756bb1', marker='h', ax=ax, alpha=0.8)
        darkphotons = eventinfo.get('darkphotons', None)
        if darkphotons:
            ax.scatter([d[0] for d in darkphotons],
                        [d[1] for d in darkphotons],
                        c='k', marker='D', s=100)
        gendesctxt = eventinfo.get('gendesc', None)
        if gendesctxt:
            ax.text(1,0, gendesctxt,
                    ha='right', va='bottom', fontsize=9, transform=ax.transAxes,)
        ax.set_xlim([-2.5, 2.5])
        ax.set_ylim([-np.pi, np.pi])
        ax.set_xlabel("eta", x=1.0, ha="right")
        ax.set_ylabel("phi", y=1.0, ha="right")
        ax.text(1,1,eventinfo['eventId'],
                    ha='right', va='bottom', fontsize=9, transform=ax.transAxes,)
        ax.set_title('leptonJets+source energy map', x=0, ha='left')

    ## save
    outfilename = '{}/energyV2.png'.format(outputdir)
    fig.savefig(outfilename)
    plt.close()

if __name__ == "__main__":
    main()
