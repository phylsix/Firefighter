import FWCore.ParameterSet.Config as cms

# https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X
# https://cmssdt.cern.ch/dxr/CMSSW/source/RecoBTag/TensorFlow/python/pfDeepFlavour_cff.py

from RecoBTag.ImpactParameter.pfImpactParameterTagInfos_cfi import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.Combined.pfDeepCSVTagInfos_cfi import *
from RecoBTag.TensorFlow.pfDeepFlavour_cff import *


pfTagInfosForDeepFlavourTask = cms.Task(pfImpactParameterTagInfos, pfInclusiveSecondaryVertexFinderTagInfos, pfDeepCSVTagInfos)
ffDeepFlavourSeq = cms.Sequence(pfTagInfosForDeepFlavourTask, pfDeepFlavourTask)