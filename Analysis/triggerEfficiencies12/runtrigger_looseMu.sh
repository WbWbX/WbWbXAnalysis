#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

dirstring=$1"_looseMu_trigger_$(date +%F_%H:%M)"

echo '\n\nrunning in directory' "${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../triggerAnalyzer_base*.h .
#cp ../prescaleList.h .
cp ../trigger_looseMu.C .
cp ../histoStyle.h .
cp ../getSubSetRoot.C .
nohup root -l -b -q trigger_looseMu.C++ > output.txt 
echo "done with ${dirstring}"
