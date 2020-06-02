#!/bin/bash
for i in 204 263 266 294 298 303 317 588 591 619 688 765 79 853 96 999
do
    cmsRun particleListDrawer_cfg.py inputFiles=file:../../ffLite/DYJets/data/lowM/ffAOD_$i.root | tee listdrawer_$i.log&
done