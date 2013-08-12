#include <vector>
#include <string>
#include "../interface/NTLepton.h"
#include "../interface/NTMuon.h"
#include "../interface/NTElectron.h"
#include "../interface/NTJet.h"
#include "../interface/NTMet.h"
#include "../interface/NTEvent.h"
#include "../interface/NTIsolation.h"
#include "../interface/NTTrack.h"
#include "../interface/NTSuClu.h"
#include "../interface/NTTrigger.h"
#include "../interface/NTTriggerObject.h"



#include "../interface/NTGenLepton.h"
#include "../interface/NTGenParticle.h"
#include "../interface/NTGenJet.h"

#include "../interface/NTBTagSF.h"
#include "../interface/NTJECUncertainties.h"
#include "../interface/NTJERAdjuster.h"

//backward comp

namespace top{using namespace ztop;}

namespace
{
  struct dictionary {
    
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
    


    std::map<std::string, double> ksdjsda;
    std::map<std::string, unsigned int> ksdjsssda;
    std::map<int, double> ijdfj;


    ztop::NTBTagSF ked;
    ztop::NTJECUncertainties kkss;
    ztop::NTJERAdjuster ksdsk;
    std::vector<ztop::NTBTagSF> kedV;
    std::vector<ztop::NTJECUncertainties> kkssV;
    std::vector<ztop::NTJERAdjuster> ksdskV;
    

  };




}


 
