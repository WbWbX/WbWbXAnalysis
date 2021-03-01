#!/bin/zsh

if [[ $CMSSW_VERSION == "" ]]
then
echo "Must be run in CMSSW environment"
exit -2
fi


if [[ `cat /proc/version` == *"el6"* && $CMSSW_VERSION == "CMSSW_5_3_14"* ]]
then
echo "Wrong CMSSW version for SL6!"
exit -2
fi


mkdir -p ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib
cd ${CMSSW_BASE}/src/WbWbXAnalysis

if [[ -a ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so && -a ../TopAnalysis/ZTopUtils/BuildFile.xml &&  -a ../FWCore/FWLite/BuildFile.xml && -a ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/libunfold.so  && -a ${CMSSW_BASE}/runEclipse.sh ]]
then
    echo "CMSSW release already prepared for WbWbXAnalysis Framework"
else
    if [[ ! -a ../TopAnalysis/ZTopUtils/BuildFile.xml ]]
    then
	echo "WbWbXAnalysis depends on the TopAnalysis/ZTopUtils package. Please check it out by hand"
	exit 1
    fi
    if  [[ ! -a ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/libunfold.so ]]
    then
	cd ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold
	make lib
	cd -
	echo "***********************"
	echo "TUnfold library created"
	echo "***********************"
    fi
    #if  [[ ! -a ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so ]]
        #ln -s ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/libunfold.so ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so
    if  [[ ! -a ${CMSSW_BASE}/lib/${SCRAM_ARCH}/libunfold.so ]]
    then
	ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/libunfold.so ${CMSSW_BASE}/lib/${SCRAM_ARCH}/libunfold.so
	ln -s ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/libunfold.so ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so
        ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/TUnfoldBinningV17Dict_rdict.pcm ${CMSSW_BASE}/lib/${SCRAM_ARCH}/TUnfoldBinningV17Dict_rdict.pcm
        ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/TUnfoldBinningXMLV17Dict_rdict.pcm ${CMSSW_BASE}/lib/${SCRAM_ARCH}/TUnfoldBinningXMLV17Dict_rdict.pcm
        ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/TUnfoldDensityV17Dict_rdict.pcm ${CMSSW_BASE}/lib/${SCRAM_ARCH}/TUnfoldDensityV17Dict_rdict.pcm
        ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/TUnfoldSysV17Dict_rdict.pcm ${CMSSW_BASE}/lib/${SCRAM_ARCH}/TUnfoldSysV17Dict_rdict.pcm
        ln -s  ${CMSSW_BASE}/src/WbWbXAnalysis/Tools/TUnfold/TUnfoldV17Dict_rdict.pcm ${CMSSW_BASE}/lib/${SCRAM_ARCH}/TUnfoldV17Dict_rdict.pcm

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
    if [ ! -f $CMSSW_BASE/runEclipse.sh ]
    then
        echo "#!/bin/bash\nif [ ! \`which eclipse\` ]\nthen\necho first install eclipse\nexit\nfi\nexport _JAVA_OPTIONS=-Xmx2048M\neclipse -data ${CMSSW_BASE}/eclipse" > $CMSSW_BASE/runEclipse.sh
        chmod +x  $CMSSW_BASE/runEclipse.sh
    
    echo "*************************"
	echo "Eclipse launcher prepared"
	echo "*************************"
fi
    echo "CMSSW release prepared for WbWbXAnalysis Framework"

fi

cat > $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/boost_iostreams.xml <<TOOL_END
<tool name="boost_iostreams" version="@TOOL_VERSION@">
  <info url="http://www.boost.org"/>
  <lib name="boost_iostreams"/>
  <use name="boost"/>
  <use name="zlib"/>
  <use name="bz2lib"/>
</tool>
TOOL_END

cat > $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/boost_serialization.xml <<TOOL_END
<tool name="boost_serialization" version="@TOOL_VERSION@">
  <info url="http://www.boost.org"/>
  <lib name="boost_serialization"/>
  <use name="boost"/>
  <use name="zlib"/>
  <use name="bz2lib"/>
</tool>
TOOL_END

scram setup boost_serialization
scram setup boost_iostreams
