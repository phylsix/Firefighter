#!/usr/bin/env python
from __future__ import print_function
import os
import time
from datetime import datetime
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

MOST_RECENT_DAYS = 3


def main():

    toResub = [
        "crabWorkArea/{0}".format(d)
        for d in os.listdir("crabWorkArea")
        if "ffNtuple" in d
        and (
            datetime.now() - datetime.strptime(d.rsplit("_", 1)[-1], "%y%m%d-%H%M%S")
        ).days
        < MOST_RECENT_DAYS
    ]

    for t in toResub:

        relDir = os.path.relpath(os.path.abspath(t), ".")
        statusDict = dict()
        try:
            statusDict = crabCommand("status", dir=relDir)
        except Exception as e:
            print(str(e))
            pass
        if statusDict.get("jobsPerStatus", {}).get("failed", 0) == 0:
            continue

        print("-" * 79)
        print(os.path.abspath(t))
        try:
            crabCommand("resubmit", dir=relDir)
        except HTTPException as hte:
            print("Failed to resubmit for task {0}: {1}".format(relDir, hte.headers))
        except ClientException as cle:
            print("Failed to resubmit for task {0}: {1}".format(relDir, cle))
        print("-" * 79)


if __name__ == "__main__":
    main()
