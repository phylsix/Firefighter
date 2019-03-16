#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import math
from collections import defaultdict

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from DataFormats.FWLite import Events, Handle
from Firefighter.ffConfig.dataSample import samples
from Firefighter.ffLite.utils import colors, pType

import ROOT
ROOT.gROOT.SetBatch()

plt.style.use('default')
plt.rcParams['grid.linestyle'] = ':'
plt.rcParams['savefig.dpi'] = 120
plt.rcParams['savefig.bbox'] = 'tight'
plt.rcParams['axes.titleweight'] = 'semibold'
plt.rcParams['font.family'] = ['Ubuntu', 'sans-serif']

dataType = sys.argv[1]
try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys()))
    sys.exit(msg)

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), dataType)
if not os.path.exists(outdir):
    os.makedirs(outdir)


def main():

    candsHdl = Handle('std::vector<reco::PFCandidate>')
    candsLbl = ('particleFlow', '', 'RECO')

    genHdl = Handle('std::vector<reco::GenParticle>')
    genLbl = ('genParticles', '', 'HLT')

    dsaHdl = Handle('std::vector<reco::Track>')
    dsaLabel = ('displacedStandAloneMuons', '', 'RECO')

    events = Events(fn)
    print("Sample's event size: ", events.size())

    wentThroughEvents = []
    for i, event in enumerate(events, 1):

        if len(wentThroughEvents) > 10:
            break

        event.getByLabel(genLbl, genHdl)
        if not genHdl.isValid():
            continue
        event.getByLabel(candsLbl, candsHdl)
        if not candsHdl.isValid():
            continue
        event.getByLabel(dsaLabel, dsaHdl)
        if not dsaHdl.isValid():
            continue

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        # darkphoton
        if dataType.startswith('signal'):
            genpars = genHdl.product()
            darkphotons = []
            for g in genpars:
                if g.pdgId() != 32:
                    continue
                darkphotons.append((g.eta(), g.phi(), g.energy(),
                                    g.daughter(0).vertex().rho()))
            if not all(map(lambda p: abs(p[0]) < 2.4, darkphotons)):
                continue
            desc_ = '({:-5.3f}, {:-5.3f}) {:7.3f} GeV {:7.3f} cm'
            desc = '\n'.join(['darkphotons [(eta, phi) energy lxy]'] +
                             [desc_.format(*dp) for dp in darkphotons])
            print(desc)

        cands = candsHdl.product()
        candsData = defaultdict(list)
        for c in cands:
            candsData['eta'].append(c.eta())
            candsData['phi'].append(c.phi())
            candsData['energy'].append(c.energy())
            candsData['type'].append(int(c.particleId()))
            candsData['color'].append(colors[int(c.particleId())])
        dsamus = dsaHdl.product()
        for mu in dsamus:
            candsData['eta'].append(mu.eta())
            candsData['phi'].append(mu.phi())
            candsData['energy'].append(math.hypot(mu.p(), 0.1056584))
            candsData['type'].append(8)
            candsData['color'].append(colors[8])
        candsDf = pd.DataFrame(candsData)
        fig, ax = plt.subplots(figsize=(8, 6))

        ax = candsDf.plot.scatter(
            x='eta',
            y='phi',
            s=candsDf['energy'] * 50,
            c=candsDf['color'],
            ax=ax,
            alpha=0.8)
        ax.set_xlim([-2.4, 2.4])
        ax.set_ylim([-3.142, 3.142])

        ax.set_title(
            '[{}] Energy spread of PFCandidate+dSAmu'.format(dataType),
            x=0.,
            ha='left')
        ax.text(
            1.,
            1.,
            'Run{}, Lumi{}, Event{}'.format(_run, _lumi, _event),
            ha='right',
            va='bottom',
            fontsize=9,
            transform=ax.transAxes)
        ax.set_xlabel('eta', x=1.0, ha='right')
        ax.set_ylabel('phi', y=1.0, ha='right')
        bprops = dict(facecolor='w', alpha=0.75)
        if dataType.startswith('signal'):
            ax.text(
                0,
                0,
                desc,
                ha='left',
                va='bottom',
                transform=ax.transAxes,
                fontsize=9,
                bbox=bprops)
            ax.scatter([d[0] for d in darkphotons],
                       [d[1] for d in darkphotons],
                       c='k',
                       marker='D',
                       s=100)
        ax.grid()

        for ic, color in enumerate(colors[:9]):
            ax.scatter([], [], s=100, c=color, label=pType[ic])
        ax.legend(
            scatterpoints=1,
            title='type',
            fontsize=9,
            framealpha=0.75,
            labelspacing=0.2)

        outfn = os.path.join(outdir, 'event_r{}l{}e{}.png'.format(
            _run, _lumi, _event))
        fig.savefig(outfn)

        wentThroughEvents.append('{}:{}:{}'.format(_run, _lumi, _event))

    print('*' * 30, ' processed events ', '*' * 30)
    for e in wentThroughEvents:
        print(e)


if __name__ == "__main__":
    main()
