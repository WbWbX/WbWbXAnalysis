#!/bin/sh

alljobs=`ls jobscripts`
nominal=""

doloop=$1

for (( i=0;i<${#alljobs[@]};i++)); do

    jobname=${alljobs[${i}]};

    if [[ -f output/$jobname.root ]];
    then
	if [[ -f output/${jobname}_plots.root ]];
	then
	    echo "${jobname} successfully done"
	else
	    echo "${jobname} failed"
	fi
    else
	echo "${jobname} running"
    fi

done

##some structure stuff still missing
