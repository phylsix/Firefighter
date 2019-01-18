#!/usr/bin/env python
from __future__ import print_function
import ROOT

inputFn = 'testffNtuple.root'
# inputFn = 'ffNtuple_SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48.root'
f = ROOT.TFile(inputFn)

t = f.Get('ffNtuplizer/ffNtuple')
for i, event in enumerate(t,1):

    print('-'*79)
    print('Event: ', i)
    print('genpid', [p for p in event.gen_pid])
    print('genp4', [(round(p.pt(),3), round(p.eta(),3), round(p.phi(),3)) for p in event.gen_p4])
    print('gen2pid', [p for p in event.gen2_pid])
    print('gen2p4', [(round(p.pt(),3), round(p.eta(),3), round(p.phi(),3)) for p in event.gen2_p4])
    print('jetTkimpact2d', [m for x in event.pfjet_kinvtx_tkImpactDist2d for m in x])
    print('jetMaxCandPtType', [x for x in event.pfjet_pfcands_maxPtType])

    #print('genpt', [round(p.pt(), 3) for p in event.gen_p4])
    # print('klm', [(v.x(), v.y(), v.z()) for v in event.pfjet_klmvtx])
    #print('kin', [(v.x(), v.y(), v.z()) for v in event.pfjet_kinvtx])
    #print('TOHLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx',
    #      [(v.pt(), v.eta(), v.phi(), v.energy()) for v in event.TOHLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx]
    #)
    # print('event ', i, 'jet', [round(j.pt(), 3) for j in event.pfjet_p4])
    if i>40: break
