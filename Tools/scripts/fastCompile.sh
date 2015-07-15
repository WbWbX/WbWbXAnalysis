#!/bin/sh

######
# script to do a quick check of test executables
# it will link libabries etc and execute the executable. Intermediate build files will be removed

echo "This script does not replace a proper compilation. It is just meant to quickly test small executables."

infile=$1
opts=$2

export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src:/usr/include/gtk-2.0/:/usr/include/glib-2.0/
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`
ROOTLIBS="${ROOTLIBS} -lXMLParser" #for TUnfold

#ROOTLIBS="${ROOTLIBS} -L/afs/naf.desy.de/group/cms/sw/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_11/external/slc5_amd64_gcc462/bin/../../../../../../lcg/root/5.32.00-cms21/lib -lGenVector"

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
./$infile.exe $opts
rm -f $infile.exe
rm -f $infile.o
