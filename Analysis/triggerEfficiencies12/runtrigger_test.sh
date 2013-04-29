#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

dirstring=$1"_test_trigger_$(date +%F_%H:%M)"

echo '\n\nrunning in directory' "${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../triggerAnalyzer_base2.h .
cp ../trigger_test.C .
cp ../histoStyle.h .
cp ../getSubSetRoot.C .
nohup root -l -b -q trigger_test.C++ > output.txt 
echo "done with ${dirstring}"
