#ifndef NTTRIGGER_H
#define NTTRIGGER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>



namespace top{

  class NTTrigger {
  public:
    NTTrigger(){}
    ~NTTrigger(){}

    //sets
    void insert(std::vector<std::string> triggerNames){
      for(std::vector<std::string>::iterator str=triggerNames.begin();str<triggerNames.end();++str){
	insert(*str);
      }
    }

    void insert(std::string triggerName){

      unsigned int pos=0;
      unsigned int mapsize=mapping_.size();
      bool found=false;

      for(unsigned int i=0;i<mapsize;i++){
	if(mapping_.at(i)==triggerName){
	  pos=i;
	  found=true;
	  break;
	}
      }
      if(!found){
	mapping_.push_back(triggerName);
	pos=mapsize;
	mapsize++;
      }

      if(fired_.size() < mapsize)
	fired_.resize(mapsize, false);

      fired_.at(pos)=true;

    }

    //gets
    static unsigned int getIndex(std::string triggername){ //! returns index of first trigger !!containing!! triggername

      for(unsigned int i=0;i<mapping_.size();i++){
	if(mapping_.at(i).find(triggername)!=std::string::npos)
	  return i;
      }
      std::cout << "NTTrigger::getIndex(): Trigger " << triggername << " not found!!" << std::endl;
      std::exit(EXIT_FAILURE);
    }


    bool fired(unsigned int index){
      if(index>=fired_.size()) return false; 
      else return fired_.at(index);
    }


    private:

    std::vector<bool> fired_;

    static std::vector<std::string> mapping_; // vector should be sufficient

  };

  std::vector<std::string> top::NTTrigger::mapping_;

}
#endif
