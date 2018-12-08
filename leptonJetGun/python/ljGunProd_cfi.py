import FWCore.ParameterSet.Config as cms

generator = cms.EDProducer(
    "ljGunProd",
    PGunParameters = cms.PSet(
        PartID = cms.vint32(-13),
        MinPt  = cms.double(20.00),
        MaxPt  = cms.double(120.00),
        MinEta = cms.double(-2.4),
        MaxEta = cms.double(2.4),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
    ),
    Verbosity = cms.untracked.int32(0), ## set to 1 (or greater)  for printouts
    AddAntiParticle = cms.bool(True),

    LvBeta = cms.double(300.), #cm
    BoundEta = cms.double(2.4),
    BoundR   = cms.double(740), #cm
    BoundZ   = cms.double(960), #cm
    MomMass  = cms.double(1.2), # GeV
    MomPid   = cms.int32(32),
)
