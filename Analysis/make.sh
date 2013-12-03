#!/bin/sh

cd $CMSSW_BASE/src/TtZAnalysis/Analysis


export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`

CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
LOCBIN=$CMSSW_BASE/src/TtZAnalysis/Analysis/bin
BUILDDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/build
SRCDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/app_src
LOCLIB=$CMSSW_BASE/src/TtZAnalysis/Analysis/lib
#declare -a libs
libs=("TopAnalysisZTopUtils" 
    "TtZAnalysisDataFormats" 
    "TtZAnalysisTools"
    "TtZAnalysisAnalysis"
    "FWCoreFWLite"
    "unfold"
#"DataFormatsStdDictionaries"
#"CondFormatsJetMETObjects"
);

#echo $libs

#exit


function compile(){
    infile=$1
    linklibs=""
    libdir=$LOCLIB

    for (( i=0;i<${#libs[@]};i++)); do
	linklibs="$linklibs -l${libs[${i}]}"
	cp ${CMSLIBS}lib${libs[${i}]}.so $libdir
    done
    echo compiling $infile

    g++ $ROOTFLAGS -fopenmp -I$CPLUS_INCLUDE_PATH -c -o $BUILDDIR/$infile.o $SRCDIR/$infile.cc
    g++ -o $LOCBIN/$1 -fopenmp -Wall $ROOTLIBS -L$libdir $linklibs -l${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so $BUILDDIR/$infile.o

}


#if [ $1 == "all" ];
#then
    cd $CMSSW_BASE/src/TtZAnalysis/Analysis
    scram b -j12

#fi

cd $BUILDDIR
if  [ $1 == "all" ] || [ $1 == "run" ];
then

    compile analyse
    compile mergeSyst


fi

if  [ $1 == "all" ] || [ $1 == "unfold" ];
then
    
    compile unfoldTtBar

fi

if  [ $1 == "all" ] || [ $1 == "helper" ];
then
    
    compile plotCSV
    compile printVariations
    compile mtFromXsec
fi
if  [ $1 == "all" ] || [ $1 == "www" ];
then
    
    compile dumpToWWW
    
fi


if  [ $1 == "all" ] || [ $1 == "combined" ];
then
    
    compile mergeUnfoldDump
    
fi
