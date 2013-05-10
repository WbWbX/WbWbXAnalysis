#!/bin/sh

infile=analyse.C

export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`

CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
#declare -a libs
libs=("TopAnalysisZTopUtils" 
"TtZAnalysisDataFormats" 
"TtZAnalysisTools"
"TtZAnalysisAnalysis"
#"DataFormatsStdDictionaries"
#"CondFormatsJetMETObjects"
);

#echo $libs

#exit

libdir=$CMSSW_BASE/src/TtZAnalysis/Analysis/lib

mkdir -p $libdir
linklibs=""

for (( i=0;i<${#libs[@]};i++)); do
linklibs="$linklibs -l${libs[${i}]}"
cp $CMSLIBS/lib${libs[${i}]}.so $libdir
done

g++ $ROOTFLAGS -I$CPLUS_INCLUDE_PATH -c -o $infile.o $infile
#exit
g++ -o analyse.exe -O3 $ROOTLIBS -Llib$linklibs $infile.o

