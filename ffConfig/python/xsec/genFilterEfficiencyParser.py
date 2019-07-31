#!/usr/bin/env python
"""GenXSecAnalyzer will output information about xsec, gen filter efficiency etc.
at the end of the job logs. This script will extract the filter efficiency numbers.
"""

import os
from os.path import join
import sys


if __name__ == "__main__":

    logsdir = sys.argv[1]
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