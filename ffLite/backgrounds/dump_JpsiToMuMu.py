#!/usr/bin/env python
from __future__ import print_function

import os
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.dataSample import samples

ROOT.gROOT.SetBatch()

plt.style.use('default')
plt.rcParams['grid.linestyle'] = ':'
plt.rcParams['savefig.dpi'] = 120
plt.rcParams['savefig.bbox'] = 'tight'

bkgType = 'JpsiToMuMu'
# JpsiToMuMu
fn = samples[bkgType]
events = Events(fn)
print('- Sample: {}'.format(fn))
print("- Number of events: {}".format(events.size()))

genHdl = Handle('std::vector<reco::GenParticle>')
genLbl = ('genParticles', '', 'HLT')

res_pt = defaultdict(list)
res_eta = defaultdict(list)
res_phi = defaultdict(list)

for i, event in enumerate(events, 1):

    if i % 1000 == 1:
        print("Event : {}".format(i))
    # if i>10: break

    event.getByLabel(genLbl, genHdl)
    if not genHdl.isValid():
        continue
    genp = genHdl.product()

    nGen = len(genp)
    # print('Number of gen particles: {}'.format(nGen))
    if i == 1:
        print('{:6} {:>8} {:6} {:>12} {:>12} {:>12} {:4}'.format(
            'pdgId', 'momPid', 'status', 'pT', 'eta', 'phi', '#dau'))
        print('=' * (6 * 2 + 8 + 5 + 12 * 3 + 5))

    mompid_ = 443  # jpsi
    momCol_ = [g for g in genp if g.isLastCopy() and abs(g.pdgId()) == mompid_]
    muCol_ = sorted(
        [g for g in genp if g.isLastCopy() and abs(g.pdgId()) == 13],
        key=lambda p: p.pt(),
        reverse=True)[:2]

    for g in momCol_ + muCol_:

        pid_ = abs(g.pdgId())

        res_pt[pid_].append(g.pt())
        res_eta[pid_].append(g.eta())
        res_phi[pid_].append(g.phi())

        if i < 10:
            print('{:6} {:8} {:6} {:12.4f} {:12.4f} {:12.4f} {:4}'.format(
                g.pdgId(),
                g.mother(0).pdgId(), g.status(), g.pt(), g.eta(), g.phi(),
                g.numberOfDaughters()))
    if i < 10:
        print('-' * (6 * 2 + 8 + 5 + 12 * 3 + 5))

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'plots')
if not os.path.exists(outdir):
    os.makedirs(outdir)

fig, ax = plt.subplots(figsize=(8, 6))
ax.hist(
    np.array(res_pt[443]),
    bins=50,
    range=[0, 50],
    histtype='step',
    normed=True,
    label='$J/\psi$')
ax.hist(
    np.array(res_pt[13]),
    bins=50,
    range=[0, 50],
    histtype='step',
    normed=True,
    label='$\mu$')

ax.set_xlabel('pT [GeV]')
ax.set_ylabel('A.U.')
ax.set_title(r'$J/\psi \rightarrow \mu\mu$ pT')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'JpsiToMuMu_pt.png'))
plt.cla()

ax.hist(
    np.array(res_eta[443]),
    bins=50,
    range=[-6, 6],
    histtype='step',
    normed=True,
    label='$J/\psi$')
ax.hist(
    np.array(res_eta[13]),
    bins=50,
    range=[-6, 6],
    histtype='step',
    normed=True,
    label='$\mu$')

ax.set_xlabel('$\eta$')
ax.set_ylabel('A.U.')
ax.set_title(r'$J/\psi \rightarrow \mu\mu$ eta')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'JpsiToMuMu_eta.png'))
plt.cla()

ax.hist(
    np.array(res_phi[443]),
    bins=50,
    range=[-3.142, 3.142],
    histtype='step',
    normed=True,
    label='$J/\psi$')
ax.hist(
    np.array(res_phi[13]),
    bins=50,
    range=[-3.142, 3.142],
    histtype='step',
    normed=True,
    label='$\mu$')

ax.set_xlabel('$\phi$')
ax.set_ylabel('A.U.')
ax.set_title(r'$J/\psi \rightarrow \mu\mu$ phi')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'JpsiToMuMu_phi.png'))
plt.cla()
