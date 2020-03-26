#!/usr/bin/env python
"""GenXSecAnalyzer will output information about xsec, gen filter efficiency etc.
at the end of the job logs. This script will extract the filter efficiency numbers.
"""
from __future__ import print_function

import os
from os.path import join
import argparse

import yaml

parser = argparse.ArgumentParser(description="parse logs to retrieve gen filter efficiency.")
parser.add_argument("dir", type=str, nargs=1, help='condor job directory')
parser.add_argument("--output", "-o", default=None, type=str, help='saving to')
args = parser.parse_args()


def updateEfficiency(prev, curr):
    res = prev
    for k in curr:
        if k in res and float(curr[k].split('+-')[1])>float(prev[k].split('+-')[1]): continue
        res[k] = curr[k]
    return res


if __name__ == "__main__":

    logsdir = args.dir[0]
    assert (os.path.isdir(logsdir))
    logs = [f for f in os.listdir(logsdir) if f.endswith('.stdout')]

    results = {}
    for f in logs:
        jobname = f.split('.')[0]

        with open(join(logsdir, f)) as inf:
            for line in inf:
                if not line.startswith('Filter efficiency (event-level)'):
                    continue
                # Filter efficiency (event-level)= (28457) / (50000) = 5.691e-01 +- 2.215e-03    [TO BE USED IN MCM]
                info = line.split('=')[-1].split('[')[0]

                results[jobname] = info.strip()
                break

    for k in sorted(results):
        print('{:60}{}'.format(k, results[k]))

    # saving
    if args.output:
        print('-'*50)
        if os.path.isfile(args.output):
            prev = yaml.load(open(args.output), Loader=yaml.Loader)
            results = updateEfficiency(prev, results)
            with open(args.output, 'w') as f:
                f.write(yaml.dump(results, default_flow_style=False))
            print('Update to:', args.output)
        else:
            with open(args.output, 'w') as f:
                f.write(yaml.dump(results, default_flow_style=False))
            print('Saved to:', args.output)