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
