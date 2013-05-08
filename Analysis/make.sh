#!/bin/sh

infile=$1

CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$CMSSW_BASE/src/
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`

CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH

TOPZTLIB=$CMSLIBS/libTopAnalysisZTopUtils.so
TTZDLIB=$CMSLIBS/libTtZAnalysisDataFormats.so
TTZTLIB=$CMSLIBS/libTtZAnalysisTools.so



g++ -c $ROOTFLAGS -L$CPLUS_INCLUDE_PATH -o $infile.o $infile

g++ -o $infile.exe $ROOTLIBS -L$LD_LIBRARY_PATH -l$TOPZTLIB -l$TTZDLIB -l$TTZTLIB $infile.o
