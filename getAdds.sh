#!/bin/sh
cd ..
eval `scramv1 runtime -sh`
# the things I use from the TopAnalysis code
#

if [[ "${CMSSW_VERSION}" != "CMSSW_5_3_3_patch3" && "${CMSSW_VERSION}" != "CMSSW_5_3_5" ]];
then
    echo 'warning! '"$CMSSW_VERSION"' not supported for automatic checkout of pat packages! add them manually!'
fi

exit

cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorZFilter.cc
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorTopFilter.cc
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/BuildFile.xml
cvs co -d TopAnalysis/TopFilter/ UserCode/Bromo/TopAnalysis/TopFilter/BuildFile.xml
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorTopFilter_cfi.py
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorZFilter_cfi.py

cvs co -d TopAnalysis/Configuration/analysis/diLeptonic  UserCode/Bromo/TopAnalysis/Configuration/analysis/diLeptonic/runallGC.pl

cd ${CMSSW_BASE}/src/TtZAnalysis/Configuration/python/analysis
ln -s ${CMSSW_BASE}/src/TopAnalysis/Configuration/analysis/diLeptonic/runallGC.pl runallGC.pl

cd $CMSSW_BASE/src/TtZAnalysis/Data
ln -s /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp PileUpJsons
cd $CMSSW_BASE/src

# get sample files

#echo "adding samplefiles (update from DAS)"

#cd TtZAnalysis/Configuration/python/samples
#./update_samplefiles2012.sh
#cd mc
#./updatemcfiles12.sh
cd $CMSSW_BASE/src

# the private JEC uncertainties code (standalone)

echo "modified JEC Uncertainty utility.."

cp -r /afs/desy.de/user/k/kiesej/public/code/CondFormats .


echo "Electron MVA stuff"

#electron mva id stuff (following top reference twiki)
cvs co -r V00-00-13 -d EGamma/EGammaAnalysisTools UserCode/EGamma/EGammaAnalysisTools
cd EGamma/EGammaAnalysisTools/data
cat download.url | xargs wget
#cd ../../../

echo "Ecal Filter"

addpkg RecoMET/METFilters V00-00-10

if [[ "$CMSSW_VERSION" == "CMSSW_5_3_3_patch3"  ]]
then

echo "checking out release V08-09-42 for ${CMSSW_VERSION} "

addpkg DataFormats/PatCandidates       V06-05-06-02
addpkg PhysicsTools/PatAlgos           V08-09-42
addpkg PhysicsTools/PatUtils           V03-09-26
addpkg CommonTools/RecoUtils           V00-00-12
addpkg CommonTools/RecoAlgos           V00-03-24
addpkg CommonTools/ParticleFlow        V00-03-16
addpkg RecoParticleFlow/PFProducer   V15-02-05-01
addpkg DataFormats/ParticleFlowCandidate   V15-03-04      
addpkg DataFormats/TrackReco   V10-02-02      
addpkg DataFormats/VertexReco   V02-00-04 

echo 'checked out release V08-09-42 - check if newest one'

fi

if [[ "$CMSSW_VERSION" == "CMSSW_5_3_5" ]]
then

echo "checking out pat release V08-09-43 for ${CMSSW_VERSION} "

addpkg DataFormats/PatCandidates V06-05-06-03
addpkg PhysicsTools/PatAlgos     V08-09-43

fi

#some copy-paste in case you want to use my root macros and the classes on command line
echo 'you will need this in the rootlogon.C:'
echo '
     gSystem->Load("libFWCoreFWLite.so"); 
     AutoLibraryLoader::enable();
     gSystem->Load("libDataFormatsFWLite.so");
     gSystem->Load("libDataFormatsPatCandidates.so");
     gSystem->Load("libTtZAnalysisDataFormats.so");
     gSystem->Load("libRooFit");
     gSystem->Load("libFWCoreUtilities.so");
     gSystem->Load("libCondFormatsJetMETObjects.so");
     cout << "libs loaded" <<endl;
     '



echo "\n\ncompile if you like"
