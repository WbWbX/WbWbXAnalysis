#!/bin/zsh

cd ##WORKDIR##

doloop=$1
cd jobscripts



#for (( i=0;i<${#alljobs[@]};i++)); do
for file in *; 
do

    fulloutpath=../output/$file
    fullcheckpath=../batch/$file
    jobname=$file
    
    if [[ -e "${fulloutpath}.root" ]];
    then
	if [ -e "${fullcheckpath}_fin" ];
	then
	    echo "${jobname} \e[1;42m successfully done \e[0m "
	    jdone=( "${jdone[@]}" "${jobname}" );
	elif [ -e "${fullcheckpath}_failed" ];
	then
	    echo "${jobname} \e[1;31m partially failed \e[0m"
	else
	    if [ ${SGE_CELL} ]
	    then
		jobid=`ls ../batch/${file}.po* | awk -F".po" '/1/ {print $NF}'`
		qout=`qstat -j ${jobid} | grep sge_o_log_name` 
		if [[ "${qout}" == *"${USER}"* ]]
		then
		    echo "${jobname} \e[1;32m partially done (${jobid})\e[0m"
		else
		    echo "${jobname} \e[1;31m  partially done but aborted\e[0m"
		    tresubmit=( "${tresubmit[@]}" "; qsub jobscripts/${jobname}" );
		fi
	    else
		echo "${jobname} \e[1;32m partially done\e[0m"
	    fi
	fi
    elif [ -e "${fullcheckpath}_failed" ];
    then
	echo "${jobname} \e[1;31m completely failed \e[0m"
    elif [ -e "../stdout/${file}.txt" ];
    then
	if [ ${SGE_CELL} ]
	then
	    jobid=`ls ../batch/${file}.po* | awk -F".po" '/1/ {print $NF}'`
	    qout=`qstat -j ${jobid} 2>/dev/null | grep sge_o_log_name` 
	    if [[ "${qout}" == *"${USER}"* ]]
	    then
		echo "${jobname} \e[1;32m running (${jobid})\e[0m"
	    else
		echo "${jobname} \e[1;31m aborted before producing output\e[0m"
	    tresubmit=( "${tresubmit[@]}" "; qsub jobscripts/${jobname}" );
	    fi
	else
	    echo "${jobname} \e[1;32m seems to be running\e[0m"
	fi
    else
	echo  "${jobname} \e[1;30m waiting \e[0m"

    fi
done

echo "for merging of systematics of successful jobs do (in output dir):"
echo "mergeSyst ${jdone[@]}"
if [ ${SGE_CELL} ]
then
    echo "to resubmit aborted jobs do: ${tresubmit[@]}"
fi
