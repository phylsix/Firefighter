#!/usr/bin/env python
from __future__ import print_function

import math
import os
import sys

import ROOT
from Firefighter.ffConfig.dataSample import ffSamples

ROOT.gROOT.SetBatch()

dataType = "muplus1000"  # sys.argv[1]
try:
    fn = ffSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(ffSamples.keys())
    )
    sys.exit(msg)

f = ROOT.TFile(fn)

dName = f.GetListOfKeys()[0].GetName()
tName = f.Get(dName).GetListOfKeys()[0].GetName()
t = f.Get("{}/{}".format(dName, tName))

bNames = [b.GetName() for b in t.GetListOfBranches()]

for i, event in enumerate(t, 1):

    print("\n", "-" * 75, [i])
    print(
        "[p4]",
        *[
            "\t({:8.3f}, {:8.3f}, {:8.3f}, {:8.3f})".format(
                p.pt(), p.eta(), p.phi(), p.energy()
            )
            for p in event.singlemu_p4
        ],
        sep="\n"
    )
    print("[charge]", list(event.singlemu_charge))
    print("[time]", *[round(v, 3) for v in event.singlemu_time])
    print("[#CSC]", list(event.singlemu_nCSCSegs))
    print("[#DT]", list(event.singlemu_nDTSegs))
    print("[#sharedCSC]", event.singlemu_nCSCSegsShared)
    print("[#sharedDT]", event.singlemu_nDTSegsShared)

    if i > 50:
        break
