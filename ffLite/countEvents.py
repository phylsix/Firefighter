#!/usr/bin/env python
from __future__ import print_function
import sys
from DataFormats.FWLite import Events, Handle

if __name__ == "__main__":

    fn = sys.argv[1]
    events = Events(fn)

    totalnum = 0
    for e in events:
        totalnum += 1

    print("{:10} {}".format(totalnum, fn))