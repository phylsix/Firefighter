#!/usr/bin/env python2.7
from __future__ import print_function
import os
import time
from datetime import datetime
from multiprocessing.pool import ThreadPool
from CRABAPI.RawCommand import crabCommand


CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'crabWorkArea')
VERBOSE = True
MOST_RECENT_DAYS = 3

def checkSingleTask(crabTaskDir):
    ''' checking a status of a given crab task directory, returns a dict. '''

    res = {}
    if not os.path.isdir(crabTaskDir):
        return res
    res['directory'] = crabTaskDir

    try:
        statusDict = crabCommand('status', dir=crabTaskDir)
        _task = statusDict.get('userWebDirURL', '').split('/')[-1]
        _status = statusDict.get('status', '').lower()
        _jobsPerStatus = statusDict.get('jobsPerStatus', {})
        _outDatasets = statusDict.get('outdatasets', '')
        _publication = statusDict.get('publication', {})

        res.update({
            'task' : _task,
            'status' : _status,
            'jobsperstatus' : _jobsPerStatus,
            'outdatasets' : _outDatasets,
            'publication' : _publication,
        })
    except Exception as e:
        res.update({
            'exception' : True,
            'msg' : str(e)
        })

    return res


def resubmitSingleTask(checkdict):
    ''' resubmit failed job given an output fo `checkSingleTask` '''

    res = {}

    _dir = checkdict.get('directory', None)
    _status = checkdict.get('status', None)
    if _dir is None or _status is None or _status=='completed': return res
    res['directory'] = _dir
    res['success'] = True

    if checkdict.get('publication', {}).get('failed', 0) != 0:
        try:
            crabCommand('resubmit', dir=_dir, publication=True)
        except Exception as e:
            res['exceptionMsg'] = str(e)
            res['success'] = False

    if _status == 'failed' or checkdict.get('jobsPerStatus',{}).get('failed',0) != 0:
        try:
            crabCommand('resubmit', dir=_dir)
        except Exception as e:
            res['exceptionMsg'] = str(e)
            res['success'] = False
    else:
        return {}

    return res





def main():

    crabTaskList = [
            os.path.join(CRAB_WORK_DIR, d) for d in os.listdir(CRAB_WORK_DIR) \
                    if os.path.isdir('%s/%s' % (CRAB_WORK_DIR, d)) \
                    and ( datetime.now()-datetime.strptime(d.rsplit('_',1)[-1], '%y%m%d-%H%M%S') ).days < MOST_RECENT_DAYS
                ]
    print('Total tasks to check: ', len(crabTaskList))


    p = ThreadPool()
    crabTaskStatuses = p.map(checkSingleTask, crabTaskList)
    p.close()

    task_completed, task_failed, task_others, task_exception = [], [], [], []
    for d in crabTaskStatuses:
        if d.get('exception', False):
            task_exception.append(d)
        else:
            _status = d.get('status', None)
            if _status == 'completed':
                task_completed.append(d)
            elif _status == 'failed':
                task_failed.append(d)
            else:
                task_others.append(d)


    p = ThreadPool()
    crabResubmitResult = p.map(resubmitSingleTask, task_failed+task_others)
    p.close()

    resubmittedTasks = [t for t in crabResubmitResult if t]
    resubTaskSuccess, resubTaskFail = [], []
    for t in resubmittedTasks:
        if t.get('success', False):
            resubTaskSuccess.append(t)
        else:
            resubTaskFail.append(t)



    with open('crabjobsCheckAndResubReport.log', 'w') as of:
        of.write(time.asctime()+'\n')
        of.write('='*79 + '\n\n')


        if task_completed:
            of.write('Completed tasks:\n===========================\n')

            for t in task_completed:
                toprint = 'directory: {0}\ntask: {1}\ndataset: {2}\n\n'.format(
                    t['directory'], t['task'], t['outdatasets'])
                of.write(toprint)

            of.write('-'*79+'\n\n')


        if task_others:
            of.write('Other tasks:\n===========================\n')

            for t in task_others:
                toprint = 'directory: {0}\ntask: {1}\nstatus: {2}\njobsPerStatus: {3}\npublication: {4}\n\n'.format(
                    t['directory'], t['task'], t['status'], str(t['jobsperstatus']), str(t['publication']))
                of.write(toprint)

            of.write('-'*79+'\n\n')


        if task_failed:
            of.write('Failed tasks:\n===========================\n')

            for t in task_failed:
                toprint = 'directory: {0}\ntask: {1}\njobsPerStatus: {2}\npublication: {3}\n\n'.format(
                    t['directory'], t['task'], str(t['jobsperstatus']), str(t['publication']))
                of.write(toprint)

            of.write('-'*79+'\n\n')


        if task_exception:
            of.write('Exception tasks:\n===========================\n')
            for t in task_exception:
                toprint = 'directory: {0}\nmessage: {1}\n\n'.format(
                    t['directory'], t['msg'])
                of.write(toprint)

            of.write('+'*79+'\n\n')



        if resubTaskSuccess:
            of.write('Successfully resubmitted tasks\n')
            of.write('==============================\n')
            for d in resubTaskSuccess:
                of.write(d['directory']+'\n')
            of.write('-'*79+'\n\n')

        if resubTaskFail:
            of.write('Failed resubmitted tasks\n')
            of.write('==============================\n')
            for d in resubTaskFail:
                of.write(d['directory']+'\n')
                of.write(d['exceptionMsg']+'\n\n')
            of.write('-'*79+'\n\n')



if __name__ == "__main__":
    main()