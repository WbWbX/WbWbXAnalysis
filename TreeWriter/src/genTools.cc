#include "../interface/genTools.h"

namespace top{


std::vector<const reco::GenParticle *> getGenFromMother(const reco::GenParticle * mother, int Id, bool recursive){ 

    std::vector<const reco::GenParticle *> out;

    for(unsigned int i=0;i<mother->numberOfDaughters();i++){
      int daugId=mother->daughter(i)->pdgId();
      if((int)daugId == Id){
	const reco::GenParticle * tempP= dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
	out.push_back(tempP);
      }
      else if(recursive){ // recursion
	std::vector<const reco::GenParticle *> temp=getGenFromMother(dynamic_cast<const reco::GenParticle*>(mother->daughter(i)), Id, recursive);
	out.insert(out.end(),temp.begin(),temp.end());
      }
    }
    return out;
  }

  

  std::vector<const reco::GenParticle *> getGenFromMother(const reco::Candidate * mother, int Id, bool recursive){
    return getGenFromMother(dynamic_cast<const reco::GenParticle*>(mother), Id, recursive);
  }
  // std::vector<const reco::GenParticle *> getGenFromMother(std::vector<reco::GenParticle>::const_iterator& mother, int Id, bool recursive){ 
  //   return getGenFromMother(&*mother, Id, recursive);
  // }
  
  

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, int lepId, bool viaTau){
    //get tops
    std::vector<const reco::GenParticle*> Ws,Taus,Out;
    std::vector<const reco::GenParticle*> temp;
    // Fill Ws
    for(unsigned int i=0;i<input.size();i++){
      if(input.at(i)->pdgId() != 6 && input.at(i)->pdgId() != -6) continue;

      temp=getGenFromMother(input.at(i), 24 , false);
      Ws.insert(Ws.begin(),temp.begin(),temp.end());
      temp=getGenFromMother(input.at(i), -24 , false);
      Ws.insert(Ws.begin(),temp.begin(),temp.end());
    }
    //Fill Taus or direct out
    for(unsigned int i=0;i<Ws.size();i++){
      if(viaTau){
	temp=getGenFromMother(Ws.at(i), 15 , false);
	Taus.insert(Taus.begin(),temp.begin(),temp.end());
	temp=getGenFromMother(Ws.at(i), -15 , false);
	Taus.insert(Taus.begin(),temp.begin(),temp.end());
      }
      else{
	temp=getGenFromMother(Ws.at(i), lepId , false);
	Out.insert(Out.begin(),temp.begin(),temp.end());
      }
    }
    if(viaTau){
      for(unsigned int i=0;i<Taus.size();i++){
	temp=getGenFromMother(Taus.at(i), lepId , true);
	Out.insert(Out.begin(),temp.begin(),temp.end());
      }
    }
    return Out;
  }

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *>  &input, std::vector<int> &lepId, bool viaTau){
    std::vector<const reco::GenParticle *> out;
    std::vector<const reco::GenParticle *> temp;
    for(unsigned int i=0;i<lepId.size();i++){
      temp=getGenLepsFromTop(input,lepId.at(i),viaTau);
      out.insert(out.begin(),temp.begin(),temp.end());
    }
    return out;
  }

  

  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, int lepId, bool viaTau){
    std::vector<const reco::GenParticle *> Z,Taus,Out, temp;
    //get Z
    for(unsigned int i=0;i<input.size();i++){
      if(input.at(i)->pdgId() != 23) continue;
     
      if(viaTau){
	temp=getGenFromMother(input.at(i), 15 , false);
	Taus.insert(Taus.begin(),temp.begin(),temp.end());
	temp=getGenFromMother(input.at(i), -15 , false);
	Taus.insert(Taus.begin(),temp.begin(),temp.end());
      }
      else{
	temp=getGenFromMother(input.at(i), lepId , false);
	Out.insert(Out.begin(),temp.begin(),temp.end());
      }
    }
    if(viaTau){
      for(unsigned int i=0;i<Taus.size();i++){
	temp=getGenFromMother(Taus.at(i), lepId , true);
	Out.insert(Out.begin(),temp.begin(),temp.end());
      }
    }
    return Out;
  }


  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, std::vector<int> & lepId, bool viaTau){
    std::vector<const reco::GenParticle *> out;
    std::vector<const reco::GenParticle *> temp;
    for(unsigned int i=0;i<lepId.size();i++){
      temp=getGenLepsFromZ(input,lepId.at(i),viaTau);
      out.insert(out.begin(),temp.begin(),temp.end());
    }
    return out;
  }
  
}
