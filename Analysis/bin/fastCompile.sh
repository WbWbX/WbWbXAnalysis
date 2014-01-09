#!/bin/sh


infile=$1

export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`
ROOTLIBS="${ROOTLIBS} -L/afs/naf.desy.de/group/cms/sw/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_11/external/slc5_amd64_gcc462/bin/../../../../../../lcg/root/5.32.00-cms21/lib -lGenVector"

CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
EXTLIBS=${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/
LOCBIN=$CMSSW_BASE/src/TtZAnalysis/Analysis/bin
BUILDDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/build
SRCDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/app_src
LOCLIB=$CMSSW_BASE/src/TtZAnalysis/Analysis/lib

libs=("TopAnalysisZTopUtils" 
    "TtZAnalysisDataFormats" 
    "TtZAnalysisTools"
    "TtZAnalysisAnalysis"
    "FWCoreFWLite"
#"DataFormatsStdDictionaries"
#"CondFormatsJetMETObjects"
);

linklibs=""

for (( i=0;i<${#libs[@]};i++)); do
    linklibs="$linklibs -l${libs[${i}]}"
done

#echo $ROOTLIBS 
g++ $ROOTFLAGS -fopenmp -I$CPLUS_INCLUDE_PATH -c -o $infile.o $infile
g++ -o $infile.exe -fopenmp -Wall $ROOTLIBS -L$CMSLIBS $linklibs $infile.o  
chmod +x $infile.exe
./$infile.exe
