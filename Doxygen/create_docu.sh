#!/bin/zsh

if [[ $CMSSW_VERSION == "" ]]
then
echo "Must be run in CMSSW environment"
exit -2
fi

cd $CMSSW_BASE/src/TtZAnalysis/Doxygen
rm -rf docu
mkdir docu
doxygen config/doxy_config

echo Documentation created. The documentation for the browser can be opened with:
echo "file://${CMSSW_BASE}/src/TtZAnalysis/Doxygen/docu/html/index.html"

