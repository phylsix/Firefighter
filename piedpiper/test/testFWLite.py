#!/usr/bin/env python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()

events = Events('../cfg/SIDM_GENSIM.root')

#handle = Handle()
#label = ()

for i, event in enumerate(events):

    print("---------------------------------")
    print("## Event ", i)
