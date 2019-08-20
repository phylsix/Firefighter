#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()


def dumpHepMC(e, hl):

    e.getByLabel(*hl["hepmc"])
    assert hl["hepmc"][1].isValid()

    hepmcprod = hl["hepmc"][1].product()
    hepmcprod.getHepMCData().print()


def main():

    inputfilename = sys.argv[1]
    assert os.path.isfile(inputfilename)
    label_plus_handles = {"hepmc": (("generatorSmeared", "", "SIM"), Handle("edm::HepMCProduct"))}

    events = Events(inputfilename)
    print("num_events:", events.size())

    for i, event in enumerate(events, 1):

        if i > 20:
            break

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        print("{} : {} : {}".format(_run, _lumi, _event).center(79, "*"))

        dumpHepMC(event, label_plus_handles)

        print("_" * 79)


if __name__ == "__main__":
    main()
