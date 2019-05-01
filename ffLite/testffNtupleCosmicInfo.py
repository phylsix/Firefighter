#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import math
import ROOT
from Firefighter.ffConfig.dataSample import ffSamples

ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
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

for i, event in enumerate(t, 1):

    print("\n", "-" * 75, [i])

    print("[muonTime] <hemisphere>", *event.muon_hemisphere)
    # print("[muonTime] <nDof>", *event.muon_timeNdof)
    # print(
    #     "[muonTime] <timeAtIpInOut>", *[round(v, 3) for v in event.muon_timeAtIpInOut]
    # )
    print("[muonTime] <rpcbxave>", *event.muon_rpcBxAve)
