#!/usr/bin/env python
from __future__ import print_function

import os
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt
import ROOT
from DataFormats.FWLite import Events, Handle
from utils import *

ROOT.gROOT.SetBatch()

plt.style.use('default')
plt.rcParams['grid.linestyle']  = ':'
plt.rcParams['savefig.dpi'] = 120
plt.rcParams['savefig.bbox'] = 'tight'


# ZZ
fn = 'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/ZZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/110000/1BE38E5F-9F8A-3C47-9AEE-0CFD8D5E2EF1.root'
events = Events(fn)
print('- Sample: {}'.format(fn))
print("- Number of events: {}".format(events.size()))

genHdl = Handle('std::vector<reco::GenParticle>')
genLbl = ('genParticles', '', 'HLT')

recoMuHdl = Handle('std::vector<reco::Muon>')
recoMuLbl = ('muons', '', 'RECO')

res_pt = defaultdict(list)
res_eta = defaultdict(list)
res_phi = defaultdict(list)
res_dR = defaultdict(list)

recoMu_n = list()
recoMu_pt = list()
recoMu_eta = list()
recoMu_dR = list() # deltaR between leading and subleading

for i, event in enumerate(events, 1):

    if i%1000 == 1: print("ZZ Event : {}".format(i))
    # if i>50: break

    #############################################
    ## GEN
    #############################################
    event.getByLabel(genLbl, genHdl)
    if not genHdl.isValid(): continue
    genp = genHdl.product()

    nGen = len(genp)
    # print('Number of gen particles: {}'.format(nGen))
    if i==1:
        print('{:6} {:6} {:6} {:>12} {:>12} {:>12}'.format(
            'pdgId',
            'momPid',
            'status',
            'pT',
            'eta',
            'phi'
        ))
        print('='*(6*3 + 5 + 12*3))

    mompid_ = 23 # Z
    momCol_ = [g for g in genp if g.isLastCopy() and abs(g.pdgId())==mompid_]
    muCol_ = sorted([g for g in genp if g.isLastCopy() and abs(g.pdgId())==13], key=lambda p: p.pt(), reverse=True)[:4]
    elCol_ = sorted([g for g in genp if g.isLastCopy() and abs(g.pdgId())==11], key=lambda p: p.pt(), reverse=True)[:4]

    if len(muCol_)>=2:
        for im in range(len(muCol_)):
            for j in range(im+1, len(muCol_)):
                if (muCol_[im].mother(0) == muCol_[j].mother(0)):
                    res_dR[13].append( delta_r(muCol_[im], muCol_[j]) )

    if len(elCol_)>=2:
        for ie in range(len(elCol_)):
            for j in range(ie+1, len(elCol_)):
                if (elCol_[ie].mother(0) == elCol_[j].mother(0)):
                    res_dR[13].append( delta_r(elCol_[ie], elCol_[j]) )


    for g in momCol_+muCol_+elCol_:

        res_pt [abs(g.pdgId())].append( g.pt() )
        res_eta[abs(g.pdgId())].append( g.eta() )
        res_phi[abs(g.pdgId())].append( g.phi() )

        if i<10:
            print('{:6} {:6} {:6} {:12.4f} {:12.4f} {:12.4f}'.format(
                g.pdgId(),
                g.mother(0).pdgId(),
                g.status(),
                g.pt(),
                g.eta(),
                g.phi()
            ))
    if i<10:
        print('-'*(6*3 + 5 + 12*3), '[{}]'.format(i))


    #############################################
    ## reco Muon
    #############################################
    event.getByLabel(recoMuLbl, recoMuHdl)
    if not recoMuHdl.isValid():
        print('{} is not valid!'.format(str(recoMuLbl)))
        continue
    recoMu = recoMuHdl.product()

    recoMu_n.append( len(recoMu) )
    for mu_ in recoMu:

        recoMu_pt.append( mu_.pt() )
        recoMu_eta.append( mu_.eta() )

    if len(recoMu)>=2:
        for i in range(len(recoMu)):
            for j in range(i+1, len(recoMu)):
                if recoMu[i].charge() * recoMu[j].charge() != -1: continue
                recoMu_dR.append( delta_r(recoMu[i], recoMu[j]) )



outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'plots')
if not os.path.exists(outdir):
    os.makedirs(outdir)

fig, ax = plt.subplots(figsize=(8, 6))
ax.hist(np.array(res_pt[23]), bins=50, range=[0, 500], histtype='step', normed=True, label='gen Z')
ax.hist(np.array(res_pt[13]), bins=50, range=[0, 500], histtype='step', normed=True, label='gen $\mu$')
ax.hist(np.array(res_pt[11]), bins=50, range=[0, 500], histtype='step', normed=True, label='gen e')
ax.hist(np.array(recoMu_pt), bins=50, range=[0, 500], histtype='step', normed=True, label='reco $\mu$')

ax.set_xlabel('pT [GeV]')
ax.set_ylabel('A.U.')
ax.set_title(r'ZZ pT')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'ZZ_pt.png'))
plt.cla()

ax.hist(np.array(res_eta[23]), bins=50, range=[-6, 6], histtype='step', normed=True, label='gen Z')
ax.hist(np.array(res_eta[13] ), bins=50, range=[-6, 6], histtype='step', normed=True, label='gen $\mu$')
ax.hist(np.array(res_eta[11] ), bins=50, range=[-6, 6], histtype='step', normed=True, label='gen e')
ax.hist(np.array(recoMu_eta ), bins=50, range=[-6, 6], histtype='step', normed=True, label='reco $\mu$')

ax.set_xlabel('$\eta$')
ax.set_ylabel('A.U.')
ax.set_title(r'ZZ eta')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'ZZ_eta.png'))
plt.cla()

ax.hist(np.array(res_phi[23]), bins=50, range=[-3.142, 3.142], histtype='step', normed=True, label='Z')
ax.hist(np.array(res_phi[13] ), bins=50, range=[-3.142, 3.142], histtype='step', normed=True, label='$\mu$')
ax.hist(np.array(res_phi[11] ), bins=50, range=[-3.142, 3.142], histtype='step', normed=True, label='e')

ax.set_xlabel('$\phi$')
ax.set_ylabel('A.U.')
ax.set_title(r'ZZ phi')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'ZZ_phi.png'))
plt.cla()


ax.hist(np.array(res_dR[13]), bins=50, range=[0, 8], histtype='step', normed=True, label='gen $\mu$')
ax.hist(np.array(res_dR[11]), bins=50, range=[0, 8], histtype='step', normed=True, label='gen e')
ax.hist(np.array(recoMu_dR), bins=50, range=[0, 8], histtype='step', normed=True, label='reco $\mu$ (OS)')

ax.set_xlabel('$\Delta R$')
ax.set_ylabel('A.U.')
ax.set_title(r'ZZ $\Delta R$')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'ZZ_dR.png'))
plt.cla()


ax.hist(np.array(recoMu_n), bins=10, range=[0, 10], histtype='step', normed=True, label='# reco $\mu (OS)$')

ax.set_xlabel('# reco muon')
ax.set_ylabel('A.U.')
ax.set_title(r'ZZ multiplicity')
plt.yscale('log', nonposy='clip')

ax.legend()
ax.grid()

fig.savefig(os.path.join(outdir, 'ZZ_multiplicity.png'))
plt.cla()
