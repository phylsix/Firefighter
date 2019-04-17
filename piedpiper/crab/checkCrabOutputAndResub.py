#!/usr/bin/env python2.7
from __future__ import print_function
import os
import sys
import time
import sqlite3
from datetime import datetime
from multiprocessing.pool import ThreadPool
from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import setConsoleLogLevel
from CRABClient.ClientUtilities import LOGLEVEL_MUTE
from CRABClient.UserUtilities import getLoggers

CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "crabWorkArea")
JOB_STATUS_DB = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "crabjobsStatus.sqlite"
)
LOGSHEET = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "crabjobsCheckAndResubReport.log"
)
MOST_RECENT_DAYS = 2
if len(sys.argv) > 1:
    try:
        MOST_RECENT_DAYS = int(sys.argv[1])
    except:
        print("MOST_RECENT_DAYS need to be an integer!")
        raise
ASYNC_CHECK = (
    False
)  # crab relies on local ~/.crab3, which is the main reason of exception


def checkSingleTask(crabTaskDir):
    """ checking a status of a given crab task directory, returns a dict. """

    res = {}
    if not os.path.isdir(crabTaskDir):
        return res
    res["directory"] = crabTaskDir

    try:
        try:
            os.remove(os.path.join(os.environ["HOME"], ".crab3"))
        except:
            pass
        statusDict = dict()
        statusDict = crabCommand("status", dir=crabTaskDir)
        _userWebDirURL = statusDict.get("userWebDirURL")
        _task = _userWebDirURL.split("/")[-1] if _userWebDirURL else "NOT ASSIGNED"
        _status = statusDict.get("status", "").lower()
        _jobsPerStatus = statusDict.get("jobsPerStatus", {})
        _outDatasets = statusDict.get("outdatasets", "")
        _publication = statusDict.get("publication", {})

        res.update(
            {
                "task": _task,
                "status": _status,
                "jobsperstatus": _jobsPerStatus,
                "outdatasets": _outDatasets,
                "publication": _publication,
            }
        )
    except Exception as e:
        res.update({"exception": True, "msg": str(e), "queryResult": statusDict})

    return res


def resubmitSingleTask(checkdict):
    """ resubmit failed job given an output fo `checkSingleTask` """

    res = {}

    _dir = checkdict.get("directory", None)
    _status = checkdict.get("status", None)
    if _dir is None or _status is None or _status == "completed":
        return res
    res["directory"] = _dir
    res["success"] = True

    if checkdict.get("publication", {}).get("failed", 0) != 0:
        try:
            crabCommand("resubmit", dir=_dir, publication=True)
        except Exception as e:
            res["exceptionMsg"] = str(e)
            res["success"] = False

    if _status == "failed" or (
        checkdict.get("jobsperstatus", {}).get("failed", 0) != 0
        and checkdict.get("jobsperstatus", {}).get("killed", 0) == 0
    ):
        try:
            crabCommand("resubmit", dir=_dir)
        except Exception as e:
            res["exceptionMsg"] = str(e)
            res["success"] = False
    else:
        return {}

    return res


def main():

    sql_table_creation = """CREATE TABLE IF NOT EXISTS crabJobStatuses (
        directory TEXT PRIMARY KEY,
        status TEXT,
        dataset TEXT
    );"""
    conn = sqlite3.connect(JOB_STATUS_DB)
    crabTaskListCompleted = []
    with conn:
        c = conn.cursor()
        c.execute(sql_table_creation)
        for row in c.execute("SELECT * FROM crabJobStatuses WHERE status='completed'"):
            crabTaskListCompleted.append(row[0])

    crabTaskList = [
        os.path.join(CRAB_WORK_DIR, d)
        for d in os.listdir(CRAB_WORK_DIR)
        if os.path.isdir("%s/%s" % (CRAB_WORK_DIR, d))
        and (
            datetime.now() - datetime.strptime(d.rsplit("_", 1)[-1], "%y%m%d-%H%M%S")
        ).days
        < MOST_RECENT_DAYS
    ]
    crabTaskListToCheck = [t for t in crabTaskList if t not in crabTaskListCompleted]
    print(
        "Checking tasks submmited for most recent {} day(s), total tasks to check: {}. Checking...".format(
            MOST_RECENT_DAYS, len(crabTaskListToCheck)
        )
    )

    setConsoleLogLevel(LOGLEVEL_MUTE)
    crabLoggers = getLoggers()

    crabTaskStatuses = []
    if ASYNC_CHECK:
        p = ThreadPool()
        r = p.map_async(
            checkSingleTask, crabTaskListToCheck, callback=crabTaskStatuses.extend
        )
        r.wait()
        p.close()
    else:
        crabTaskStatuses = [checkSingleTask(d) for d in crabTaskListToCheck]

    task_completed = []
    task_failed = []
    task_submitfailed = []
    task_others = []
    task_exception = []

    for d in crabTaskStatuses:
        if d.get("exception", False):
            task_exception.append(d)
        else:
            _status = d.get("status", None)
            if _status == "completed":
                task_completed.append(d)
            elif _status == "failed":
                task_failed.append(d)
            elif _status == "submitfailed":
                task_submitfailed.append(d)
            else:
                task_others.append(d)

    # updating local db
    conn = sqlite3.connect(JOB_STATUS_DB)
    with conn:
        c = conn.cursor()
        set_complete = [
            (t["directory"], "completed", t["outdatasets"]) for t in task_completed
        ]
        c.executemany(
            "INSERT OR REPLACE INTO crabJobStatuses VALUES (?,?,?)", set_complete
        )
        set_noncomplete = [
            (t["directory"], "failed", "") for t in task_failed + task_others
        ]
        exceptedTasks = [
            (t["directory"], "failed", "")
            for t in task_exception
            if ".requestcache" not in t["msg"]
        ]
        if exceptedTasks:
            print("Number of tasks excepted when querying: ", len(exceptedTasks))
        set_noncomplete.extend(exceptedTasks)
        c.executemany(
            "INSERT OR REPLACE INTO crabJobStatuses VALUES (?,?,?)", set_noncomplete
        )

    print("Trying to resubmit {} task(s) ...".format(len(task_failed+task_others)))
    crabResubmitResult = []
    p = ThreadPool()
    r = p.map_async(resubmitSingleTask, task_failed + task_others, callback=crabResubmitResult.extend)
    r.wait()
    p.close()

    resubmittedTasks = [t for t in crabResubmitResult if t]
    resubTaskSuccess, resubTaskFail = [], []
    for t in resubmittedTasks:
        if t.get("success", False):
            resubTaskSuccess.append(t)
        else:
            resubTaskFail.append(t)

    print("Successfully resubmit {} task(s), !Yay!".format(len(resubTaskSuccess)))
    print("Writing Check&Resub result to:\n\t", LOGSHEET)
    with open(LOGSHEET, "w") as of:
        of.write("Generated at " + time.asctime() + "\n")
        of.write("=" * 79 + "\n\n")

        if task_completed:
            of.write("Completed tasks: [{}]\n".format(len(task_completed)))
            of.write("===========================\n")

            for t in task_completed:
                toprint = "directory: {0}\ntask: {1}\ndataset: {2}\n\n".format(
                    t["directory"], t["task"], t["outdatasets"]
                )
                of.write(toprint)

            of.write("-" * 79 + "\n\n")

        if task_others:
            of.write("Other tasks: [{}]\n".format(len(task_others)))
            of.write("===========================\n")

            for t in task_others:
                toprint = "directory: {0}\ntask: {1}\nstatus: {2}\njobsPerStatus: {3}\npublication: {4}\n\n".format(
                    t["directory"],
                    t["task"],
                    t["status"],
                    str(t["jobsperstatus"]),
                    str(t["publication"]),
                )
                of.write(toprint)

            of.write("-" * 79 + "\n\n")

        if task_failed:
            of.write("Failed tasks: [{}]\n".format(len(task_failed)))
            of.write("===========================\n")

            for t in task_failed:
                toprint = "directory: {0}\ntask: {1}\njobsPerStatus: {2}\npublication: {3}\n\n".format(
                    t["directory"],
                    t["task"],
                    str(t["jobsperstatus"]),
                    str(t["publication"]),
                )
                of.write(toprint)

            of.write("-" * 79 + "\n\n")

        if task_submitfailed:
            of.write("Submitfailed tasks: [{}]\n".format(len(task_submitfailed)))
            of.write("===========================\n")
            print("Follwoing tasks are failed to submit:\n")
            for t in task_submitfailed:
                towrite = "directory: {}\n".format(t["directory"])
                of.write(towrite)
                print("crab resubmit -d {}".format(t["directory"]))
            of.write("-" * 79 + "\n\n")

        if task_exception:
            of.write("Exception tasks [{}]:\n".format(len(task_exception)))
            of.write("===========================\n")
            print("Resubmit manually for the following:")
            for t in task_exception:
                toprint = "directory: {0}\nmessage: {1}\nqueryResult: {2}\n\n".format(
                    t["directory"], t["msg"], t["queryResult"]
                )
                of.write(toprint)
                print("crab resubmit -d {}".format(t["directory"]))

            of.write("+" * 79 + "\n\n")

        if resubTaskSuccess:
            of.write(
                "Successfully resubmitted tasks: [{}]\n".format(len(resubTaskSuccess))
            )
            of.write("====================================\n")
            for d in resubTaskSuccess:
                of.write(d["directory"] + "\n")
            of.write("-" * 79 + "\n\n")

        if resubTaskFail:
            of.write("Failed resubmitted tasks: [{}]\n".format(len(resubTaskFail)))
            of.write("==============================\n")
            for d in resubTaskFail:
                of.write(d["directory"] + "\n")
                of.write(d["exceptionMsg"] + "\n\n")
            of.write("-" * 79 + "\n\n")


if __name__ == "__main__":
    main()
