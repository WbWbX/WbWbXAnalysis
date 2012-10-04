#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

dirstring="Analysis_$(date +%F_%H:%M)"

echo "\n\nrunning in directory ${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../*inputfiles.txt .
sed 's/runInNotQuietMode=true/runInNotQuietMode=false/' <../Analyzer.C > Analyzer.C
nohup root -l -b -q Analyzer.C++ > output.txt
echo "done with ${dirstring}"
