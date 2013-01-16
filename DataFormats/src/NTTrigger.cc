#include "../interface/NTTrigger.h"



namespace top{

  std::vector<std::string>  top::NTTrigger::mapping_;

  void NTTrigger::readMapFromTree(TTree * t){
    std::vector<bool> tempfired=fired_;
    std::vector<std::string> * p = 0;
    t->SetBranchAddress("NTTriggerMaps",&p); 
    for(float i=0;i<t->GetEntries();i++){
      t->GetEntry(i);
      insert(*p);
    }
    fired_=tempfired;
  }

  void NTTrigger::writeMapToTree(TTree *t){
    std::vector<std::string> temp;
    if(t->GetBranch("NTTriggerMaps"))
      t->SetBranchAddress("NTTriggerMaps", &temp);
    else
      t->Branch("NTTriggerMaps", "std::vector<std::string>", &temp);
    temp=getMap();
    t->Fill();
  }

  void NTTrigger::insert(const std::vector<std::string> & triggerNames){ //! fill in the vector of fired triggers
    for(unsigned int i=0;i<triggerNames.size(); i++){
      insert(triggerNames.at(i));
    }
  }

  void NTTrigger::insert(const std::string & triggerName){ //! fill in fired trigger

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

  std::vector<unsigned int> NTTrigger::getIndices(const std::string & triggername){ //! returns indices of all triggers !!containing!! triggername

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

  std::vector<unsigned int> NTTrigger::getIndices(const std::vector<std::string> triggernames){ //! returns indices of all triggers !!containing!! triggernames

    std::vector<unsigned int> out;

    for(unsigned int i=0;i<triggernames.size();i++){
      std::vector<unsigned int> temp=getIndices(triggernames.at(i));
      out.insert(out.end(),temp.begin(),temp.end());
    }
    return out;
  }

  bool NTTrigger::anyFired(std::vector<unsigned int> indices){  //! returns true if any trigger associated to the list of indices has fired
    for(unsigned int i=0;i<indices.size();i++){
      if(fired(indices.at(i)))
	return true;
    }
    return false;
  }



}
