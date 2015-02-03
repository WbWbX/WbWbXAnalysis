{


  //for interactive vector handling 
#include <vector>
#pragma link C++ class std::vector<float>;

  gSystem->Load("libFWCoreFWLite.so"); 
  AutoLibraryLoader::enable();

  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libTtZAnalysisDataFormats.so");
  gSystem->Load("libTtZAnalysisTools.so");
  gSystem->Load("libTopAnalysisZTopUtils.so");

  cout << "TtZAnalysis libs loaded" <<endl;


  //this would make inlined functions working (root bug), but takes a lot of loading time
  //#include "TtZAnalysis/DataFormats/src/classes.h"


  //reduce eye-bleeding with standard colz style
  const Int_t logon_NRGBs = 5;
  const Int_t logon_NCont = 255;

  Double_t logon_stops[logon_NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t logon_red[logon_NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t logon_green[logon_NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t logon_blue[logon_NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(logon_NRGBs, logon_stops, logon_red, logon_green, logon_blue, logon_NCont);
  gStyle->SetNumberContours(logon_NCont);

  using namespace ztop; //complain if you don't like


}

