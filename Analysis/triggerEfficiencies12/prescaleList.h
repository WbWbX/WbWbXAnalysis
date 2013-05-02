#ifndef PRESCALELIST_H
#define PRESCALELIST_H

#include <vector>
#include <map>
#include <string>
#include <iostream>


class prescaleList{

public:
  prescaleList(){}
  ~prescaleList(){}

  /*double & getContribution(std::string name, unsigned int prsc) //name and prescale
  {
    size_t idx=999999999;
    for(size_t i=0;i<names_.size();i++){
      if(names_.at(i).find(name) != std::string::npos){
	idx=i;
	break;
      }
    }
    if(idx > 999999998)
      return 0;

  }
  */

  void fill(std::string name, unsigned int prescale, double weight=1){ // change weight to get weighted contributions
    bool isNewName=true;
    bool isNewPrescale=true;
    size_t nidx=999999;
    size_t pidx=999999;
    for(size_t i=0;i<names_.size();i++){
      if(names_.at(i).find(name) != std::string::npos){
	isNewName=false;
	nidx=i;
	for(size_t k=0;k<prescales_.at(i).size();k++){
	  if(prescale == prescales_.at(i).at(k)){
	    isNewPrescale=false;
	    pidx=k;
	    break;
	  }
	}
      }
    }
    if(isNewName){
      names_.push_back(name);

      std::vector<unsigned int> presc;
      presc.push_back(prescale);
      prescales_.push_back(presc);

      std::vector<double> contr;
      contr.push_back(weight);
      contributions_.push_back(contr);
    }
    else if(isNewPrescale){ //now new name but new prescale
      prescales_.at(nidx).push_back(prescale);
      contributions_.at(nidx).push_back(weight);
    }
    else{
      contributions_.at(nidx).at(pidx) += weight;
    }
  }

  void cout(){
    using namespace std;
    for(size_t i=0;i<names_.size();i++){
      std::cout << names_.at(i) << "\t" ;
      for(size_t j=0;j<prescales_.at(i).size();j++){
	std::cout << "ps: " << prescales_.at(i).at(j) << " #:" << contributions_.at(i).at(j) << "\t";
      }
      std::cout << std::endl;
    }
  }



  void orderByContribution(){} //does nothing yet

private:

  std::vector<std::string> names_;
  std::vector<std::vector<unsigned int> > prescales_; 
  std::vector<std::vector<double> > contributions_; 


};


#endif
