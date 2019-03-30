#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffConfig.dataSample import samples
from Firefighter.ffLite.utils import formatPoint

ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys())
    )
    sys.exit(msg)


def main():

    isvHdl = Handle("std::vector<reco::Vertex>")
    isvLbl = ("inclusiveSecondaryVertices", "", "RECO")

    events = Events(fn)
    print("Sample's event size: ", events.size())

    for i, event in enumerate(events, 1):

        if i > 10:
            break

        event.getByLabel(isvLbl, isvHdl)
        if not isvHdl.isValid():
            continue

        isv = isvHdl.product()
        print(
            "Event",
            "{:>4} :".format(i),
            *[formatPoint(v.position()) for v in isv if v.isValid()]
        )


if __name__ == "__main__":
    main()
