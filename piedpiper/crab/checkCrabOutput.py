#!/usr/bin/env python2.7
from __future__ import print_function
import os
import time
from datetime import datetime
from CRABAPI.RawCommand import crabCommand

CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "crabWorkArea")
VERBOSE = True
MOST_RECENT_DAYS = 3


def main():

    crabTaskList = [
        d
        for d in os.listdir(CRAB_WORK_DIR)
        if os.path.isdir("%s/%s" % (CRAB_WORK_DIR, d))
        and (
            datetime.now() - datetime.strptime(d.rsplit("_", 1)[-1], "%y%m%d-%H%M%S")
        ).days
        < MOST_RECENT_DAYS
    ]
    print("Total tasks to check: ", len(crabTaskList))

    task_completed, task_failed, task_others, task_exception = [], [], [], []

    with open("checkCrabOutput.log", "w") as of:
        of.write(time.asctime() + "\n")
        of.write("=" * 79 + "\n\n")

        for t in crabTaskList:

            try:
                statusDict = crabCommand("status", dir="crabWorkArea/{0}".format(t))

                localTaskDirectory = os.path.join(CRAB_WORK_DIR, t)
                of.write("directory:\t {0}\n".format(localTaskDirectory))
                of.write(
                    "task:\t {0}\n".format(
                        statusDict.get("userWebDirURL", "").split("/")[-1]
                    )
                )
                of.write(
                    "status:\t {0} {1}/{2}\n".format(
                        statusDict.get("status", ""),
                        statusDict.get("jobsPerStatus", {}).get("finished", 0),
                        sum(statusDict.get("jobsPerStatus", {}).values()),
                    )
                )
                of.write(
                    "publication:\t dataset: {0} | done: {1}\n".format(
                        statusDict.get("outdatasets", ""),
                        statusDict.get("publication", {}).get("done", 0),
                    )
                )
                of.write("\n")

                taskstatus = statusDict.get("status", "")
                taskStatusDictShort = {
                    "localDirectory": localTaskDirectory,
                    "task": statusDict.get("userWebDirURL", "").split("/")[-1],
                    "status": taskstatus,
                    "dataset": statusDict.get("outdatasets", ""),
                    "fraction": "{0}/{1}".format(
                        statusDict.get("jobsPerStatus", {}).get("finished", 0),
                        sum(statusDict.get("jobsPerStatus", {}).values()),
                    ),
                }
                if taskstatus.lower() == "completed":
                    task_completed.append(taskStatusDictShort)
                elif taskstatus.lower() == "failed":
                    task_failed.append(taskStatusDictShort)
                else:
                    task_others.append(taskStatusDictShort)
            except Exception as e:
                of.write("!!! ++++++++++++++++++++++++++++++++++++++++++++++++++ !!!\n")
                of.write(
                    "!!! directory:\t {0}\n".format(os.path.join(CRAB_WORK_DIR, t))
                )
                of.write("!!! crab error encounterd - MSG - {}\n".format(str(e)))
                of.write("!!! ++++++++++++++++++++++++++++++++++++++++++++++++++ !!!\n")
                of.write("\n")

                task_exception.append(os.path.join(CRAB_WORK_DIR, t))

        of.write("\n\n\n" + "*" * 79 + "\n\n\n")

        of.write("Completed tasks:\n===========================\n")
        for t in task_completed:
            of.write("directory: {}\n".format(t.get("localDirectory", "")))
            of.write("task: {}\n".format(t.get("task", "")))
            of.write("dataset: {}\n".format(t.get("dataset", "")))
            of.write("\n")

        of.write("-" * 79 + "\n\n")

        of.write("Other tasks:\n===========================\n")
        for t in task_others:
            of.write("directory: {}\n".format(t.get("localDirectory", "")))
            of.write("task: {}\n".format(t.get("task", "")))
            of.write("status: {}\n".format(t.get("status", "")))
            of.write("fraction: {}\n".format(t.get("fraction", "")))
            of.write("\n")

        of.write("-" * 79 + "\n\n")

        of.write("Failed tasks:\n===========================\n")
        for t in task_failed:
            of.write("directory: {}\n".format(t.get("localDirectory", "")))
            of.write("task: {}\n".format(t.get("task", "")))
            of.write("\n")

        of.write("-" * 79 + "\n\n")

        of.write("Exception tasks:\n===========================\n")
        for t in task_exception:
            of.write("directory: {}\n".format(t))
            of.write("\n")

        of.write("-" * 79 + "\n\n")


if __name__ == "__main__":
    main()
