#!/bin/bash

TAG=$1
FRAG=$2
YEAR=$3 #2017, 2018

DATALIST=${TAG}.list
OUTPUT=${FRAG}_${TAG}.root
LOGDIR=Logs/${YEAR}
LOGFILE=${FRAG}_${TAG}.log

[ -d ${LOGDIR} ] || mkdir -p ${LOGDIR}

cmsRun ../cfg/${FRAG}_cfg.py year=${YEAR} inputFiles_load=../data/${YEAR}/${DATALIST} outputFile=${YEAR}/${OUTPUT} > ${LOGDIR}/${LOGFILE} 2>&1
