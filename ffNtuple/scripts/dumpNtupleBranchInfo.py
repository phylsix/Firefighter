#!/usr/bin/env python
from __future__ import print_function

import argparse
import os

import ROOT

ROOT.gROOT.SetBatch()

parser = argparse.ArgumentParser(description="dump branch info of a ROOT TTree")
parser.add_argument("--input", "-i", type=str)
parser.add_argument("--outfilename", "-o", type=str)
args = parser.parse_args()

TTREE_PATH = 'ffNtuplizer/ffNtuple'


if __name__ == "__main__":
    assert(os.path.isfile(args.input))
    try:
        f = ROOT.TFile.Open(args.input)
    except:
        print('Unable to open input file "{}"'.format(args.input))

    ## scan branches
    branchesInfo = []
    for b in f.Get(TTREE_PATH).GetListOfBranches():
        if b.ClassName()=='TBranch':
            branchesInfo.append((b.GetName(), b.GetListOfLeaves()[0].GetTypeName()))
        else:
            _className = b.GetClassName()
            _className = _className.replace('ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >', 'math::XYZTLorentzVectorF')
            _className = _className.replace('ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float>,ROOT::Math::DefaultCoordinateSystemTag>', 'math::XYZPointF')
            branchesInfo.append((b.GetName(), _className))

    ## save as an output
    with open(args.outfilename, 'w') as outf:
        nameWidth = max([len(n) for n, c in branchesInfo])+10
        typeWidth = max([len(c) for n, c in branchesInfo])
        dic = dict(nameWidth=nameWidth, typeWidth=typeWidth)
        outf.write('='*(nameWidth+typeWidth)+'\n')
        outf.write('\n'.join(['{0:{nameWidth}}{1:{typeWidth}}'.format(n, c, **dic) for n, c in branchesInfo]))
        outf.write('\n'+'='*(nameWidth+typeWidth))
