#include "../interface/NTBTagSF.h"


namespace ztop{
  NTBTagSF  NTBTagSF::operator + (NTBTagSF  second){
    std::map<std::string,std::vector<TH2D> >::iterator sampleit,sampleit2;//=histos_.find(samplename);
    for(sampleit=histos_.begin();sampleit!=histos_.end();++sampleit){
      std::string samplename1=sampleit->first;
      for(sampleit2=second.histos_.begin();sampleit2!=second.histos_.end();++sampleit2){
	std::string samplename2=sampleit2->first;
	if(samplename1 == samplename2){
	  std::cout << "NTBTagSF::operator +: adding efficiencies with same samplename not supported (yet)!" << std::endl;
	  return second;
	}
      }
    }
    //none of the samples are the same
    second.histos_.insert(histos_.begin(),histos_.end()); // samplename vec<hist>
    second.effhistos_.insert(effhistos_.begin(),effhistos_.end()); // samplename vec<hist>
    return second;
  }
}
