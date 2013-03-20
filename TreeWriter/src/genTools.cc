#include "../interface/genTools.h"

namespace top{



std::vector<const reco::GenParticle *> getGenFromMother(const reco::GenParticle * mother, int Id){ 

    std::vector<const reco::GenParticle *> out;

    for(unsigned int i=0;i<mother->numberOfDaughters();i++){
      int daugId=mother->daughter(i)->pdgId();
      if((int)daugId == Id){
	const reco::GenParticle * tempP= dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
	out.push_back(tempP);
      }
      
    }
    return out;
  }

  

  std::vector<const reco::GenParticle *> getGenFromMother(const reco::Candidate * mother, int Id){
    return getGenFromMother(dynamic_cast<const reco::GenParticle*>(mother), Id);
  }
  // std::vector<const reco::GenParticle *> getGenFromMother(std::vector<reco::GenParticle>::const_iterator& mother, int Id, bool recursive){ 
  //   return getGenFromMother(&*mother, Id, recursive);
  // }

  // std::vector<const reco::GenParticle *> getFullDecayChainRecursive(const reco::GenParticle * part, int vetoId){//only fill in status 1!! does it work?!? doesn't... or does? more than one mother?!?
  //   std::vector<const reco::GenParticle *> out;

  //   for(unsigned int i=0;i<part->numberOfMothers();i++){
  //     const reco::GenParticle * tempP= dynamic_cast<const reco::GenParticle*>(part->mother(i));
  //     if(vetoId){
  // 	if(tempP->pdgId() == vetoId)
  // 	  break;
  //     }
  //     out.push_back(tempP);
  //     if(tempP->numberOfMothers()>0){
  // 	std::vector<const reco::GenParticle *> temp=getFullDecayChainRecursive(tempP, vetoId);
  // 	out.insert(out.end(),temp.begin(),temp.end());
  //     }
  //     if(tempP->numberOfMothers()>1){
  // 	std::cout << "more than one mother... weird" << std::endl;
  //     }
  //   }
  //   return out;
  // }

  
  

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, int lepId){
    //get tops
    std::vector<const reco::GenParticle*> Ws,Out;
    std::vector<const reco::GenParticle*> temp;
    // Fill Ws
    for(unsigned int i=0;i<input.size();i++){
      if(fabs(fabs(input.at(i)->pdgId()) - 6) > 0.1) continue;

      temp=getGenFromMother(input.at(i), 24);
      Ws.insert(Ws.begin(),temp.begin(),temp.end());
      temp=getGenFromMother(input.at(i), -24);
      Ws.insert(Ws.begin(),temp.begin(),temp.end());
    }
    //Fill Taus or direct out
    for(unsigned int i=0;i<Ws.size();i++){
	temp=getGenFromMother(Ws.at(i), lepId);
	Out.insert(Out.begin(),temp.begin(),temp.end());
    }
    return Out;
  }

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *>  &input, std::vector<int> &lepId){
    std::vector<const reco::GenParticle *> out;
    std::vector<const reco::GenParticle *> temp;
    for(unsigned int i=0;i<lepId.size();i++){
      temp=getGenLepsFromTop(input,lepId.at(i));
      out.insert(out.begin(),temp.begin(),temp.end());
    }
    return out;
  }

  

  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, int lepId){
    std::vector<const reco::GenParticle *> Out, temp;
    //get Z
    for(unsigned int i=0;i<input.size();i++){
      if(input.at(i)->pdgId() != 23) continue; 
	temp=getGenFromMother(input.at(i), lepId);
	Out.insert(Out.begin(),temp.begin(),temp.end());  
    }
    return Out;
  }


  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, std::vector<int> & lepId){
    std::vector<const reco::GenParticle *> out;
    std::vector<const reco::GenParticle *> temp;
    for(unsigned int i=0;i<lepId.size();i++){
      temp=getGenLepsFromZ(input,lepId.at(i));
      out.insert(out.begin(),temp.begin(),temp.end());
    }
    return out;
  }
  
}
