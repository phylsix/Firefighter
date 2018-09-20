#!/bin/bash

TAG=$1
FRAG=$2
DATALIST=${TAG}.list
OUTPUT=${FRAG}_${TAG}.root
YEAR=2017

cmsRun ../cfg/${FRAG}_cfg.py year=${YEAR} inputFiles_load=../data/${YEAR}/${DATALIST} outputFile=${YEAR}/${OUTPUT} 2>&1
