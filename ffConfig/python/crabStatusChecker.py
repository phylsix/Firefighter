#!/usr/bin/env python
"""check each crab job's status under a submission folder
It uses a `crabcompleted.json` file under the submission folder to cache the
completed jobs to avoid unnecessary secondary query.

Usage: python crabStatusChecker.py <folder>
"""
from __future__ import print_function
import os
import json
from os.path import join, isdir, exists, basename
from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import setConsoleLogLevel
from CRABClient.ClientUtilities import LOGLEVEL_MUTE


def preparequerylist(submissiondir):
    totaljobdirs = []
    for root, dirs, files in os.walk(submissiondir):
        totaljobdirs.extend( [join(root, name) for name in dirs if exists(join(root, name, '.requestcache'))] )
    print("Number of jobs under {}: {}".format(submissiondir, len(totaljobdirs)))
    print(*totaljobdirs, sep='\n')
    completeddirs = []
    completejson = join(submissiondir, 'crabcompleted.json')
    if os.path.exists(completejson):
        completeddirs.extend(json.load(open(completejson)))
    return [d for d in totaljobdirs if basename(d) not in completeddirs]


def querystatus(submissiondir, toquerylist):
    setConsoleLogLevel(LOGLEVEL_MUTE)
    res = {}
    print("Making queries for {} jobs..".format(len(toquerylist)))
    for d in toquerylist:
        if not d.startswith(submissiondir):
            d = join(submissiondir, d)
        statusdict = crabCommand("status", dir=d)
        _status = statusdict.get("status", "")
        _jobsPerStatus = statusdict.get("jobsPerStatus", {})
        res[d] = {'status': _status, 'jobsPerStatus': _jobsPerStatus}
    return res


def updatecompletelist(submissiondir, queryresult):
    completeddirs = []
    completejson = join(submissiondir, 'crabcompleted.json')
    noncompletejson = join(submissiondir, 'crabnoncompleted.json')
    if os.path.exists(completejson):
        completeddirs.extend(json.load(open(completejson)))
    beforeupdate_ = len(completeddirs)
    noncompleted = {} # record status of those not completed
    for d in queryresult:
        if queryresult[d]['status'] == 'COMPLETED':
            completeddirs.append(basename(d))
        else:
            noncompleted[basename(d)] = queryresult[d]['status']
    print("Number of jobs updated to COMPLETED: {}".format(len(completeddirs)-beforeupdate_))
    with open(completejson, 'w') as outf:
        outf.write(json.dumps(completeddirs, indent=4))
    with open(noncompletejson, 'w') as outf:
        outf.write(json.dumps(noncompleted, indent=4))


def displaystatus(submissiondir, queryresult):
    bannermid = '###    {}    ###'.format(submissiondir)
    headtail = '#' * len(bannermid)
    print('\n'.join([headtail, bannermid, headtail]))

    completejson = join(submissiondir, 'crabcompleted.json')
    completeddirs = json.load(open(completejson))
    print('** COMPLETED **')
    print(*completeddirs, sep='\n')

    print('++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++')

    totalstatus = []
    for d in queryresult:
        if queryresult[d]['status'] == 'COMPLETED': continue
        if queryresult[d]['status'] in totalstatus: continue
        totalstatus.append(queryresult[d]['status'])

    for stat in totalstatus:
        print("** {} **".format(stat))
        for d in queryresult:
            if queryresult[d]['status'] != stat: continue
            print("{:120}    {}".format(d, str(queryresult[d]['jobsPerStatus'])))
        print()


def printcrabresubcmd(submissiondir, queryresult):
    jobnames = [d for d in queryresult if queryresult[d]['status']=='FAILED']
    cmds = []
    for d in jobnames:
        if d.startswith(submissiondir):
            cmds.append('crab resubmit -d {}'.format(d))
        else:
            cmds.append('crab resubmit -d {}'.format(join(submissiondir, d)))
    if cmds:
        print(" To submit failed jobs ".center(79, "*"))
        print(*cmds, sep="\n")



if __name__ == "__main__":

    import sys
    subdir = sys.argv[1]
    if not os.path.isdir(subdir):
        sys.exit("{} is not a submission directory! Exit..".format(subdir))

    qlist = preparequerylist(subdir)
    qresults = querystatus(subdir, qlist)
    updatecompletelist(subdir, qresults)
    displaystatus(subdir, qresults)
    printcrabresubcmd(subdir, qresults)
