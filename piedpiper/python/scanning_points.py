#!/usr/bin/env python

points = [
    dict(
        name="SIDM_XXTo2ATo4Mu_mXX-100_mA-0p25_ctau-0p02_lxy-3",
        weight=1.0,
        gridpack="../gridpack.tar.xz",
        procParam=[
            "ParticleDecays:tau0Max = 1000.1",
            "LesHouches:setLifetime = 2",
            "32:tau0 = 0.02",
        ],
    ),
    dict(
        name="SIDM_XXTo2ATo4Mu_mXX-100_mA-0p25_ctau-0p2_lxy-30",
        weight=1.0,
        gridpack="../gridpack.tar.xz",
        procParam=[
            "ParticleDecays:tau0Max = 1000.1",
            "LesHouches:setLifetime = 2",
            "32:tau0 = 0.2",
        ],
    ),
]
