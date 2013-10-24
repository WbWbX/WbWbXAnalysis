#!/bin/sh
#
#(make sure the right shell will be used)
#$ -S /bin/sh
#$ -l site=hh
#
#(the cpu time for this job)
#$ -l h_cpu=04:59:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=2048M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#
#$ -cwd -V
#$ -l h_stack=1536M
#$ -pe multicore 6 -R y

#don't overthread the wn
export OMP_NUM_THREADS=1

workdir=##WORKDIR##

cd $workdir

exec > "stdout/##OUTNAME##.txt" 2>&1

export LD_LIBRARY_PATH=$workdir/lib:$LD_LIBRARY_PATH 
$workdir/analyse ##PARAMETERS##


#runthe executable here
