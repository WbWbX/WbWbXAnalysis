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

for file in *; 
do

    fulloutpath=../output/$file
    fullcheckpath=../batch/$file
    jobname=$file
    jobid=""
    if [ ${SGE_CELL} ]
    then
	if [ -e "../stdout/${file}.txt" ];
	then
	    jobid=`ls ../batch/${file}.o* 2>&1 | awk -F".o" '{print $NF}'`
	fi
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
	    if [ $jobid ]
	    then
		
		if [ ${jobrunning} ]
		then
		    echo "${jobname} \e[1;32m partially done (${jobid})\e[0m"
		elif [ ${jobEqw} ]
		then
		    echo "${jobname} \e[1;31m in Eqw state (${jobid})\e[0m"
		else
		    echo "${jobname} \e[1;31m  partially done but aborted\e[0m"
		    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
		fi
	    else
		echo "${jobname} \e[1;31m  partially done but without ID \e[0m"
		tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		rmfiles=( "${rmfiles[@]}" "rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
	    fi
	fi
    elif [ -e "${fullcheckpath}_failed" ];
    then
	echo "${jobname} \e[1;31m completely failed \e[0m"
    elif [ -e "../stdout/${file}.txt" ];
    then
	if [ $jobid ]
	then
	    if [ ${jobrunning} ]
	    then
		echo "${jobname} \e[1;32m running (${jobid})\e[0m"
	    else
		echo "${jobname} \e[1;31m aborted before producing output\e[0m"
		tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt" );
	    fi
	else
	    echo "${jobname} \e[1;31m  partially done but without ID \e[0m"
	    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
	    rmfiles=( "${rmfiles[@]}" "rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
	fi
    else
	if [ $jobid ]
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
    if [[ "${option}" == "resubmit" ]]
    then
	eval `echo "${rmfiles[@]}"`
	cd ../batch
	for (( i=0;i<${#tresubmit[@]};i++)); do
	    
    #then
	    if [ ${tresubmit[${i}]} ]
	    then
		qsub ${tresubmit[${i}]}
	    fi
    #fi
	done
    else
	echo "to resubmit aborted jobs run check.sh resubmit!"
	

    fi
else
    echo "You are not on the NAF. Resubmitting not possible"
fi
