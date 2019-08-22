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


def dumpGenParticle(e, hls):

    hl = hls['genparticles']
    e.getByLabel(*hl)
    assert hl[1].isValid()

    genparticles = hl[1].product()
    print("{:>8} | {:>8} {:>8} {:>8} {:>8}  {}".format(
        'pid', 'pt', 'vxy', 'vz', 'v3d', 'statusFlags'))
    for p in genparticles:
        pid = abs(p.pdgId())
        if pid not in [11, 13, 32]:
            continue
        # if pid in [11, 13] and p.status() != 1:
        #     continue
        # statusflags = p.statusFlags()
        # if not all([statusflags.isLastCopy(),
        #             statusflags.fromHardProcess(),
        #             statusflags.isPrompt()
        #             ]):
        #     continue
        if pid in [11, 13] and not all([
            p.isPromptFinalState(),
            p.fromHardProcessFinalState(),
            p.isLastCopy()]):
            continue
        print("{:>8} | {:8.3f} {:8.3f} {:8.3f} {:8.3f} {}".format(
            p.pdgId(), p.pt(), p.vertex().rho(), p.vertex().z(), p.vertex().r(), p.statusFlags().flags_.to_string()))


def main():

    inputfilename = sys.argv[1]
    assert os.path.isfile(inputfilename)
    label_plus_handles = {
        "hepmc": (("generatorSmeared", "", "SIM"), Handle("edm::HepMCProduct")),
        "genparticles": (("genParticles", "", "SIM"), Handle("vector<reco::GenParticle>"))
    }

    events = Events(inputfilename)
    print("num_events:", events.size())

    for i, event in enumerate(events, 1):

        if i > 20:
            break

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        print("{} : {} : {}".format(_run, _lumi, _event).center(79, "*"))

        # dumpHepMC(event, label_plus_handles)
        dumpGenParticle(event, label_plus_handles)

        print("_" * 79)


if __name__ == "__main__":
    main()
