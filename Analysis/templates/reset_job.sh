#!/bin/zsh



cd ##WORKDIR##

workdir=`pwd`

function resetJob(){
    job=$1
    rm -f batch/$job* > /dev/null
    rm -f output/$job* > /dev/null
    mv stdout/$job.txt "stdout/${job}_old.txt"  > /dev/null

    echo job $job has been reset
}

job=$1
if [ $1 ] ;
then
    echo "really remove all output for job ${job}? Type \"yes\"."
    echo "old stdout will be kept and renamed"
    read input
    if [[ $input != "yes" ]] ;
    then
	exit
    fi
    resetJob $job
else
    echo "really remove all output failed jobs? Type \"yes\"."
    echo "old stdout will be kept and renamed"
    read inputa
    if [[ $inputa != "yes" ]] ;
    then
	exit
    fi
    cd jobscripts
    for f in *                                                
    do
	if [ -e "../batch/${f}_failed" ]
	then
	    cd ..
	    resetJob $f
	    cd -
	fi
    done
    
fi
