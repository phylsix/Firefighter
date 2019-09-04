#!/usr/bin/env python
"""draw energy scatter plots on eta-phi plane. Inc:
- PFCandidates
- leptonJets
- AK4CHS with some quality cuts
"""
from __future__ import print_function
import os
import sys
from collections import defaultdict
import ROOT
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.utils import colors, pType

ROOT.gROOT.SetBatch()
plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"


inputfile = sys.argv[1]
assert os.path.isfile(inputfile)

outputdir = inputfile.split('.')[0]
try:
    os.makedirs(outputdir)
except:
    pass



def main():

    labelHandls = {
        'pfcands': (("particleFlow", "", "RECO"), Handle("vector<reco::PFCandidate>")),
        'ak4chs': (("ak4PFJetsCHS", "", "RECO"), Handle("vector<reco::PFJet>")),
        'filteredcands': (("particleFlowIncDSA", "", "FF"), Handle("vector<edm::FwdPtr<reco::PFCandidate> >")),
        'leptonjets': (("ffLeptonJetFwdPtrs", "", "FF"), Handle("vector<edm::FwdPtr<reco::PFJet> >")),
        'filteredleptonjets': (("filteredLeptonJet", "", "FF"), Handle("vector<edm::FwdPtr<reco::PFJet> >")),
        'genparticles': (("genParticles", "", "HLT"), Handle("std::vector<reco::GenParticle>")),
    }

    issignal=False
    if 'signal' in inputfile:
        issignal = True

    numevents = 5
    ievt = 0
    events = Events(inputfile)
    for i, event in enumerate(events, 1):

        if ievt > numevents: break

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        event.getByLabel(*labelHandls['pfcands'])
        if not labelHandls['pfcands'][1].isValid(): continue
        event.getByLabel(*labelHandls['ak4chs'])
        if not labelHandls['ak4chs'][1].isValid(): continue
        event.getByLabel(*labelHandls['filteredcands'])
        if not labelHandls['filteredcands'][1].isValid(): continue
        event.getByLabel(*labelHandls['filteredleptonjets'])
        if not labelHandls['filteredleptonjets'][1].isValid(): continue
        event.getByLabel(*labelHandls['leptonjets'])
        if not labelHandls['leptonjets'][1].isValid(): continue
        if issignal:
            event.getByLabel(*labelHandls['genparticles'])
            if not labelHandls['genparticles'][1].isValid(): continue

        pfcands = labelHandls['pfcands'][1].product()
        pfcandsData = defaultdict(list)
        for c in pfcands:
            pfcandsData['eta'].append(c.eta())
            pfcandsData['phi'].append(c.phi())
            pfcandsData['energy'].append(c.energy())
            pfcandsData['color'].append(colors[int(c.particleId())])
        pfcandsDf = pd.DataFrame(pfcandsData)

        ak4chsjets = labelHandls['ak4chs'][1].product()
        ak4chsData = defaultdict(list)
        for j in ak4chsjets:
            if j.pt()<20: continue
            ak4chsData['eta'].append(j.eta())
            ak4chsData['phi'].append(j.phi())
            ak4chsData['energy'].append(j.energy())
        ak4chsDf = pd.DataFrame(ak4chsData)

        filteredcands = labelHandls['filteredcands'][1].product()
        filteredcandsData = defaultdict(list)
        generaltrackRefId = None
        for c in pfcands:
            if c.trackRef().isNonnull():
                generaltrackRefId = c.trackRef().id()
                break
        for c in filteredcands:
            filteredcandsData['eta'].append(c.eta())
            filteredcandsData['phi'].append(c.phi())
            filteredcandsData['energy'].append(c.energy())
            if c.trackRef().isNonnull() and c.trackRef().id() != generaltrackRefId:
                filteredcandsData['color'].append(colors[8])
            else:
                filteredcandsData['color'].append(colors[int(c.particleId())])
        filteredcandsDf = pd.DataFrame(filteredcandsData)

        filteredleptonjets = labelHandls['filteredleptonjets'][1].product()
        filteredleptonjetsData = defaultdict(list)
        for j in filteredleptonjets:
            #if j.pt()<20: continue
            filteredleptonjetsData['eta'].append(j.eta())
            filteredleptonjetsData['phi'].append(j.phi())
            filteredleptonjetsData['energy'].append(j.energy())
        filteredleptonjetsDf = pd.DataFrame(filteredleptonjetsData)

        leptonjets = labelHandls['leptonjets'][1].product()
        leptonjetsData = defaultdict(list)
        for j in leptonjets:
            #if j.pt()<20: continue
            leptonjetsData['eta'].append(j.eta())
            leptonjetsData['phi'].append(j.phi())
            leptonjetsData['energy'].append(j.energy())
        leptonjetsDf = pd.DataFrame(leptonjetsData)

        if issignal:
            genparticles = labelHandls['genparticles'][1].product()
            darkphotons = []
            dmboundstatemass = None
            darkphotonmass = None
            for p in genparticles:
                if p.pdgId() != 32: continue
                darkphotons.append(
                    (p.eta(), p.phi(), p.energy(), p.daughter(0).vertex().rho())
                )
                dmboundstatemass = p.mother(0).mass()
                darkphotonmass = p.mass()
            desc_ = "({:-5.3f}, {:-5.3f}) {:7.2f} GeV {:7.2f} cm"
            desc = "\n".join(
                ["mXX: {:6.1f} GeV, mA: {:4.2f} GeV".format(dmboundstatemass, darkphotonmass), '\n']
                + ["darkphotons [(eta, phi) energy lxy]"]
                + [desc_.format(*dp) for dp in darkphotons]
            )

        fig, axes = plt.subplots(2,3,figsize=(8*3,6*2), gridspec_kw=dict(wspace=0.1, hspace=0.15))
        pfcandsDf.plot.scatter(x='eta', y='phi', s=pfcandsDf['energy']*50,
                               c=pfcandsDf['color'], ax=axes[0][0], alpha=0.8,)
        axes[0][0].set_title('PFCandidates energy map', x=0, ha='left')

        pfcandsDf.plot.scatter(x='eta', y='phi', s=pfcandsDf['energy']*50,
                               c=pfcandsDf['color'], ax=axes[0][1], alpha=0.4,)
        if ak4chsDf.size:
            ak4chsDf.plot.scatter(x='eta', y='phi', s=ak4chsDf['energy']*50,
                                  ax=axes[0][1], c="None",
                              edgecolors='k', linewidth=2, linestyle='--')
        axes[0][1].set_title('PFCandidates+AK4JetsCHS energy map', x=0, ha='left')

        filteredcandsDf.plot.scatter(x='eta', y='phi', s=filteredcandsDf['energy']*50,
                                     c=filteredcandsDf['color'], ax=axes[1][0], alpha=0.8,)
        axes[1][0].set_title('PFCandidates for leptonJets energy map', x=0, ha='left')

        filteredcandsDf.plot.scatter(x='eta', y='phi', s=filteredcandsDf['energy']*50,
                                     c=filteredcandsDf['color'], ax=axes[1][1], alpha=0.4,)
        if leptonjetsDf.size:
            leptonjetsDf.plot.scatter(x='eta', y='phi', s=leptonjetsDf['energy']*50,
                                    ax=axes[1][1], c="None",
                                    edgecolors='k', linewidth=2, linestyle='--')
        axes[1][1].set_title('leptonJets+source energy map', x=0, ha='left')

        filteredcandsDf.plot.scatter(x='eta', y='phi', s=filteredcandsDf['energy']*50,
                                     c=filteredcandsDf['color'], ax=axes[1][2], alpha=0.4,)
        if filteredleptonjetsDf.size:
            filteredleptonjetsDf.plot.scatter(x='eta', y='phi', s=filteredleptonjetsDf['energy']*50,
                                    ax=axes[1][2], c="None",
                                    edgecolors='k', linewidth=2, linestyle='--')
        axes[1][2].set_title('filtered leptonJets+source energy map', x=0, ha='left')

        for i, ax in enumerate(axes.flat):
            if i==2: continue
            ax.set_xlabel("eta", x=1.0, ha="right")
            ax.set_ylabel("phi", y=1.0, ha="right")
            ax.set_xlim([-2.5, 2.5])
            ax.set_ylim([-np.pi, np.pi])
            ax.text(1,1,'{}-{}-{}'.format(_run, _lumi, _event),
                    ha='right', va='bottom', fontsize=9, transform=ax.transAxes,)
            if issignal:
                ax.scatter([d[0] for d in darkphotons],
                           [d[1] for d in darkphotons],
                           c='k', marker='D', s=100)

        axes[0][2].get_xaxis().set_visible(False)
        axes[0][2].get_yaxis().set_visible(False)
        for x in ['top', 'left', 'right', 'bottom']:
            axes[0][2].spines[x].set_visible(False)
        for e in np.array([1, 10, 100, 200, 300]):
            axes[0][2].scatter([], [], s=e*50, c="None", label='{} GeV'.format(int(e)))
        axes[0][2].legend(scatterpoints=1, ncol=1, fontsize=9,
                  framealpha=0.75, labelspacing=8, loc='lower right',
                  bbox_to_anchor=(1, 0))
        for ic, color in enumerate(colors):
            axes[0][2].scatter([], [], s=100, c=color, label=pType[ic])
        axes[0][2].legend(scatterpoints=1, ncol=2, fontsize=9,
                  framealpha=0.75, columnspacing=1, loc='lower left',
                  bbox_to_anchor=(0.2, 0))

        if issignal:
            plt.text(0, 0.8, desc, ha='left', va='top', multialignment="left",
                     fontsize=10, transform=axes[0][2].transAxes)

        outfilename = '{}/{}-{}-{}.png'.format(outputdir, _run, _lumi, _event)
        fig.savefig(outfilename)
        plt.close()

        ievt += 1




if __name__ == "__main__":
    main()
