#!/bin/zsh

cd ##WORKDIR##


workdir=`pwd`

option=$1

cd jobscripts


if [ ${SGE_CELL} ]
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
		rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
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
		rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
	    fi
	else
	    echo "${jobname} \e[1;31m  partially done but without ID \e[0m"
	    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
	    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
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
	    echo  "${jobname} \e[1;31m is not submitted or died without any tracable reason!! \e[0m"
	    tresubmit=( "${tresubmit[@]}" "../jobscripts/${jobname}" );
	    rmfiles=( "${rmfiles[@]}" "rm -f ../batch/${file}.o${jobid}; rm -f ../output/${file}.root;rm -f ../stdout/${file}.txt;" );
	fi
    fi
done


#echo "for merging of systematics of successful jobs do (in output dir):"
#echo "mergeSyst ${jdone[@]}"

if [ ${SGE_CELL} ]
then
    if [[ "${option}" == "resubmit" ]]
    then
	echo "resubmitting ${#tresubmit[@]} jobs: ${tresubmit[@]}"
	eval `echo "${rmfiles[@]}"`
	cd ../batch
	for (( i=0;i<${#tresubmit[@]};i++)); do
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


else
exit
    echo "all your jobs have finished, trying to merge systematics...."
    cd $workdir/output

    test=`echo $PATH | grep ${CMSSW_BASE}/src/TtZAnalysis/Analysis/bin`
    if [ $test ]
    then
	mergeSyst ${jdone[@]}
    else
	if [ ${CMSSW_BASE} ]
	then
	    echo "Your TtZAnalysis environment is not setup correctly, setting it up and merging"
	    export PATH=${CMSSW_BASE}/src/TtZAnalysis/Analysis/bin:$PATH
	    mergeSyst ${jdone[@]}
	else
	    echo "Cannot merge systematics. Has to be run from a CMSSW environment"
	fi
    fi


fi

