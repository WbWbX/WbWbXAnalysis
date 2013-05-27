#!/bin/zsh


doloop=$1
cd jobscripts

#for (( i=0;i<${#alljobs[@]};i++)); do
for file in *; do

    jobname=$file
    if [[ -f output/$jobname.root ]];
    then
	if [[ -f output/${jobname}_plots.root ]];
	then
	    echo "${jobname} \e[1;42m successfully done \e[0m "
	   # if [[ "${jobname}" == *"nominal"*]]
	   ## then
		#nominal
	    done=( "${done[@]}" "${jobname}" );
	else
	    echo "${jobname} \e[1;31m failed \e[0m"
	fi
    else
	echo "${jobname} \e[1;42m running \e[0m"
    fi
done

echo "for merging of systematics of successful jobs do:"
echo "../mergeSyst.exe ${done[@]}"
