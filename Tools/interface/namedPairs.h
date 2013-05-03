#ifndef PRESCALELIST_H
#define PRESCALELIST_H

#include <vector>
#include <map>
#include <string>
#include <iostream>

///if you need it for something else it might be worth templating


class namedPairs{

public:
  namedPairs(){}
  ~namedPairs(){}


  void fill(std::string name, double second, double third=1){ // change third to get thirded contributions
    bool isNewName=true;
    bool isNewSecond=true;
    size_t nidx=999999;
    size_t pidx=999999;
    for(size_t i=0;i<names_.size();i++){
      if(names_.at(i).find(name) != std::string::npos){
	isNewName=false;
	nidx=i;
	for(size_t k=0;k<seconds_.at(i).size();k++){
	  if(second == seconds_.at(i).at(k)){
	    isNewSecond=false;
	    pidx=k;
	    break;
	  }
	}
      }
    }
    if(isNewName){
      names_.push_back(name);

      std::vector<double> presc;
      presc.push_back(second);
      seconds_.push_back(presc);

      std::vector<double> contr;
      contr.push_back(third);
      thirds_.push_back(contr);
    }
    else if(isNewSecond){ //now new name but new prescale
      seconds_.at(nidx).push_back(second);
      thirds_.at(nidx).push_back(third);
    }
    else{
      thirds_.at(nidx).at(pidx) += third;
    }

  }

  void coutList(){
    using namespace std;
    double allcontr=0;
    for(size_t i=0;i<thirds_.size();i++){
      for(size_t j=0;j<thirds_.at(i).size();j++){
	if(allcontr<thirds_.at(i).at(j))
	  allcontr=thirds_.at(i).at(j);
      }
    }
    cout.setf(ios::fixed,ios::floatfield);
    std::cout.precision(3);
    for(size_t i=0;i<names_.size();i++){
      std::cout << names_.at(i) << "\n" ;
      for(size_t j=0;j<seconds_.at(i).size();j++){
	std::cout << "sec: " << seconds_.at(i).at(j) << " third:" << thirds_.at(i).at(j) << "\n";
	//
	std::cout << "[";
	int granularity=100;
	for(int g=1;g<granularity;g++){
	  if((double)g/(double)granularity <= thirds_.at(i).at(j)/allcontr)
	    std::cout << "=";
	  else
	    std::cout << " ";
	}
	std::cout << "]" << std::endl;

      }
      std::cout << std::endl;
    }
  }



  std::vector<double> * secondsByName(std::string name){
    for(size_t i=0;i<names_.size();i++){
      if(names_.at(i) == name)
	return &seconds_.at(i);
    }
    return 0;
  }
  std::vector<double> * thirdsByName(std::string name){
    for(size_t i=0;i<names_.size();i++){
      if(names_.at(i)==name)
	return &thirds_.at(i);
    }
    return 0;
  }


  void sortByThird(){} //does nothing yet

  void sortBySecond(){}

  //orderbyname
  void sortByName(){}

  //compare
  namedPairs compare(namedPairs first, namedPairs second){return first;} //output?!?

  //operator +
  namedPairs operator + (const namedPairs & L){
    namedPairs out=*this;
    for(size_t i=0;i<L.names_.size();i++){
      for(size_t j=0;j<L.seconds_.at(i).size();j++){
	out.fill(L.names_.at(i), L.seconds_.at(i).at(j), L.thirds_.at(i).at(j));
      }
    }
    return out;
  }


private:

  std::vector<std::string> names_;
  std::vector<std::vector<double> > seconds_; 
  std::vector<std::vector<double> > thirds_; 
 
 

};


#endif
