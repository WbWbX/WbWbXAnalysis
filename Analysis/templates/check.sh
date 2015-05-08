#!/bin/zsh

cd ##WORKDIR##


workdir=`pwd`

option=$1

if [ $option ]
then
    if [[ $option != "cont" ]] && [[ $option != "nomerge" ]] && [[ $option != "resubmit" ]];
    then
	echo "option \"${option}\" not known. Available options are:"
	echo "\"nomerge\", \"resubmit\", \"cont\" or no option"
	exit
    fi

fi

cd jobscripts
JOBSONBATCH=$SGE_CELL
if [ ${JOBSONBATCH} ]
then
##JOBSNOTONBATCH##JOBSONBATCH=""
fi

if [ ${JOBSONBATCH} ]
then
    joblist=`qstat -u ${USER} 2>/dev/null`
    qwjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="qw") print $(i-4) }}'`
    rjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="r") print $(i-4) }}'`
    Eqwjobs=`echo ${joblist} | awk '{for (i=1; i<=NF; i++){ if($i=="Eqw") print $(i-4) }}'`
fi

tresubmit=()
rmfiles=()
jdone=()
allj=()

for file in *; 
do

    fulloutpath=../output/$file
    fullcheckpath=../batch/$file
    jobname=$file
    jobid=""
    allj=( "${allj[@]}" "${file}" );
    if [ ${JOBSONBATCH} ]
    then
	if [ -e "../stdout/${file}" ];
	then
	    jobid=`ls ../batch/${file}.o* 2>&1 | awk -F".o" '{print $NF}'`
	fi
	jobrunning=`echo $rjobs | grep ${jobid} 2>/dev/null | head -c 3`;
	jobEqw=`echo $Eqwjobs | grep ${jobid} 2>/dev/null | head -c 3`;
	jobQ=`echo $qjobs | grep ${jobid} 2>/dev/null | head -c 3`;
    fi
    
    if [[ -e "${fulloutpath}.ztop" ]];
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
		    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.ztop;rm -f ../stdout/${file};" );
		fi
	    else
		if [ $JOBSONBATCH ]
		then
		    echo "${jobname} \e[1;31m  partially done but without ID \e[0m"
		    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.ztop;rm -f ../stdout/${file};" );
		else
		    echo "${jobname} \e[1;32m partially done (${jobid})\e[0m"
		fi
	    fi
	fi
    elif [ -e "${fullcheckpath}_failed" ];
    then
	echo "${jobname} \e[1;31m completely failed \e[0m"
    elif [ -e "../stdout/${file}" ];
    then
	if [ $jobid ]
	then
	    if [ ${jobrunning} ]
	    then
		echo "${jobname} \e[1;32m running (${jobid})\e[0m"
	    else
		echo "${jobname} \e[1;31m aborted before producing output\e[0m"
		tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.ztop;rm -f ../stdout/${file};" );
	    fi
	else
	    if [ $JOBSONBATCH ]
	    then
		echo "${jobname} \e[1;31m  partially done but without ID \e[0m"
		tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
		rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.ztop;rm -f ../stdout/${file};" );
	    else
		echo "${jobname} \e[1;32m partially done (${jobid})\e[0m"
	    fi
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
	    echo  "${jobname} \e[01;30m still queuing or \e[1;31m died without any tracable reason!! \e[0m"
	    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
	    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.ztop;rm -f ../stdout/${file};" );
	fi
    fi
done


#echo "for merging of systematics of successful jobs do (in output dir):"
#echo "mergeSyst ${jdone[@]}"

if [ ${JOBSONBATCH} ]
then
    if [[ "${option}" == "resubmit" ]]
    then
	echo "resubmitting ${#tresubmit[@]} jobs: ${tresubmit[@]}"
	eval `echo "${rmfiles[@]}"`
	cd ../batch
	for (( i=1;i<=${#tresubmit[@]};i++)); do
	    if [ "${tresubmit[${i}]}" ]
	    then
	
		 qsub ${tresubmit[${i}]}
	    fi
	done
    else
	if [[ ${#tresubmit[@]} > 0 ]]
	then
	    echo "to resubmit aborted/died jobs run check.sh resubmit!"
	    #echo "${tresubmit[@]}"
	fi

    fi
fi


if [ "${#allj[@]}" -gt "${#jdone[@]}" ]
then

    echo "you have still unfinished jobs: (${#jdone[@]} / ${#allj[@]} done)"
    displayStatusBar ${#jdone[@]} ${#allj[@]}
    if [[ "${option}" == "cont" ]] || [[ "${2}" == "cont" ]]
    then
       sleep 10;
       cd $workdir;
       clear;
       exec ./check.sh $option $2
       exit 7
    fi

    exit 6


else
    echo "all your jobs have finished"
    displayStatusBar ${#jdone[@]} ${#allj[@]}
    if [[ "${option}" == "nomerge" ]]
        then
        exit 0
    fi
    echo "trying to merge systematics...."
    cd $workdir/output

    if [ ${CMSSW_BASE} ]
    then
        mergeSyst --addextension ztop ${jdone[@]}
    else
        echo "Cannot merge systematics. Has to be run from a CMSSW environment"
        exit 0
    fi


fi



