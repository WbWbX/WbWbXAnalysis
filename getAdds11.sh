#!/bin/sh
cd ..
eval `scramv1 runtime -sh`
# the things I use from the TopAnalysis code
#
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorZFilter.cc
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorTopFilter.cc
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/BuildFile.xml
cvs co -d TopAnalysis/TopFilter/ UserCode/Bromo/TopAnalysis/TopFilter/BuildFile.xml
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorTopFilter_cfi.py
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorZFilter_cfi.py

cvs co -d TopAnalysis/Configuration/analysis/diLeptonic  UserCode/Bromo/TopAnalysis/Configuration/analysis/diLeptonic/runallGC.pl

cd ${CMSSW_BASE}/src/TtZAnalysis/Configuration/python/analysis
ln -s ${CMSSW_BASE}/src/TopAnalysis/Configuration/analysis/diLeptonic/runallGC.pl runallGC.pl


# get sample files

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


echo 'checking out release V08-07-50 - check if newest one'

addpkg DataFormats/PatCandidates V06-04-38
addpkg PhysicsTools/PatAlgos     V08-07-50
addpkg PhysicsTools/PatUtils     V03-09-18-03
addpkg CommonTools/ParticleFlow  V00-03-05-10
addpkg FWCore/GuiBrowsers        V00-00-60


echo 'checked out release V08-07-50 - check if newest one'

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
