#!/bin/sh
cd ..
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorZFilter.cc
cvs co -d TopAnalysis/TopFilter/plugins/ UserCode/Bromo/TopAnalysis/TopFilter/plugins/GeneratorTopFilter.cc
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorTopFilter_cfi.py
cvs co -d TopAnalysis/TopFilter/python/filters/ UserCode/Bromo/TopAnalysis/TopFilter/python/filters/GeneratorZFilter_cfi.py

cp -r /afs/desy.de/user/k/kiesej/public/code/CondFormats .

cvs co -r V00-00-13 -d EGamma/EGammaAnalysisTools UserCode/EGamma/EGammaAnalysisTools
cd EGamma/EGammaAnalysisTools/data
cat download.url | xargs wget
#cd ../../../
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
echo 'dont forget to checkout the latest pat release'
