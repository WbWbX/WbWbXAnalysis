#include <vector>
#include <string>
#include "TtZAnalysis/DataFormats/interface/NTLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/DataFormats/interface/NTMet.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/NTIsolation.h"
#include "TtZAnalysis/DataFormats/interface/NTTrack.h"
#include "TtZAnalysis/DataFormats/interface/NTSuClu.h"
#include "TtZAnalysis/DataFormats/interface/NTTrigger.h"
#include "TtZAnalysis/DataFormats/interface/NTTriggerObject.h"


#include "TtZAnalysis/DataFormats/interface/NTVertex.h"

#include "TtZAnalysis/DataFormats/interface/NTGenLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTGenParticle.h"
#include "TtZAnalysis/DataFormats/interface/NTGenJet.h"

#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include "TtZAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "TtZAnalysis/DataFormats/interface/NTJERAdjuster.h"
#include "TtZAnalysis/DataFormats/interface/mathdefs.h"
#include "TtZAnalysis/DataFormats/interface/NTLorentzVector.h"

#include "TtZAnalysis/DataFormats/interface/NTSystWeight.h"

//backward comp

namespace top{using namespace ztop;}

namespace
{
  struct dictionary_dataformats {


      ztop::NTLorentzVector<float> vecfff;
      std::vector<ztop::NTLorentzVector<float> > dfwe3rel;
      std::vector<float> dsfwefewfds;
    std::vector<std::string> dummy1;
    std::vector<std::vector<std::string> > dummy2;
    ztop::NTLepton dummy5;
    std::vector<ztop::NTLepton> dummy6;
    ztop::NTMuon dummy3;
    std::vector<ztop::NTMuon> dummy4;
    ztop::NTElectron dummy7;
    std::vector<ztop::NTElectron> dummy8;
    ztop::NTJet dummy9;
    std::vector<ztop::NTJet> dummy10;
    ztop::NTMet dummy11;
    std::vector<ztop::NTMet> dummy12;
    ztop::NTEvent dummy13;
    std::vector<ztop::NTEvent> dummy14;
    ztop::NTIsolation dummy15;
    std::vector<ztop::NTIsolation> dummy16;
    ztop::NTTrack dummy17;
    std::vector<ztop::NTTrack> dummy18;
    ztop::NTSuClu dummy19;
    std::vector<ztop::NTSuClu> dummy20;
    ztop::NTGenParticle ewfik;
    std::vector<ztop::NTGenParticle> swded;
    ztop::NTGenJet ewfffffik;
    std::vector<ztop::NTGenJet> swgggfded;
    std::vector< ztop::NTGenParticle *> jfffd;
    ztop::NTGenLepton ihisdh;
    std::vector<ztop::NTGenLepton> asfkkk;
    ztop::NTTriggerObject isdhisdh;
    std::vector<ztop::NTTriggerObject> asfsdkkk;
    
    ztop::NTVertex sdfsasa;
    std::vector<ztop::NTVertex> dqwqdasdd;


    std::map<std::string, double> ksdjsda;
    std::map<std::string, float> ksdjdsfsda;
    std::map<std::string, unsigned int> ksdjsssda;
   // std::map<int, double> ijdfj;
    std::map<int, float> ijddsfsfj;
    std::vector<std::pair<std::string, float > > oisdjfoiwjefo;


    ztop::NTBTagSF ked;
    ztop::NTJECUncertainties kkss;
    ztop::NTJERAdjuster ksdsk;
    std::vector<ztop::NTJECUncertainties> kkssV;
    std::vector<ztop::NTJERAdjuster> ksdskV;
    std::vector<TObject*> kjnfjkefrerfe;
    
    ztop::NTWeight hdiufhiuhfierf;
    std::vector<ztop::NTWeight> hdiufhiuhfierfv;

  };




}


 
