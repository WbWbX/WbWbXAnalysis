#!/bin/sh

if [[ $CMSSW_BASE == "" ]]
then
echo "please run in CMSSW environment"
exit
fi

cd $CMSSW_BASE/src/TtZAnalysis/Analysis


export CPLUS_INCLUDE_PATH=$CMSSW_BASE/src:$CMSSW_RELEASE_BASE/src
ROOTFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`
ROOTLIBS="${ROOTLIBS} -lXMLParser" #for TUnfold
#ROOTLIBS="${ROOTLIBS} -L/afs/naf.desy.de/group/cms/sw/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_11/external/slc5_amd64_gcc462/bin/../../../../../../lcg/root/5.32.00-cms21/lib -lGenVector"
#naf afs switched off

CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
EXTLIBS=${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/
LOCBIN=$CMSSW_BASE/src/TtZAnalysis/Analysis/bin
BUILDDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/build
SRCDIR=$CMSSW_BASE/src/TtZAnalysis/Analysis/app_src
LOCLIB=$CMSSW_BASE/src/TtZAnalysis/Analysis/lib

RELBASELIBS=$CMSSW_RELEASE_BASE/lib/$SCRAM_ARCH/
#declare -a libs
libs=("TopAnalysisZTopUtils" 
    "TtZAnalysisDataFormats" 
    "TtZAnalysisTools"
    "TtZAnalysisAnalysis"
    "FWCoreFWLite"
# -> goes to external    "unfold"
#"DataFormatsStdDictionaries"
#"CondFormatsJetMETObjects"
);
rellibs=(""
);

#echo $libs

#exit
function copylibs(){

    linklibs=""
    libdir=$LOCLIB
    mkdir -p $libdir

    for (( i=0;i<${#libs[@]};i++)); do
	linklibs="$linklibs -l${libs[${i}]}"
	cp ${CMSLIBS}lib${libs[${i}]}.so $libdir
    done
    for (( i=0;i<${#libs[@]};i++)); do
	if [[ ${rellibs[${i}]} ]]
	then
	linklibs="$linklibs -l${libs[${i}]}"
	cp ${RELBASELIBS}lib${rellibs[${i}]}.so $libdir
	fi
    done
    cp $EXTLIBS/libunfold.so $libdir 

}
function compile(){
    infile=$1
    if [ $infile ]
    then

	outfile=${infile%.cc}
	echo compiling $infile to $outfile
	g++ $ROOTFLAGS -fopenmp -I$CPLUS_INCLUDE_PATH -c -o $BUILDDIR/$infile.o $SRCDIR/$infile
	if [ $? -ne 0 ]; 
	then
	    echo "Compilation of ${infile} not successful"
	    exit 6
	fi
	g++ -o $LOCBIN/$outfile -fopenmp -Wall $ROOTLIBS -L$libdir $linklibs -l${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so $BUILDDIR/$infile.o
	if [ $? -ne 0 ]; 
	then
	    echo "Compilation of ${infile} not successful"
	    exit 6
	fi
    fi

}


cd $CMSSW_BASE/src/TtZAnalysis/Analysis
cd app_src
for i in *.cc
do
    mv -f ../bin/${i%.cc} ../bin/${i%.cc}.old > /dev/null 2>&1 
done
cd -
scram b -j12
if [ $? -ne 0 ]; 
then
    echo "Error in scram while compiling! stopping... old executables are marked as <>.old"
    exit 6
fi


copylibs
cd app_src
for i in *.cc
do
    compile $i&
    rm -f ../bin/${i%.cc}.old
done
sleep 1
echo "waiting for unfinished jobs"
wait
echo "done"
