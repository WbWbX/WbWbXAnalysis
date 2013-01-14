#ifndef NTTRIGGER_H
#define NTTRIGGER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "TObject.h"
#include "TFile.h"
/*
for future changes:
convert mapping vector to a map (faster lookup) use mapping_.size() as next index filled in map



*/


namespace top{



  class triggerMap : public TObject{

    ClassDef(triggerMap,1) ;

  public:
    triggerMap(){}
    triggerMap(std::vector<std::string> Map){map=Map;}
    ~triggerMap(){}

    // private:

    std::vector<std::string> map;

  };


  class NTTrigger {
  public:
    NTTrigger(){}
    ~NTTrigger(){}

    //io options
    void writeMap(){
      triggerMap m(mapping_);
      //m.SetName("triggerMap");
      m.Write();
    }

    void readMap(TFile *f){
      triggerMap *temp=new triggerMap();
      f->GetObject("top::triggerMap",temp);
      mapping_=temp->map;
      delete temp;
    }

    //sets
    void insert(const std::vector<std::string> & triggerNames){ //! fill in the vector of fired triggers
      for(unsigned int i=0;i<triggerNames.size(); i++){
	insert(triggerNames.at(i));
      }
    }

    void insert(const std::string & triggerName){ //! fill in fired trigger

      unsigned int pos=0;

      bool found=false;

      for(unsigned int i=0;i<mapping_.size();i++){
	if(mapping_.at(i)==triggerName){
	  pos=i;
	  found=true;
	  break;
	}
      }
      if(!found){
	pos=mapping_.size();
	mapping_.push_back(triggerName);
      }

      if(fired_.size() < mapping_.size())
	fired_.resize(mapping_.size(), false);

      fired_.at(pos)=true;

    }

    //gets
    std::vector<unsigned int> getIndices(const std::string & triggername){ //! returns indices of all triggers !!containing!! triggername

      std::vector<unsigned int> out;

      for(unsigned int i=0;i<mapping_.size();i++){
	if(mapping_.at(i).find(triggername)!=std::string::npos)
	  out.push_back(i);
      }
      if(out.size()==0){
	std::cout << "NTTrigger::getIndex(): Trigger " << triggername << " not found!!" << std::endl;
	std::exit(EXIT_FAILURE);
      }
      return out;
    }

    std::vector<unsigned int> getIndices(const std::vector<std::string> triggernames){ //! returns indices of all triggers !!containing!! triggernames

      std::vector<unsigned int> out;

      for(unsigned int i=0;i<triggernames.size();i++){
	std::vector<unsigned int> temp=getIndices(triggernames.at(i));
	out.insert(out.end(),temp.begin(),temp.end());
      }
      return out;
    }

    bool anyFired(std::vector<unsigned int> indices){  //! returns true if any trigger associated to the list of indices has fired
      for(unsigned int i=0;i<indices.size();i++){
	if(fired(indices.at(i)))
	  return true;
      }
      return false;
    }

    bool fired(unsigned int index){  //! returns true if trigger with index has fired
      if(index>=fired_.size()) return false; 
      else return fired_.at(index);
    }


    private:

    std::vector<bool> fired_;

    static std::vector<std::string> mapping_; // vector should be sufficient

  };

  std::vector<std::string>  top::NTTrigger::mapping_;

}
#endif
