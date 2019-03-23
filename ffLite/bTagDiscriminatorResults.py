#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffConfig.dataSample import samples

ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys()))
    sys.exit(msg)


def main():

    ak4chsHdl = Handle('std::vector<reco::PFJet>')
    ak4chsLbl = ('ak4PFJetsCHS', '', 'RECO')
    bTagHdl = Handle(
        'edm::AssociationVector<edm::RefToBaseProd<reco::Jet>,vector<float>,edm::RefToBase<reco::Jet>,unsigned int,edm::helper::AssociationIdenticalKeyReference>'
    )
    bTagLbl = ('pfCombinedInclusiveSecondaryVertexV2BJetTags', '', 'RECO')

    events = Events(fn)
    print("Sample's event size: ", events.size())

    for i, event in enumerate(events, 1):

        if i > 30: break

        event.getByLabel(bTagLbl, bTagHdl)
        if not bTagHdl.isValid(): continue
        event.getByLabel(ak4chsLbl, ak4chsHdl)
        if not ak4chsHdl.isValid(): continue

        btagDiscr = bTagHdl.product()
        ak4chs = ak4chsHdl.product()
        print('Event', '{:>4} :'.format(i))
        # res = [btagDiscr[ak4chs.refAt(i)].second for i in range(len(ak4chs))]
        res = [x.second for x in btagDiscr]
        print(len(ak4chs), [round(x, 3) for x in res if x > 0])


if __name__ == "__main__":
    main()
