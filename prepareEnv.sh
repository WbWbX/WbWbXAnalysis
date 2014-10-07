#!/bin/zsh

mkdir -p ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib
cd ${CMSSW_BASE}/src/TtZAnalysis

if [[ -a ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so && -a ../TopAnalysis/ZTopUtils/BuildFile.xml &&  -a ../FWCore/FWLite/BuildFile.xml && -a ${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so  ]]
then
    echo "CMSSW release already prepared for TtZAnalysis Framework"
else
    if [[ ! -a ../TopAnalysis/ZTopUtils/BuildFile.xml ]]
    then
	echo "TtZAnalysis depends on the TopAnalysis/ZTopUtils package. Please check it out by hand"
	exit 1
    fi
    if  [[ ! -a ${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so ]]
    then
	cd ${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold
	make lib
	cd -
	echo "***********************"
	echo "TUnfold library created"
	echo "***********************"
    fi
    if  [[ ! -a ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so ]]
    then
	ln -s ${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so

	echo "*******************"
	echo "library symlink set"
	echo "*******************"
    fi
    if [[ !  -a ../FWCore/FWLite/BuildFile.xml  ]]
    then
	mkdir -p ../FWCore
	ln -s $CMSSW_RELEASE_BASE/src/FWCore/FWLite ../FWCore/FWLite

	echo "************************"
	echo "linked to  FWCore/FWLite"
	echo "************************"
    fi
     echo "CMSSW release prepared for TtZAnalysis Framework"

fi
