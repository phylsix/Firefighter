import FWCore.ParameterSet.Config as cms
from Firefighter.ffConfig.dataSample import skimmedSamples as samples
import sys

dataType = "signal-4mu"
TEST_FAST = True


_event_runover = -1
_report_every = 1000
try:
    _data_runover = (
        [samples[dataType]] if isinstance(samples[dataType], str) else samples[dataType]
    )
    _data_runover = map(
        lambda f: f if f.startswith("root://") else "file:" + f, _data_runover
    )
    _output_fname = "ffLite_{}.root".format(dataType)
except KeyError:
    sys.exit(
        "Sample '{}' not available! choose from {}".format(dataType, samples.keys())
    )

if TEST_FAST:
    _event_runover = 10
    _report_every = 1
    _data_runover = [_data_runover[0]]

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames=cms.vstring(*_data_runover),
    maxEvents=cms.int32(_event_runover),
    outputEvery=cms.uint32(_report_every),
)
process.fwliteOutput = cms.PSet(fileName=cms.string(_output_fname))

process.subjetResults = cms.PSet()
