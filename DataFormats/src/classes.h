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



#include "../interface/NTGenLepton.h"
#include "../interface/NTGenParticle.h"
#include "../interface/NTGenJet.h"

#include "../interface/NTBTagSF.h"
#include "../interface/NTJECUncertainties.h"
#include "../interface/NTJERAdjuster.h"

namespace
{
  struct dictionary {

    std::vector<std::string> dummy1;
    std::vector<std::vector<std::string> > dummy2;
    top::NTLepton dummy5;
    std::vector<top::NTLepton> dummy6;
    top::NTMuon dummy3;
    std::vector<top::NTMuon> dummy4;
    top::NTElectron dummy7;
    std::vector<top::NTElectron> dummy8;
    top::NTJet dummy9;
    std::vector<top::NTJet> dummy10;
    top::NTMet dummy11;
    std::vector<top::NTMet> dummy12;
    top::NTEvent dummy13;
    std::vector<top::NTEvent> dummy14;
    top::NTIsolation dummy15;
    std::vector<top::NTIsolation> dummy16;
    top::NTTrack dummy17;
    std::vector<top::NTTrack> dummy18;
    top::NTSuClu dummy19;
    std::vector<top::NTSuClu> dummy20;
    top::NTTrigger dummy21;
    std::vector<top::NTTrigger> dummy22;



    std::map<std::string, double> ksdjsda;
    std::map<int, double> ijdfj;

    top::NTGenLepton ihisdh;
    std::vector<top::NTGenLepton> asfkkk;

    top::NTBTagSF ked;
    top::NTJECUncertainties kkss;
    top::NTJERAdjuster ksdsk;
    std::vector<top::NTBTagSF> kedV;
    std::vector<top::NTJECUncertainties> kkssV;
    std::vector<top::NTJERAdjuster> ksdskV;

    top::NTGenParticle ewfik;
    std::vector<top::NTGenParticle> swded;
    top::NTGenJet ewfffffik;
    std::vector<top::NTGenJet> swgggfded;

    std::vector< top::NTGenParticle *> jfffd;
    
  };
}

