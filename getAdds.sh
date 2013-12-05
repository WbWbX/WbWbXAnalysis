#!/bin/sh
####
#
# run before cloning any usercode (also TtZAnalysis) rep into CMSSW_BASE/src...
# needs CMS env
#
####
if [[ "${CMSSW_VERSION}" != "CMSSW_5_3_11" && "${CMSSW_VERSION}" != "CMSSW_5_3_12_patch2" ]];
then
    echo 'warning! '"$CMSSW_VERSION"' not supported for automatic checkout of pat packages! add them manually!'
fi


cd ${CMSSW_BASE}/src



if [[ "$CMSSW_VERSION" == "CMSSW_5_3_12_patch2" ]]
then

git cms-addpkg PhysicsTools/PatAlgos
git cms-merge-topic cms-analysis-tools:5_3_12_patch2-metUncertainties
git cms-merge-topic cms-analysis-tools:5_3_12_patch2-newJECs
#git cms-merge-topic cms-analysis-tools:5_3_12_patch2-mvaElIdPatFunction

git cms-addpkg FWCore/FWLite
git cms-addpkg ElectroWeakAnalysis/Utilities

fi

# the things I use from the TopAnalysis code
#


echo checking out TtZAnalysis package
cd ${CMSSW_BASE}/src
git clone https://github.com/jkiesele/TtZAnalysis.git

#fast hack
ln -s ${CMSSW_BASE}/src/TtZAnalysis/Tools/TUnfold/libunfold.so ${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/libunfold.so

##TopAnalysis package
cd ${CMSSW_BASE}/src
git clone https://git.cern.ch/reps/TopAnalysis






echo "Electron MVA stuff"

#electron mva id stuff (following top reference twiki)
cvs co -r V00-00-13 -d EGamma/EGammaAnalysisTools UserCode/EGamma/EGammaAnalysisTools
cd EGamma/EGammaAnalysisTools/data
cat download.url | xargs wget
cd $CMSSW_BASE/src

echo "MET  Filter"

cvs co -r V00-00-13-01 RecoMET/METFilters
cvs co -r V00-00-08 RecoMET/METAnalyzers
cvs co -r V00-03-23 CommonTools/RecoAlgos
cvs co -r V01-00-11-01 DPGAnalysis/Skims
cvs co -r V00-11-17 DPGAnalysis/SiStripTools
cvs co -r V00-00-08 DataFormats/TrackerCommon
cvs co -r V01-09-05 RecoLocalTracker/SubCollectionProducers

#git cms-addpkg  RecoMET/METFilters #??


#
#echo "setting LHAPDF path to /afs/naf.desy.de/user/k/kieseler/public/lhapdf"
#
#mv $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/lhapdf.xml $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/lhapdf.xml.old
#
#sed  -e 's;environment name=\"LHAPDF_BASE\" default=\"/afs/naf.desy.de/group/cms/sw/slc5_amd64_gcc462/external/lhapdf/5.8.5-cms2\";environment name=\"LHAPDF_BASE\" default=\"/afs/naf.desy.de/user/k/kieseler/public/lhapdf\";g' < $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/lhapdf.xml.old  > $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/lhapdf.xml




#some copy-paste in case you want to use my root macros and the classes on command line
echo 'you will need this in the rootlogon.C:'
echo '
     gSystem->Load("libFWCoreFWLite.so"); 
     AutoLibraryLoader::enable();
     gSystem->Load("libDataFormatsFWLite.so");
     gSystem->Load("libDataFormatsPatCandidates.so");
     gSystem->Load("libTtZAnalysisDataFormats.so");
     gSystem->Load("libTtZAnalysisTools.so");
     gSystem->Load("libTopAnalysisZTopUtils.so");
     gSystem->Load("libRooFit");
     gSystem->Load("libFWCoreUtilities.so");
     cout << "libs loaded" <<endl;
     '


echo "To prepare eclipse project run: /afs/naf.desy.de/user/k/kieseler/public/eclipsetemp/prepareEclipseProject.sh"
echo "don't forget to do <scram setup lhapdffull> when intending to use pdf weights"

echo "\n\ncompile if you like"
