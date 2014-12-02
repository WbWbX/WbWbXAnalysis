#!/bin/sh
#
#(make sure the right shell will be used)
#$ -S /bin/sh
#$ -l site=hh
#
#(the cpu time for this job)
#$ -l h_rt=02:55:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=2047M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#$ -m a
#$ -cwd -V
#( -l h_stack=1536M) #try with small stack
#$ -pe local 6 -R y
#$ -P af-cms

#don't overthread the wn
export OMP_NUM_THREADS=1

workdir=##WORKDIR##

cd $workdir

exec > "stdout/##OUTNAME##" 2>&1

export LD_LIBRARY_PATH=##WORKDIR##/lib:$LD_LIBRARY_PATH

./bin/analyse ##PARAMETERS##


#runthe executable here
