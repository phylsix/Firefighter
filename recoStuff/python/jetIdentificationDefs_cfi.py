import FWCore.ParameterSet.Config as cms
import os

jetiddefs = cms.string('pt>0.')

if os.getenv("CMSSW_VERSION").startswith("CMSSW_10_2"):
    # https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2018
    jetiddefs = cms.string(" || ".join([
        "({})".format(" && ".join([
            "abs(eta)<2.6",
            "neutralHadronEnergyFraction<0.9",
            "neutralEmEnergyFraction<0.9",
            "(chargedMultiplicity+neutralMultiplicity)>1",
            "muonEnergyFraction<0.8",
            "chargedHadronEnergyFraction>0",
            "chargedMultiplicity>0",
            "chargedEmEnergyFraction<0.8",
        ])),
        "({})".format(" && ".join([
            "abs(eta)>2.6 && abs(eta)<=2.7",
            "neutralHadronEnergyFraction<0.9",
            "neutralEmEnergyFraction<0.99",
            "muonEnergyFraction<0.8",
            "chargedMultiplicity>0",
            "chargedEmEnergyFraction<0.8",
        ])),
        "({})".format(" && ".join([
            "abs(eta)>2.7 && abs(eta)<=3.0",
            "neutralEmEnergyFraction>0.02",
            "neutralEmEnergyFraction<0.99",
            "neutralMultiplicity>2",
        ])),
        "({})".format(" && ".join([
            "abs(eta)>3.0",
            "neutralHadronEnergyFraction>0.2",
            "neutralEmEnergyFraction<0.9",
            "neutralMultiplicity>10",
        ])),
        # "( abs(eta)<2.6 && chargedEmEnergyFraction<0.8 && chargedMultiplicity>0 && chargedHadronEnergyFraction>0 && (chargedMultiplicity+neutralMultiplicity)>1 && neutralEmEnergyFraction<0.9 && muonEnergyFraction<0.8 && neutralHadronEnergyFraction<0.9)",
        # "( abs(eta)>2.6 && abs(eta)<=2.7 && chargedEmEnergyFraction<0.8 && chargedMultiplicity>0 && neutralEmEnergyFraction<0.99 && muonEnergyFraction<0.8 && neutralHadronEnergyFraction<0.9 )",
        # "( neutralEmEnergyFraction>0.02 && neutralEmEnergyFraction<0.99 && neutralMultiplicity>2 && abs(eta)>2.7 && abs(eta)<=3.0 )",
        # "(neutralEmEnergyFraction<0.90 && neutralHadronEnergyFraction>0.2 && neutralMultiplicity>10 && abs(eta)>3.0 )"
    ]))