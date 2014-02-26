#!/bin/sh
#
#(make sure the right shell will be used)
#$ -S /bin/sh
#$ -l site=hh
#
#(the cpu time for this job)
#$ -l h_rt=01:55:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=1024M
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

exec > "stdout/##OUTNAME##.txt" 2>&1

export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
#echo $LD_LIBRARY_PATH # obsolete by now
#ldd $workdir/analyse
./analyse ##PARAMETERS##


#runthe executable here
