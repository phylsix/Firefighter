#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from Firefighter.ffConfig.dataSample import ffSamples

ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
try:
    fn = ffSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys())
    )
    sys.exit(msg)

f = ROOT.TFile(fn)

dName = f.GetListOfKeys()[0].GetName()
tName = f.Get(dName).GetListOfKeys()[0].GetName()
t = f.Get("{}/{}".format(dName, tName))

bNames = [b.GetName() for b in t.GetListOfBranches()]

sigMC = any([b.startswith("gen") for b in bNames])
nonSig = not sigMC

for i, event in enumerate(t, 1):

    print("\n", "-" * 75, [i])
    if sigMC:
        print("genpid", [p for p in event.gen_pid])
        print(
            "genp4",
            *[
                "({:8.3f}, {:8.3f}, {:8.3f}, {:8.3f})".format(
                    p.pt(), p.eta(), p.phi(), p.energy()
                )
                for p in event.gen_p4
            ],
            sep="\n"
        )
        print("gen2pid", [p for p in event.gen2_pid])
        print("gen2vtx", [round(v.rho(), 3) for v in event.gen2_vtx])
        print(
            "gen2p4",
            *[
                "({:8.3f}, {:8.3f}, {:8.3f}, {:8.3f})".format(
                    p.pt(), p.eta(), p.phi(), p.energy()
                )
                for p in event.gen2_p4
            ],
            sep="\n"
        )

    print("[pfcands] <type>", *[list(j) for j in event.pfjet_pfcand_type], sep="\n")
    print(
        "[pfcands] <pt>",
        *[map(lambda v: round(v, 3), list(j)) for j in event.pfjet_pfcand_pt],
        sep="\n"
    )
    print(
        "[pfcands] <tkD0Sig>",
        *[map(lambda v: round(v, 3), list(j)) for j in event.pfjet_pfcand_tkD0Sig],
        sep="\n"
    )

    print("[Event] <pu> - ", event.puInteractionNum)
    print("[Event] <trueInter> - ", event.trueInteractionNum)
    print(
        "[HFTag] <DeepCSV b> [L, M, T]/Total",
        "[{}, {}, {}]/{}".format(
            len(filter(lambda x: x > 0.1241, event.hftagscore_DeepCSV_b)),
            len(filter(lambda x: x > 0.4184, event.hftagscore_DeepCSV_b)),
            len(filter(lambda x: x > 0.7527, event.hftagscore_DeepCSV_b)),
            len(event.hftagscore_DeepCSV_b),
        ),
        sep="\n",
    )

    print("[pfjet] <lambda>", *[round(v, 3) for v in event.pfjet_subjet_lambda])
    print("[pfjet] <epsilon>", *[round(v, 3) for v in event.pfjet_subjet_epsilon])
    print("[pfjet] <ecf1>", *[round(v, 3) for v in event.pfjet_subjet_ecf1])
    print("[pfjet] <ecf2>", *[round(v, 3) for v in event.pfjet_subjet_ecf2])
    print("[pfjet] <ecf3>", *[round(v, 3) for v in event.pfjet_subjet_ecf3])

    if i > 50:
        break
