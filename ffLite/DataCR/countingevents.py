#!/usr/bin/env python
from __future__ import print_function
import yaml
from DataFormats.FWLite import Events, Handle
import os
from os.path import join

configfile = join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/skimmed/bkgmc.yml')
config = yaml.load(open(configfile), Loader=yaml.Loader)
# print(len(config['datasetNames']))
# print(len(config['fileList']))

result = {}
for name, files in zip(config['datasetNames'], config['fileList']):
    result[name.split('/')[2]] = sum(Events('root://cmseos.fnal.gov//'+f).size() for f in files)
from pprint import pprint
pprint(result)