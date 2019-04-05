#!/usr/bin/env python
from __future__ import print_function
import os
from datetime import datetime

CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "crabWorkArea")
MOST_RECENT_DAYS = 5


def main():

    jobDirs = [
        d
        for d in os.listdir(CRAB_WORK_DIR)
        if os.path.isdir(os.path.join(CRAB_WORK_DIR, d))
    ]

    willremove = (
        lambda d: (
            datetime.now() - datetime.strptime(d.rsplit("_", 1)[-1], "%y%m%d-%H%M%S")
        ).days
        > MOST_RECENT_DAYS
    )
    jobToRemove = list(filter(willremove, jobDirs))

    print(
        "Following crab directories need to remove, because they were created >{} days ago.\n".format(
            MOST_RECENT_DAYS
        )
    )
    print(
        *["rm -r {}".format(os.path.join(CRAB_WORK_DIR, d)) for d in jobToRemove],
        sep="\n"
    )


if __name__ == "__main__":
    main()
