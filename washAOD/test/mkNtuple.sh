#!/bin/bash

TAG=$1
FRAG=$2
YEAR=$3 #2017, 2018
DATALIST=${TAG}.list
OUTPUT=${FRAG}_${TAG}.root

cmsRun ../cfg/${FRAG}_cfg.py year=${YEAR} inputFiles_load=../data/${YEAR}/${DATALIST} outputFile=${YEAR}/${OUTPUT} 2>&1
