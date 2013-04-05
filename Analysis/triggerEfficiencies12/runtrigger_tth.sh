#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

dirstring=$1"_tth_trigger_$(date +%F_%H:%M)"

echo '\n\nrunning in directory' "${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../triggerAnalyzer_base.h .
cp ../trigger_tth.C .
cp ../makeplotsnice.h .
cp ../make2dplots.h .
nohup root -l -b -q trigger_tth.C++ > output.txt 
echo "done with ${dirstring}"
