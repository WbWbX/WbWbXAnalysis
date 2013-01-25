#ifndef GENTOOLS_H
#define GENTOOLS_H

//#define WITHOUT_NTOBJECTS

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

namespace top{

  std::vector<const reco::GenParticle *> getGenFromMother(const reco::GenParticle * mother, int Id, bool recursive=false);
  
  std::vector<const reco::GenParticle *> getGenFromMother(const reco::Candidate * mother, int Id, bool recursive=false);

  //std::vector<const reco::GenParticle *> getGenFromMother(std::vector<reco::GenParticle>::const_iterator& mother, int Id, bool recursive=false);

  
  
  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, int lepId, bool viaTau);

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, std::vector<int> &lepId, bool viaTau);
  
  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, int lepId, bool viaTau);

  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, std::vector<int> &lepId, bool viaTau);

  std::vector<const reco::GenParticle *> getFullDecayChainRecursive(const reco::GenParticle * part, int vetoId=0);

  // std::vector<const reco::GenParticle *> getAllFromMother(std::vector<const reco::GenParticle *> & allgen, int outPartId, std::vector<int> forceIntermediate, std::vector<int> excludeIntermediate){

  //   std::vector<bool> forcedFound;
  //   forcedFound.resize(forceIntermediate.size(), false)
  //   std::vector<bool> excludeFound;
  //   excludedFound.resize(excludeIntermediate.size(), false)







  // }


#ifndef WITHOUT_NTOBJECTS
//NTGenLeptons etc dependent functions


#endif
  

}

#endif
