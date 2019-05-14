#!/usr/bin/env python
from __future__ import print_function
import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True

import os
import sys
import argparse
from collections import defaultdict

import pandas as pd
import matplotlib.pyplot as plt
from Firefighter.ffConfig.dataSample import ffSamples
from Firefighter.ffLite.utils import colors, pType

ROOT.gROOT.SetBatch()

plt.style.use("default")
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.dpi"] = 120
plt.rcParams["savefig.bbox"] = "tight"
plt.rcParams["axes.titleweight"] = "semibold"
plt.rcParams["font.family"] = ["Ubuntu", "sans-serif"]

parser = argparse.ArgumentParser(description="make energy scatter plot from ffNtuple")
parser.add_argument("--type", default="signal-4mu")
parser.add_argument("--nevents", type=int, default=10, help="number of events to plot")
parser.add_argument(
    "--eventlist",
    type=str,
    default="",
    help="path to text file which stores the list of events to plot",
)
parser.add_argument(
    "--writeeventlist",
    action="store_false",
    help="print processed events into a text file? default: True",
)
args = parser.parse_args()

nevents = args.nevents
if args.eventlist:
    eventlistFile = args.eventlist
    try:
        print("++ Processing events from {}".format(eventlistFile))
        eventsallowed = open(eventlistFile).read().split()
        eventsallowed = [
            tuple(map(lambda s: int(s), x.split(":"))) for x in eventsallowed
        ]
    except Exception as e:
        print("Exception occured while trying to read {}".format(eventlistFile))
        print("It may not exist, or content is broken. Exiting..")
        print("Msg:", str(e))
        sys.exit(1)
else:
    print("++ Going to process {} events".format(nevents))

dataType = args.type
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

        if (
            args.eventlist
            and (int(event.run), int(event.lumi), int(event.event)) not in eventsallowed
        ):
            continue
        if not args.eventlist and len(wentThroughEvents) > nevents:
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

        for ic, color in enumerate(colors):
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
    processed_str = "\n".join(wentThroughEvents)
    print(processed_str)
    if args.writeeventlist:
        with open("events_ffNtuple.log", "w") as f:
            f.write(processed_str)
        print(">> events_ffNtuple.log")


if __name__ == "__main__":
    main()
