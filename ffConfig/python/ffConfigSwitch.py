#!/usr/bin/env python
from __future__ import print_function
import json

# controls all switches

switches = {
    "jobtype": "bkgmc",  # {'data_abc', 'data_d', 'sigmc', 'bkgmc'}
    "region": "all",  # {'all', 'signal', 'control', 'single'}
    "recoStuff": {"leptonCandOnly": True, "usingCHS": False},
}


assert switches["jobtype"] in ["data_abc", "data_d", "sigmc", "bkgmc"]
assert switches["region"] in ["all", "signal", "control", "single"]
assert (
    switches["recoStuff"]["leptonCandOnly"]
    and switches["recoStuff"]["usingCHS"] is False
)

print("######################################################################")
print("############     Firefighter.ffConfig.ffConfigSwitch      ############")
print("######################################################################")
print()
print(json.dumps(switches, indent=4))
print()
print("######################################################################")
