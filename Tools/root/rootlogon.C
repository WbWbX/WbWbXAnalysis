{
     gSystem->Load("libFWCoreFWLite.so"); 
     AutoLibraryLoader::enable();
     gSystem->Load("libDataFormatsFWLite.so");
     gSystem->Load("libTtZAnalysisDataFormats.so");
     gSystem->Load("libTtZAnalysisTools.so");
     gSystem->Load("libTopAnalysisZTopUtils.so");
     gSystem->Load("libRooFit");
     gSystem->Load("libCondFormatsJetMETObjects.so");

     cout << "libs loaded" <<endl;


     //for interactive vector handling 

#include <vector>
#pragma link C++ class std::vector<float>;


}

