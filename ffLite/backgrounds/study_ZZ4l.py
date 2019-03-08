#!/usr/bin/env python
from __future__ import print_function

import os
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt
import ROOT
from DataFormats.FWLite import Events, Handle
import Firefighter.ffLite.utils as fu
from Firefighter.ffLite.dataSample import samples

ROOT.gROOT.SetBatch()

plt.style.use('default')
plt.rcParams['grid.linestyle'] = ':'
plt.rcParams['savefig.dpi'] = 120
plt.rcParams['savefig.bbox'] = 'tight'

bkgType = 'ZZTo4L'
fn = samples[bkgType]
events = Events(fn)
print('- Sample: {}'.format(fn))
print("- Number of events: {}".format(events.size()))

puHdl = Handle('std::vector<PileupSummaryInfo>')
puLbl = ('addPileupInfo', '', 'HLT')

pfHdl = Handle('std::vector<reco::PFCandidate>')
pfLbl = ('particleFlow', '', 'RECO')

puNum = list()
trueNum = list()

for i, event in enumerate(events, 1):

    if i > 50:
        break

    #################################
    # pile up
    #################################

    event.getByLabel(puLbl, puHdl)
    if not puHdl.isValid():
        continue
    pu = puHdl.product()

    for p in pu:
        if p.getBunchCrossing() != 0:
            continue
        puNum.append(p.getPU_NumInteractions())
        trueNum.append(p.getTrueNumInteractions())

    if i == 1:
        print('>> Event ', i)
        print('[BX] ', [p.getBunchCrossing() for p in pu])
        print('[PU] ', [p.getPU_NumInteractions() for p in pu])
        print('[TN] ', [p.getTrueNumInteractions() for p in pu])
        print('--------------------')

    #################################
    # PFCandidates
    #################################

    event.getByLabel(pfLbl, pfHdl)
    if not pfHdl.isValid():
        continue
    pf = pfHdl.product()

    print('>> Event ', i)
    print([(p.particleId(), p.pdgId()) for p in pf])

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'plots')
if not os.path.exists(outdir):
    os.makedirs(outdir)

fig, ax = plt.subplots(figsize=(8, 6))
ax.hist(
    np.array(puNum), histtype='step', normed=True, label='#pu interactions')
ax.hist(
    np.array(trueNum),
    histtype='step',
    normed=True,
    label='#true interactions')

ax.set_xlabel('# interactions')
ax.set_ylabel('A.U.')
ax.set_title(r'pileup summary')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'zz4l_pu.png'))
plt.cla()
