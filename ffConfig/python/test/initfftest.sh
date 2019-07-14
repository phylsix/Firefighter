#!/bin/bash

FFCFG=$CMSSW_BASE/src/Firefighter/ffConfig/cfg/ffNtupleFromAOD_cfg.py
TESTCONFIGBASE=$CMSSW_BASE/src/Firefighter/ffConfig/python/test

alias fftestsig4mu='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_signal-4mu.yml keepskim=1'
alias fftestsig2mu2e='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_signal-4mu.yml keepskim=1'
alias fftestzz='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_ZZ.yml'
alias fftestqcd='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_QCDpT300To470.yml'
alias fftestdy='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DYJetsM50.yml'
alias fftestdatac='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018C.yml'
alias fftestdatad='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018D.yml'
