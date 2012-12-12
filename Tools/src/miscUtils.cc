#include "../interface/miscUtils.h"


namespace top{
  void displayStatusBar(Long64_t event, Long64_t nEvents){
    
    if((event +1)* 50 % nEvents <50){ //2% steps
      int statusbar=(event+1) * 50 / nEvents ;
      std::cout << "\r[";
      for(int i=0;i<statusbar;i++){
	std::cout << "=";
      }
      for(int i=statusbar;i<50;i++){
	std::cout << " ";
      }
      std::cout << "] " << statusbar*2 << "%   ";
      flush(std::cout);
      statusbar++;
    }
    if(event==0){
      std::cout << "[                                                  ] " << "0%   ";
      flush(std::cout);
    }
  }
}
TH2D divideTH2DBinomial(TH2D &h1, TH2D &h2){ //! out = h1 / h2
  TH2D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX() || h1.GetNbinsY() != h2.GetNbinsY()){
    std::cout << "divideTH2DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    for(int biny=1;biny<=h1.GetNbinsY()+1;biny++){
      double cont=0;
      double err=1;
      if(h2.GetBinContent(binx,biny) !=0){
	cont=h1.GetBinContent(binx,biny) / h2.GetBinContent(binx,biny);
	err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx,biny));
      }
      out.SetBinContent(binx,biny,cont);
      out.SetBinError(binx,biny,err);
    }
  }
  return out;
}
TH2D divideTH2D(TH2D &h1, TH2D &h2){ 
TH2D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX() || h1.GetNbinsY() != h2.GetNbinsY()){
    std::cout << "divideTH2DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    for(int biny=1;biny<=h1.GetNbinsY()+1;biny++){
      double cont=0;
      double err=1;
      if(h2.GetBinContent(binx,biny) !=0){
	cont=h1.GetBinContent(binx,biny) / h2.GetBinContent(binx,biny);
	err=sqrt(pow(h1.GetBinError(binx,biny)/ h2.GetBinContent(binx,biny),2) + pow((cont / h2.GetBinContent(binx,biny)* h2.GetBinError(binx,biny)),2));
      }
      out.SetBinContent(binx,biny,cont);
      out.SetBinError(binx,biny,err);
    }
  }
  return out;
}


TH1D divideTH1DBinomial(TH1D &h1, TH1D &h2){ //! out = h1 / h2
  TH1D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX()){
    std::cout << "divideTH1DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    double cont=0;
    double err=1;
    if(h2.GetBinContent(binx) !=0){
      cont=h1.GetBinContent(binx) / h2.GetBinContent(binx);
      err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx));
    }
    out.SetBinContent(binx,cont);
    out.SetBinError(binx,err);
  }  
  return out;
}

