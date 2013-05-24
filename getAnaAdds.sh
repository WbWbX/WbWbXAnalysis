#!/bin/sh
cd ..
eval `scramv1 runtime -sh`

cd ${CMSSW_BASE}/src

# remove not needed stuff
# no job sendig possible

rm -rf ${CMSSW_BASE}/src/TtZAnalysis/Configuration
rm -rf ${CMSSW_BASE}/src/TtZAnalysis/TreeWriter
rm -rf ${CMSSW_BASE}/src/TtZAnalysis/SimpleCounter
rm -rf ${CMSSW_BASE}/src/TtZAnalysis/Workarounds

#utilz moved to topAnalysis

cvs co -d TopAnalysis/ZTopUtils UserCode/Bromo/TopAnalysis/ZTopUtils

#b-jet stuff

cd $CMSSW_BASE/src/TtZAnalysis/Data
ln -s /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp PileUpJsons
cd $CMSSW_BASE/src


cd $CMSSW_BASE/src

addpkg FWCore/FWLite/src/FWCoreFWLite

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
