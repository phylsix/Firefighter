import FWCore.ParameterSet.Config as cms
import math

leptonjetpairfilter = cms.EDFilter(
    "LeptonJetPairFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minDPhi=cms.double(math.pi/2),
)
