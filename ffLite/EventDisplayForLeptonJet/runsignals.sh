#!/bin/bash

source $CMSSW_BASE/src/Firefighter/ffConfig/python/test/initfftest.sh

cmsRun $FFCFG config=cfg/ffSuperConfig_signal-2mu2e_mXX-1000_mA-0p25_lxy-0p3.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-2mu2e_mXX-1000_mA-0p25_lxy-300.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-2mu2e_mXX-100_mA-5_lxy-0p3.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-2mu2e_mXX-100_mA-5_lxy-300.yml keepskim=1&

cmsRun $FFCFG config=cfg/ffSuperConfig_signal-4mu_mXX-1000_mA-0p25_lxy-0p3.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-4mu_mXX-1000_mA-0p25_lxy-300.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-4mu_mXX-100_mA-5_lxy-0p3.yml keepskim=1&
cmsRun $FFCFG config=cfg/ffSuperConfig_signal-4mu_mXX-100_mA-5_lxy-300.yml keepskim=1&