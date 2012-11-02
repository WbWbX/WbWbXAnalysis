#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

dirstring="trigger_$(date +%F_%H:%M)"

echo '\n\nrunning in directory' "${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../do_triggerAnalyzer.C .
cp ../makeplotsnice.h .
nohup root -l -b -q do_triggerAnalyzer.C++ > output.txt 
echo "done with ${dirstring}"
