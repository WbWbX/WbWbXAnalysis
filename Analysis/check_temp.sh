#!/bin/zsh

cd ##WORKDIR##

doloop=$1
cd jobscripts

#for (( i=0;i<${#alljobs[@]};i++)); do
for file in *; do

    fullpath=../output/$file
    jobname=$file
    #echo "checking $fullpath"
    if [[ -e "${fullpath}.root" ]];
    then
	if [ -e "${fullpath}_fin" ];
	then
	    echo "${jobname} \e[1;42m successfully done \e[0m "
	   # if [[ "${jobname}" == *"nominal"*]]
	   ## then
		#nominal
	    done=( "${done[@]}" "${jobname}" );
	elif [ -e "${fullpath}_failed" ];
	then
	    echo "${jobname} \e[1;31m partially failed \e[0m"
	else
	    echo "${jobname} \e[1;32m partially done \e[0m"
	fi
    elif [ -e "${fullpath}_failed" ];
    then
	echo "${jobname} \e[1;31m completely failed \e[0m"
    elif [ -e "stdout_${file}.txt" ];
    then
	echo "${jobname} \e[1;32m running \e[0m"
    else
	echo  "${jobname} \e[1;30m waiting \e[0m"
    fi
done

echo "for merging of systematics of successful jobs do (in output dir):"
echo "mergeSyst ${done[@]}"
