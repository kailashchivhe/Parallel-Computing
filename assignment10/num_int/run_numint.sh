#!/bin/sh

RESULTDIR=result/

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


# import params.
source ../params.sh
P=${SLURM_CPUS_ON_NODE}
NP=${SLURM_NNODES}


for N in ${NUM_INT_NS} ;
do
   for INTEN in ${INTENSITIES} ;
   do
       TIMEFILE=${RESULTDIR}/mpi_num_int_${N}_${INTEN}_${NP}_${P}
       srun ./mpi_num_int 1 0 10 ${N} ${INTEN} > /dev/null 2> ${TIMEFILE}
       process_time_file ${TIMEFILE}
   done
done
