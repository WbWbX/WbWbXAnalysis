#!/bin/zsh



cd ##WORKDIR##

workdir=`pwd`

job=$1

echo "really remove all output for job ${job}? Type \"yes\"."
echo "old stdout will be kept and renamed"
read input
if [[ $input != "yes" ]] ;
then
    exit
fi

rm -f batch/$job*
rm -f output/$job*
mv stdout/$job.txt "stdout/${job}_old.txt"

echo job $job has been reset
