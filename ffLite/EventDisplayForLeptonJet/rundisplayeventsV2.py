#!/usr/bin/python

import subprocess
import yaml

if __name__ == "__main__":

    samples = yaml.load(open('rundisplayeventsV2.yml'), Loader=yaml.SafeLoader)
    for d in samples:
        subprocess.call("python displayeventsV2.py {} &".format(d).split())