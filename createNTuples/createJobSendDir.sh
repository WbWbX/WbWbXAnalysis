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

echo "************************************************"
echo "* This script will setup a copy of your CMSSW- *"
echo "* installation that can be used to submit      *"
echo "* ntuple production jobs without interfering   *"
echo "* with your current setup. This might take a   *"
echo "* while...                                     *"
echo "* to continue type \"yes\"                       *"
echo "************************************************\n"

read input
if [[ $input != "yes" ]] ;
then
    exit
fi

mkdir -p /nfs/dust/cms/user/$USER/JobSend


cd /nfs/dust/cms/user/$USER/JobSend

dir=$(date +"%Y%m%d_%H%M")
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


echo "doing TtZAnalysis specific adjustments (library symlinks)... "

$CMSSW_BASE/src/TtZAnalysis/prepareEnv.sh

scram setup lhapdffull

scram b -j16

echo "to create ntuples, go to ${targetdir}/TtZAnalysis/createNTuples"
echo "and set the environment. Don't pull anything or push anything, this"
echo "is just a plain copy"
