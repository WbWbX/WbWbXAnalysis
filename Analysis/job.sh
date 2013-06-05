#!/bin/sh
#
#(make sure the right shell will be used)
#$ -S /bin/sh
#
#(the cpu time for this job)
#$ -l h_cpu=01:00:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=1024M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#
#$ -cwd -V
#$ -l h_stack=1024M
#$ -pe multicore 8 -R y

workdir=$CMSSW_BASE/src/TtZAnalysis/Analysis/##WORKDIR##

cd $workdir

exec > "stdout_##OUTNAME##.txt" 2>&1

export LD_LIBRARY_PATH=$workdir/lib:$LD_LIBRARY_PATH 
$workdir/analyse.exe ##PARAMETERS##


#runthe executable here
