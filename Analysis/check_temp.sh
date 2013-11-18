#!/bin/zsh

cd ##WORKDIR##

option=$1

cd jobscripts

if [ ${SGE_CELL} ]
then
    joblist=`qstat -u ${USER} 2>/dev/null`
    qwjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="qw") print $(i-4) }}'`
    rjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="r") print $(i-4) }}'`
    Eqwjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="Eqw") print $(i-4) }}'`
fi

#qcommand="qstat -u ${USER} -j ${jobid} | grep sge_o_log_name"

#for (( i=0;i<${#alljobs[@]};i++)); do
for file in *; 
do

    fulloutpath=../output/$file
    fullcheckpath=../batch/$file
    jobname=$file
    if [ ${SGE_CELL} ]
    then
	jobid=`ls ../batch/${file}.po* 2>/dev/null | awk -F".po" '{print $NF}'`
	jobrunning=`echo $rjobs | grep ${jobid} 2>/dev/null | head -c 3`;
	jobEqw=`echo $Eqwjobs | grep ${jobid} 2>/dev/null | head -c 3`;
	jobQ=`echo $qjobs | grep ${jobid} 2>/dev/null | head -c 3`;
    fi
    
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
		
		if [ ${jobrunning} ]
		then
		    echo "${jobname} \e[1;32m partially done (${jobid})\e[0m"
		elif [ ${jobEqw} ]
		then
		    echo "${jobname} \e[1;31m in Eqw state (${jobid})\e[0m"
		else
		    echo "${jobname} \e[1;31m  partially done but aborted\e[0m"
		    tresubmit=( "${tresubmit[@]}" "; qsub ../jobscripts/${jobname}" );
		    oldids=( "${oldids[@]}" "*${jobid}" );
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
	    if [ ${jobrunning} ]
	    then
		echo "${jobname} \e[1;32m running (${jobid})\e[0m"
	    else
		echo "${jobname} \e[1;31m aborted before producing output\e[0m"
		tresubmit=( "${tresubmit[@]}" "; qsub ../jobscripts/${jobname}" );
		oldids=( "${oldids[@]}" "*${jobid}" );
	    fi
	else
	    echo "${jobname} \e[1;32m seems to be running\e[0m"
	fi
    else
	if [ ${SGE_CELL} ]
	then
	    if [ ${jobEqw} ]
	    then
		echo "${jobname} \e[1;31min Eqw state!! \e[0m"
	    elif [ ${jobQ} ]
	    then
		echo  "${jobname} \e[1;30m waiting (${jobid})\e[0m"
	    fi
	else
	    echo  "${jobname} \e[1;30m waiting \e[0m"
	fi
    fi
done

echo "for merging of systematics of successful jobs do (in output dir):"
echo "mergeSyst ${jdone[@]}"
if [ ${SGE_CELL} ]
then
    echo "to resubmit aborted jobs clear id files etc by hand!"
fi
exit
##some thoughts on auto resubmit
    if [ 1 ]
    then 
	echo "resubmitting jobs"
	cd ../batch
	rm ${oldids[@]} 2>/dev/null
	eval `echo ${tresubmit[@]}`
    fi
fi
