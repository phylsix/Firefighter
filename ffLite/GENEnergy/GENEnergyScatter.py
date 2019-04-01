#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from collections import defaultdict
from DataFormats.FWLite import Events, Handle
from Firefighter.ffConfig.dataSample import samples
from Firefighter.ffLite.utils import colors, pType, inferTypeFromPid
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

ROOT.gROOT.SetBatch()
dataType = sys.argv[1]

plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"
plt.rcParams["axes.titleweight"] = "semibold"
plt.rcParams["font.family"] = ["Ubuntu", "sans-serif"]

try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys())
    )
    sys.exit(msg)

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), dataType)
if not os.path.exists(outdir):
    os.makedirs(outdir)


def main():

    trigHdl = Handle("edm::TriggerResults")
    trigLbl = ("TriggerResults", "", "HLT")

    genHdl = Handle("std::vector<reco::GenParticle>")
    genLbl = ("genParticles", "", "HLT")

    trigpaths = [
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha",
        "HLT_DoubleL2Mu25NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",
    ]

    events = Events(fn)
    print("Sample's event size: ", events.size())

    wentThroughEvents = []

    for i, event in enumerate(events, 1):

        if len(wentThroughEvents) > 10:
            break

        event.getByLabel(trigLbl, trigHdl)
        if not trigHdl.isValid():
            continue

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        triggerResults = trigHdl.product()
        names = event.object().triggerNames(triggerResults)
        trigpathsVersioned = map(
            lambda p: [x for x in names.triggerNames() if x.startswith(p)][0], trigpaths
        )
        if not any(
            map(
                lambda p: triggerResults.accept(names.triggerIndex(p)),
                trigpathsVersioned,
            )
        ):
            continue

        event.getByLabel(genLbl, genHdl)
        if not genHdl.isValid():
            continue
        genpars = genHdl.product()

        if dataType.startswith("signal"):
            # darkphotons
            darkphotons = []
            for g in genpars:
                if g.pdgId() != 32:
                    continue
                darkphotons.append(
                    (g.eta(), g.phi(), g.energy(), g.daughter(0).vertex().rho())
                )
            if not all(map(lambda p: abs(p[0]) < 2.4, darkphotons)):
                continue
            desc_ = "({:-5.3f}, {:-5.3f}) {:7.3f} GeV {:7.3f} cm"
            desc = "\n".join(
                ["darkphotons [(eta, phi) energy lxy]"]
                + [desc_.format(*dp) for dp in darkphotons]
            )
            print(desc)

        genPartData = defaultdict(list)
        for p in genpars:
            # add some filters
            pabspid = abs(p.pdgId())
            if p.numberOfDaughters() != 0:
                continue  # keep final state apriticles only
            if pabspid > 999:
                continue  # mostly baryons
            if pabspid in [12, 14, 16]:
                continue  # neutrinos

            _type = inferTypeFromPid(pabspid)
            if _type == 0:
                print("Found unrecogzied particle: id {}".format(p.pdgId()))
            genPartData["eta"].append(p.eta())
            genPartData["phi"].append(p.phi())
            genPartData["energy"].append(p.energy())
            genPartData["type"].append(_type)
            genPartData["color"].append(colors[_type])

        genpartDf = pd.DataFrame(genPartData)
        fig, ax = plt.subplots(figsize=(8, 6))
        ax = genpartDf.plot.scatter(
            x="eta",
            y="phi",
            s=genpartDf["energy"] * 50,
            c=genpartDf["color"],
            ax=ax,
            alpha=0.8,
        )
        ax.set_xlim([-2.4, 2.4])
        ax.set_ylim([-3.142, 3.142])
        ax.set_title(
            "[{}] Energy spread of GenParticles".format(dataType), x=0.0, ha="left"
        )
        ax.text(
            1.0,
            1.0,
            "Run{}, Lumi{}, Event{}".format(_run, _lumi, _event),
            ha="right",
            va="bottom",
            fontsize=9,
            transform=ax.transAxes,
        )
        ax.set_xlabel("eta", x=1.0, ha="right")
        ax.set_ylabel("phi", y=1.0, ha="right")
        ax.grid()

        for ic, color in enumerate(colors[:6]):
            ax.scatter([], [], s=100, c=color, label=pType[ic])
        ax.legend(
            scatterpoints=1, title="type", fontsize=9, framealpha=0.75, labelspacing=0.2
        )

        if dataType.startswith("signal"):
            bprops = dict(facecolor="w", alpha=0.75)
            ax.text(
                0,
                0,
                desc,
                ha="left",
                va="bottom",
                transform=ax.transAxes,
                fontsize=9,
                bbox=bprops,
            )
            ax.scatter(
                [d[0] for d in darkphotons],
                [d[1] for d in darkphotons],
                c="k",
                marker="D",
                s=100,
            )

        _outfn = "event_r{}l{}e{}.png".format(_run, _lumi, _event)
        outfn = os.path.join(outdir, _outfn)
        fig.savefig(outfn)

        wentThroughEvents.append("{}:{}:{}".format(_run, _lumi, _event))

    print("*" * 30, " processed events ", "*" * 30)
    for e in wentThroughEvents:
        print(e)


if __name__ == "__main__":
    main()
