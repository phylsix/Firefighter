#!/usr/bin/env python
import shlex
import subprocess
import json

pyfrag = """\
#!/usr/bin/env python

ffDataSet = {
    'datasetNames': %s,
    'maxEvents': -1,
    'globalTag': '102X_upgrade2018_realistic_v18',
    'fileList': %s,
}
"""

datasources = """\
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-100_MDp-0p25_ctau-18p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-100_MDp-0p8_ctau-60-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-100_MDp-1p2_ctau-90-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-100_MDp-2p5_ctau-187p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-100_MDp-5_ctau-375-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-150_MDp-0p25_ctau-12p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-150_MDp-0p8_ctau-40-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-150_MDp-1p2_ctau-60-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-150_MDp-2p5_ctau-125-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-0p25_ctau-9p375-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-0p8_ctau-30-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-45-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-2p5_ctau-93p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-5_ctau-187p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-500_MDp-0p25_ctau-3p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-500_MDp-0p8_ctau-12-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-500_MDp-1p2_ctau-18-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-500_MDp-2p5_ctau-37p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-500_MDp-5_ctau-75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-800_MDp-0p25_ctau-2p344-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-800_MDp-0p8_ctau-7p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-800_MDp-1p2_ctau-11p25-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-800_MDp-2p5_ctau-23p438-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-800_MDp-5_ctau-46p875-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-0p25_ctau-1p875-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-0p8_ctau-6-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-1p2_ctau-9-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-2p5_ctau-18p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-1000_MDp-5_ctau-37p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-100_MDp-0p25_ctau-18p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-100_MDp-0p8_ctau-60-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-100_MDp-1p2_ctau-90-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-100_MDp-2p5_ctau-187p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-100_MDp-5_ctau-375-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-0p25_ctau-12p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-0p8_ctau-40-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-1p2_ctau-60-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-2p5_ctau-125-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-150_MDp-5_ctau-250-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-200_MDp-0p25_ctau-9p375-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-200_MDp-0p8_ctau-30-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-200_MDp-1p2_ctau-45-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-200_MDp-2p5_ctau-93p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-200_MDp-5_ctau-187p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-500_MDp-0p25_ctau-3p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-500_MDp-0p8_ctau-12-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-500_MDp-1p2_ctau-18-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-500_MDp-2p5_ctau-37p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-500_MDp-5_ctau-75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-800_MDp-0p25_ctau-2p344-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-800_MDp-0p8_ctau-7p5-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-800_MDp-1p2_ctau-11p25-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-800_MDp-2p5_ctau-23p438-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-800_MDp-5_ctau-46p875-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-0p25_ctau-1p875-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-0p8_ctau-6-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-1p2_ctau-9-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-2p5_ctau-18p75-7c74ac161ee1f5c5534fed7a9685e204/USER
/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-5_ctau-37p5-7c74ac161ee1f5c5534fed7a9685e204/USER"""


datasets = {}

for d in datasources.split():
    dtag = d.split("/")[2]
    dtag = dtag.rsplit('-',1)[0].split('_',1)[-1].replace('MBs', 'mXX').replace('MDp', 'mA').replace('Bs', 'XX').replace('Dp', 'A').replace('e', 'E')
    if dtag in datasets:
        datasets[dtag].append(d)
    else:
        datasets[dtag] = [d]

for dtag in datasets:
    pyfragName = dtag + ".py"
    flist = []
    for ds in datasets[dtag]:
        subflist = subprocess.check_output(
            shlex.split('dasgoclient -query="file dataset={0} instance=prod/phys03"'.format(ds))
        ).split()
        subflist = [f for f in subflist if f]
        flist.append(subflist)

    with open(pyfragName, "w") as f:
        f.write(pyfrag % (str(datasets[dtag]), str(flist)))

# summary
with open('description.json', 'w') as f:
    f.write(json.dumps(['Firefighter.ffConfig.production.Autumn18.sigmc.private.{0}'.format(dtag) for dtag in datasets]))
