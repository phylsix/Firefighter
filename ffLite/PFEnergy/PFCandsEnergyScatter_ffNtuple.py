#!/usr/bin/env python
from __future__ import print_function
import os
import sys
from collections import defaultdict

import pandas as pd
import matplotlib.pyplot as plt
from Firefighter.ffConfig.dataSample import ffSamples
from Firefighter.ffLite.utils import colors, pType

import ROOT

ROOT.gROOT.SetBatch()

plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"
plt.rcParams["axes.titleweight"] = "semibold"
plt.rcParams["font.family"] = ["Ubuntu", "sans-serif"]

dataType = sys.argv[1]
try:
    fn = ffSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(ffSamples.keys())
    )
    sys.exit(msg)

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), dataType)
if not os.path.exists(outdir):
    os.makedirs(outdir)


def main():

    f = ROOT.TFile.Open(fn)
    dName = f.GetListOfKeys()[0].GetName()
    tName = f.Get(dName).GetListOfKeys()[0].GetName()
    t = f.Get("{}/{}".format(dName, tName))

    bNames = [b.GetName() for b in t.GetListOfBranches()]

    sigMC = any([b.startswith("gen") for b in bNames])

    wentThroughEvents = []
    for i, event in enumerate(t, 1):

        if len(wentThroughEvents) > 10:
            break

        if sigMC:
            darkphotons = []
            for pid, dp, vtx in zip(
                list(event.gen2_pid), list(event.gen2_p4), list(event.gen2_vtx)
            ):
                if abs(pid) not in (11, 13):
                    continue
                darkphotons.append((dp.eta(), dp.phi(), dp.energy(), vtx.rho()))
            if not all(map(lambda p: abs(p[0]) < 2.4, darkphotons)):
                continue
            desc_ = "({:-5.3f}, {:-5.3f}) {:7.3f} GeV {:7.3f} cm"
            desc = "\n".join(
                ["darkphotons [(eta, phi) energy lxy]"]
                + [desc_.format(*dp) for dp in darkphotons]
            )
            print(desc)

        candsData = defaultdict(list)
        candsData["type"] = [x for j in event.pfjet_pfcand_type for x in list(j)]
        candsData["energy"] = [x for j in event.pfjet_pfcand_energy for x in list(j)]
        candsData["eta"] = [x for j in event.pfjet_pfcand_eta for x in list(j)]
        candsData["phi"] = [x for j in event.pfjet_pfcand_phi for x in list(j)]
        candsData["color"] = [colors[t] for t in candsData["type"]]

        if not candsData["type"]:
            continue

        candsDf = pd.DataFrame(candsData)
        fig, ax = plt.subplots(figsize=(8, 6))

        ax = candsDf.plot.scatter(
            x="eta",
            y="phi",
            s=candsDf["energy"] * 50,
            c=candsDf["color"],
            ax=ax,
            alpha=0.8,
        )
        ax.set_xlim([-2.4, 2.4])
        ax.set_ylim([-3.142, 3.142])
        ax.grid()
        ax.set_title(
            "[{}] Energy spread of leptonJet constituents".format(dataType),
            x=0.0,
            ha="left",
        )
        ax.text(
            1.0,
            1.0,
            "Run{}, Lumi{}, Event{}".format(event.run, event.lumi, event.event),
            ha="right",
            va="bottom",
            fontsize=9,
            transform=ax.transAxes,
        )
        ax.set_xlabel("eta", x=1.0, ha="right")
        ax.set_ylabel("phi", y=1.0, ha="right")

        if sigMC:
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

        for ic, color in enumerate(colors[:8]):
            ax.scatter([], [], s=100, c=color, label=pType[ic])
            ax.legend(
                scatterpoints=1,
                title="type",
                fontsize=9,
                framealpha=0.75,
                labelspacing=0.2,
            )

        outfn = os.path.join(
            outdir, "ffNtuple_r{}l{}e{}.png".format(event.run, event.lumi, event.event)
        )
        fig.savefig(outfn)

        wentThroughEvents.append("{}:{}:{}".format(event.run, event.lumi, event.event))
    print("*" * 30, " processed events ", "*" * 30)
    for e in wentThroughEvents:
        print(e)


if __name__ == "__main__":
    main()
