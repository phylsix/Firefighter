#!/bin/bash

FFCFG=$CMSSW_BASE/src/Firefighter/ffConfig/cfg/ffNtupleFromAOD_cfg.py
FFCFG2=$CMSSW_BASE/src/Firefighter/ffConfig/cfg/ffNtupleFromAOD_v2_cfg.py
TESTCONFIGBASE=$CMSSW_BASE/src/Firefighter/ffConfig/python/test

alias fftestsig4mu='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_signal-4mu.yml keepskim=1'
alias fftestsig2mu2e='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_signal-2mu2e.yml keepskim=1'
alias fftestzz='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_ZZ.yml'
alias fftestwww='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_WWW.yml'
alias fftestqcd='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_QCD.yml'
alias fftestdy='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DYJetsM50.yml'
alias fftestdatac='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018C.yml'
alias fftestdatad='cmsRun $FFCFG config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018D.yml'

alias ffv2testsig4mu='cmsRun $FFCFG2 config=$TESTCONFIGBASE/ffSuperConfig_signal-4mu.yml keepskim=1'
alias ffv2testsig2mu2e='cmsRun $FFCFG2 config=$TESTCONFIGBASE/ffSuperConfig_signal-2mu2e.yml keepskim=1'
alias ffv2testqcd='cmsRun $FFCFG2 config=$TESTCONFIGBASE/ffSuperConfig_QCD.yml'
alias ffv2testdatac='cmsRun $FFCFG2 config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018C.yml'
alias ffv2testdatad='cmsRun $FFCFG2 config=$TESTCONFIGBASE/ffSuperConfig_DoubleMuon2018D.yml'