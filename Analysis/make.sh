#!/bin/sh

cd $CMSSW_BASE/src/TtZAnalysis/Analysis


export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`
ROOTLIBS="${ROOTLIBS} -L/afs/naf.desy.de/group/cms/sw/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_11/external/slc5_amd64_gcc462/bin/../../../../../../lcg/root/5.32.00-cms21/lib -lGenVector"


CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
#EXTLIBS=${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/
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
    mkdir -p $libdir

    for (( i=0;i<${#libs[@]};i++)); do
	linklibs="$linklibs -l${libs[${i}]}"
	cp ${CMSLIBS}lib${libs[${i}]}.so $libdir
    done
    #cp $EXTLIBS/libunfold.so $libdir 
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
    compile mtFromXsec2
fi

if  [ $1 == "all" ] || [ $1 == "www" ];
then
    
    compile dumpToWWW
    
fi


if  [ $1 == "all" ] || [ $1 == "combined" ];
then
    
    compile mergeUnfoldDump
    
fi
