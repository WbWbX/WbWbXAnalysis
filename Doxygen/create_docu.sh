#!/bin/zsh

if [[ $CMSSW_VERSION == "" ]]
then
echo "Must be run in CMSSW environment"
exit -2
fi

DOXYOUT=$CMSSW_BASE/src/TtZAnalysis/Doxygen/docu
if [[ $1 != "" ]]
then
echo "creating documentation in: ${1}"
DOXYOUT=$1
else
echo "using default output directory: ${DOXYOUT}"
mkdir -p $DOXYOUT
fi

if [ ! -d $DOXYOUT ]
then
echo specified output directory is not a directory
exit -1
fi


DOXYOUT=`readlink -f $DOXYOUT`
rm -rf   $DOXYOUT
mkdir -p $DOXYOUT
cd $DOXYOUT
export DOXYOUT=$DOXYOUT

doxygen $CMSSW_BASE/src/TtZAnalysis/Doxygen/config/doxy_config

echo Documentation created. The documentation for the browser can be opened with:

echo "file://${DOXYOUT}/html/index.html"

