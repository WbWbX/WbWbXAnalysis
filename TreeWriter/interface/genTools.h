#ifndef GENTOOLS_H
#define GENTOOLS_H

//#define WITHOUT_NTOBJECTS

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

namespace top{

  std::vector<const reco::GenParticle *> getGenFromMother(const reco::GenParticle * mother, int Id);
  
  std::vector<const reco::GenParticle *> getGenFromMother(const reco::Candidate * mother, int Id);

  //std::vector<const reco::GenParticle *> getGenFromMother(std::vector<reco::GenParticle>::const_iterator& mother, int Id, bool recursive=false);


  //  const reco::GenParticle * searchMother(const reco::GenParticle * p, std::vector<const reco::GenParticle *> & possibilities);
  
  // std::vector<const reco::GenParticle * > searchaughters(const reco::GenParticle * p, std::vector<const reco::GenParticle *> & possibilities);
  
  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, int lepId);

  std::vector<const reco::GenParticle *> getGenLepsFromTop(std::vector<const reco::GenParticle *> &input, std::vector<int> &lepId);
  
  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, int lepId);

  std::vector<const reco::GenParticle *> getGenLepsFromZ(std::vector<const reco::GenParticle *> & input, std::vector<int> &lepId);



#ifndef WITHOUT_NTOBJECTS
//NTGenLeptons etc dependent functions


#endif
  

}

#endif
