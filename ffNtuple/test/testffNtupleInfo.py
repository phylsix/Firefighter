#!/usr/bin/env python
from __future__ import print_function
import ROOT

inputFn = 'testffNtuple.root'
f = ROOT.TFile(inputFn)

dName = f.GetListOfKeys()[0].GetName()
tName = f.Get(dName).GetListOfKeys()[0].GetName()
t = f.Get('{}/{}'.format(dName, tName))

bNames = [b.GetName() for b in t.GetListOfBranches()]

sigMC = any([b.startswith('gen') for b in bNames])
nonSig = not sigMC

for i, event in enumerate(t, 1):

    print('\n', '-' * 75, [i])
    if sigMC:
        print('genpid', [p for p in event.gen_pid])
        print('genp4', [(round(p.pt(), 3), round(p.eta(), 3),
                         round(p.phi(), 3)) for p in event.gen_p4])
        print('gen2pid', [p for p in event.gen2_pid])
        print('gen2p4', [(round(p.pt(), 3), round(p.eta(), 3),
                          round(p.phi(), 3)) for p in event.gen2_p4])

    print('[pfcands] <type>', *[list(j)
                                for j in event.pfjet_pfcand_type], sep='\n')
    print('[pfcands] <pt>', *[map(lambda p:round(p.pt(), 3), j)
                              for j in event.pfjet_pfcand_p4], sep='\n')
    print('[pfcands] <tkD0Sig>', *[map(lambda v: round(v, 3), list(j))
                                   for j in event.pfjet_pfcand_tkD0Sig], sep='\n')

    if i > 40:
        break
