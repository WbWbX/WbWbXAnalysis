#include <vector>
#include <string>
#include "WbWbXAnalysis/DataFormats/interface/NTLepton.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMuon.h"
#include "WbWbXAnalysis/DataFormats/interface/NTElectron.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include "WbWbXAnalysis/DataFormats/interface/NTIsolation.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTrack.h"
#include "WbWbXAnalysis/DataFormats/interface/NTSuClu.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTrigger.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTriggerObject.h"


#include "WbWbXAnalysis/DataFormats/interface/NTVertex.h"

#include "WbWbXAnalysis/DataFormats/interface/NTGenLepton.h"
#include "WbWbXAnalysis/DataFormats/interface/NTGenParticle.h"
#include "WbWbXAnalysis/DataFormats/interface/NTGenJet.h"

#include "WbWbXAnalysis/DataFormats/interface/NTBTagSF.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJERAdjuster.h"
#include "WbWbXAnalysis/DataFormats/interface/mathdefs.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLorentzVector.h"

#include "WbWbXAnalysis/DataFormats/interface/NTSystWeight.h"

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


 
