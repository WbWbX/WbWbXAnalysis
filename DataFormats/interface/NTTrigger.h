#ifndef NTTRIGGER_H
#define NTTRIGGER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "TObject.h"
//#include "TFile.h"
#include "TTree.h"
/*
  for future changes:
  convert mapping vector to a map (faster lookup) use mapping_.size() as next index filled in map



*/


namespace top{

 
  class NTTrigger {
  public:
    NTTrigger(){}
    ~NTTrigger(){}

    //io options
    
    void readMapFromTree(TTree * t);
    void writeMapToTree(TTree * t);
    void insert(const std::vector<std::string> & triggerNames);
    void insert(const std::string & triggerName);
    
    //gets
    std::vector<unsigned int> getIndices(const std::string & triggername);
    std::vector<unsigned int> getIndices(const std::vector<std::string> triggernames);

    bool anyFired(std::vector<unsigned int> indices);

    bool fired(unsigned int index){  //! returns true if trigger with index has fired
      if(index>=fired_.size()) return false; 
      else return fired_.at(index);
    }

    static std::vector<std::string> & getMap(){return mapping_;}

  private:

    std::vector<bool> fired_;

    static std::vector<std::string> mapping_; // vector should be sufficient

  };

  

}
#endif
