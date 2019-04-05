#!/usr/bin/env python
from __future__ import print_function
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime

plt.style.use("default")
plt.rcParams["savefig.dpi"] = 200
plt.rcParams["grid.linestyle"] = ":"
plt.rcParams["savefig.bbox"] = "tight"
plt.rcParams["axes.titleweight"] = "semibold"
plt.rcParams["font.family"] = ["Ubuntu", "sans-serif"]

CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "crabWorkArea")
OUTPUT_FILE = "/publicweb/w/wsi/public/lpcdm/crabworkareastat.png"


def main():

    jobDirs = [
        d
        for d in os.listdir(CRAB_WORK_DIR)
        if os.path.isdir(os.path.join(CRAB_WORK_DIR, d))
    ]
    timestamps = [
        datetime.strptime(d.rsplit("_", 1)[-1], "%y%m%d-%H%M%S") for d in jobDirs
    ]

    jobDf = pd.DataFrame({"jobs": timestamps, "count": [1] * len(timestamps)})
    jobDf.index = pd.to_datetime(jobDf["jobs"])
    grouped = jobDf["count"].resample("D").sum()
    print(grouped.tail())

    fig, ax = plt.subplots(figsize=(max(8, len(grouped) * 0.2), 6))
    ax.plot(
        grouped.index,
        grouped.values,
        drawstyle="steps-mid",
        color="#1f78b4",
        linewidth="1.5",
    )
    ax.set_title("jobs under: " + CRAB_WORK_DIR)
    ax.grid()
    ax.set_xlabel("Date")
    ax.set_ylabel("#jobs")
    plt.xticks(rotation=75, fontsize=9)
    ax.xaxis.set_major_locator(mdates.DayLocator())
    ax.xaxis.set_major_formatter(mdates.DateFormatter("%b%d"))
    fig.savefig(OUTPUT_FILE)

    print("plot saved at: ", OUTPUT_FILE)
    print("Please visit\n\thttp://home.fnal.gov/~wsi/public/lpcdm/crabworkareastat.png")


if __name__ == "__main__":
    main()
