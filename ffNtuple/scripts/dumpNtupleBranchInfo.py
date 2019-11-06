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
        _name = b.GetName()
        _title = "" if b.GetTitle().startswith(_name) else b.GetTitle()
        if b.ClassName()=='TBranch':
            _className = b.GetListOfLeaves()[0].GetTypeName()
            branchesInfo.append((_name, _className, _title))
        else:
            _className = b.GetClassName()
            _className = _className.replace('ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >', 'math::XYZTLorentzVectorF')
            _className = _className.replace('ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float>,ROOT::Math::DefaultCoordinateSystemTag>', 'math::XYZPointF')
            _className = _className.replace('ROOT::Math::DisplacementVector2D<ROOT::Math::Cartesian2D<float>,ROOT::Math::DefaultCoordinateSystemTag>', 'XYVectorF')
            branchesInfo.append((_name, _className, _title))

    ## save as an output
    with open(args.outfilename, 'w') as outf:
        if args.outfilename.endswith('.html'):
            outf.write('<html>\n')
            outf.write('<head>\n')
            outf.write('<title>Documentation for Firefighter Ntuple</title>\n')
            outf.write('<style>th, td {border-bottom: 1px solid black; padding: 1px 1em;}</style>\n')
            outf.write('</head>\n')

            outf.write("<body style='font-family: monospace;'><table style='margin: auto;'>\n")
            outf.write("<tr><th>Branch Name</th><th>Class Type</th><th>Description</th></tr>\n")
            for n, c, d in branchesInfo:
                outf.write("<tr><th style='text-align: left;'>{0}</th><td><pre>{1}</pre></td><td style='text-align: left;'>{2}</td></tr>\n".format(
                    n, c.replace('<', '&lt;').replace('>', '&gt;'), d))
            outf.write('</table></body>\n')
            outf.write('</html>\n')
        else:
            nameWidth = max([len(n) for n, c, d in branchesInfo])+10
            typeWidth = max([len(c) for n, c, d in branchesInfo])+10
            descWidth = max([len(d) for n, c, d in branchesInfo])
            dic = dict(nameWidth=nameWidth, typeWidth=typeWidth)

            outf.write('='*(nameWidth+typeWidth+descWidth)+'\n')
            outf.write('\n'.join(['{0:{nameWidth}}{1:{typeWidth}}{2}'.format(n, c, d, **dic) for n, c, d in branchesInfo]))
            outf.write('\n'+'='*(nameWidth+typeWidth+descWidth))
