#!/bin/bash

if [ ! -e .passed_mergesort ] ;
then
    echo "Must pass \`make test\` before queueing jobs"
    exit 1
fi


running=$(qstat -u ${USER} | egrep "bench_mergesort.*[R|Q]")
if [ -n "${running}" ] ;
then
    echo "------------------------------------"
    echo ${USER} has this job in the queue or running 
    qstat | grep ${USER}
    echo
    echo please delete this job or wait for it to complete
    exit 1
fi

sbatch --partition=Centaurus --time=02:00:00 --nodes=1 --ntasks-per-node=16 bench_mergesort.sh

echo "----------------------"
echo
echo "Once the job COMPLETES, plot with \`make plot\`"
