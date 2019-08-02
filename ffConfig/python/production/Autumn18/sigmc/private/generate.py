#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""

import json
import shlex
import subprocess

pyfrag = """\
#!/usr/bin/env python

ffDataSet = {
    'datasetNames': %s,
    'maxEvents': -1,
    'globalTag': '102X_upgrade2018_realistic_v19',
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

datasources2 = """\
/SIDM_XXTo2ATo2Mu2E/wsi-mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875_AODSIM_2018-889dc1528a7a51924818b24d55ffac27/USER
/SIDM_XXTo2ATo4Mu/wsi-mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875_AODSIM_2018-889dc1528a7a51924818b24d55ffac27/USER
/SIDM_XXTo2ATo4Mu/wsi-mXX-100_mA-5_lxy-0p3_ctau-0p375_AODSIM_2018-889dc1528a7a51924818b24d55ffac27/USER
/SIDM_XXTo2ATo2Mu2E/wsi-mXX-100_mA-5_lxy-0p3_ctau-0p375_AODSIM_2018-889dc1528a7a51924818b24d55ffac27/USER"""

datasets = {}

for d in datasources.split():
    dtag = d.split("/")[2]
    dtag = dtag.rsplit('-', 1)[0].split('_', 1)[-1]\
        .replace('MBs', 'mXX')\
        .replace('MDp', 'mA')\
        .replace('Bs', 'XX')\
        .replace('Dp', 'A')\
        .replace('e', 'E')  # XXTo2ATo2Mu2E_mXX-100_mA-0p8_ctau-60
    dtag = dtag.rsplit("_", 1)
    dtag = "_".join([dtag[0], "lxy-300", dtag[-1]]) # XXTo2ATo2Mu2E_mXX-100_mA-0p8_lxy-300_ctau-60
    if dtag in datasets:
        datasets[dtag].append(d)
    else:
        datasets[dtag] = [d]

for d in datasources2.split():
    pd = d.split("/")[1]
    taginfo = d.split("/")[2]
    proc = pd.split('_')[-1]  # XXTo2ATo2Mu2E
    taginfo = taginfo.rsplit('-', 1)[0].split('-', 1)[-1]  # mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875_AODSIM_2018
    taginfo = "_".join(taginfo.split("_")[:-2])  # mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875
    dtag = "_".join([proc, taginfo])
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
        flist.append(sorted(subflist))

    with open(pyfragName, "w") as f:
        f.write(pyfrag % (str(datasets[dtag]), str(flist)))

# summary
with open('description.json', 'w') as f:
    f.write(json.dumps([
        'Firefighter.ffConfig.production.Autumn18.sigmc.private.{0}'.format(dtag)
        for dtag in datasets
    ], indent=4))
