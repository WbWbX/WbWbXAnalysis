#!/bin/zsh
if [ ! $CMSSW_BASE ]
then
echo "Needs to be executed with a CMSSW environment set up"
exit -1
fi

mkdir -p haddlog
for f in naf_*; do; 
    if [[ "${f}" == *"run"* ]]; then  
	echo "not using ${f} (is data)";
    else; 
	echo "${f} is going to be hadded" ; 
	$CMSSW_BASE/src/WbWbXAnalysis/createNTuples/somehelpers/haddUnfinished.sh $f > haddlog/$f.hadd.out &; 
    fi; 
    echo 
done
