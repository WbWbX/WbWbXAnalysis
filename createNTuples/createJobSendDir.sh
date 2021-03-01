#!/bin/zsh



if [[ $SGE_CELL == "" ]] ;
then
    echo has to be run on naf
exit
fi
if [[ $CMSSW_BASE == "" ]] ;
then
    echo has to be run on from a source CMSSW environment
exit
fi
dir=$(date +"%Y%m%d_%H%M")_$1

echo "************************************************"
echo "* This script will setup a copy of your CMSSW- *"
echo "* installation that can be used to submit      *"
echo "* ntuple production jobs without interfering   *"
echo "* with your current setup. This might take a   *"
echo "* while...                                     *"
echo "************************************************"
echo " Its name will be ${dir}" 
echo " to continue type \"yes\"  "

read input
if [[ $input != "yes" ]] ;
then
    exit
fi

mkdir -p /nfs/dust/cms/user/$USER/JobSend


cd /nfs/dust/cms/user/$USER/JobSend


mkdir $dir
cd $dir

echo "set up from: \n${CMSSW_BASE}\n\nOnly use to sumbit jobs, don't do any git pulls here!" > source.txt

echo "setting up a new ${CMSSW_VERSION} environment... "

scramv1 project CMSSW $CMSSW_VERSION
cd $CMSSW_VERSION/src

targetdir=`pwd`
sourcedir=$CMSSW_BASE/src

echo "copying from ${sourcedir} to ${targetdir}"


cp -r $sourcedir/* $targetdir/


cd $targetdir
eval `scramv1 runtime -sh`


echo "doing WbWbXAnalysis specific adjustments (library symlinks)... "

$CMSSW_BASE/src/WbWbXAnalysis/prepareEnv.sh

scram setup lhapdffull

scram b USER_CXXFLAGS="-Wno-delete-non-virtual-dtor" -j16

echo "to create ntuples, go to ${targetdir}/WbWbXAnalysis/createNTuples"
echo "and set the environment. Don't pull anything or push anything, this"
echo "is just a plain copy"
echo "!!!!! please do cmsenv and compile (scram b)  again in ${targetdir}!!"
echo "!!!!!       automatic compilation doesn't work reliably            !!"
