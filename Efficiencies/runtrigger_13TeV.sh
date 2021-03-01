#!/bin/sh

#runs analysis und puts analysis code and output to an extra directory with date flag

cd ${CMSSW_BASE}/src/WbWbXAnalysis/Efficiencies

source /cvmfs/cms.cern.ch/cmsset_default.sh

eval `scramv1 runtime -sh`


dirstring=$1"_13TeV_trigger_$(date +%F_%H:%M)__GI"

echo '\n\nrunning in directory' "${dirstring}."

mkdir $dirstring
cd $dirstring
cp ../triggerAnalyzer_base_13TeV.h .
#cp ../prescaleList.h .
cp ../trigger_13TeV.C .
cp ../histoStyle.h .
cp ../getSubSetRoot.C .
touch $HOSTNAME
nohup fastCompile.sh trigger_13TeV.C > output.txt 
echo "done with ${dirstring}"
