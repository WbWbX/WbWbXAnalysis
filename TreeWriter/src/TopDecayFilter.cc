#include <memory>  

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TtZAnalysis/TreeWriter/interface/topDecaySelector.h"
#include <vector>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "../../DataFormats/interface/mathdefs.h"

#include "TopAnalysis/ZTopUtils/interface/consumeTemplate.h"

/**
  \class   TopDecayFilter TopDecayFilter.cc "TtZAnalysis/Treewriter/src/TopDecayFilter.cc"

  \brief   EDFilter to select the decay of ttbar events on generator level
 
*/



using namespace std;



class TopDecayFilter : public FilterTemplate {
 public:
  /// default constructor
  explicit TopDecayFilter(const edm::ParameterSet&);
  /// default destructor
  ~TopDecayFilter();
  
 private:
  /// read in which channels are going to be selected and print out
  virtual void beginJob();
  /// the real filtering procedure is implemendted in analyze()
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  /// gives a summary how many events have been selected
  virtual void endJob();
   
  /// finds out if event passes or is blocked. Remark: if invert_selection is set as true in 
  /// config then the meaning of blocked and passed swapes
  bool analyze(edm::Event&, const edm::EventSetup&);
  
  /// as source the gen particle collection shoud be used
  edm::InputTag src_;
  /// bool from config: invert the selection?
  bool invert_;   
  
      
};



TopDecayFilter::TopDecayFilter(const edm::ParameterSet& cfg) :
  src_              (cfg.getParameter<edm::InputTag>("src"       )),
  invert_           (cfg.getParameter<bool>("invert"   ))              
{
	consumeTemplate<reco::GenParticleCollection>(src_);
}



TopDecayFilter::~TopDecayFilter() { 
}



bool TopDecayFilter::filter(edm::Event& evt, const edm::EventSetup& es) {
    bool result = analyze( evt, es);
   
    if (!invert_)return result;
    else return ! result;
}



bool TopDecayFilter::analyze(edm::Event& evt, const edm::EventSetup& es) {  

    using namespace edm;
    using namespace ztop;
    using namespace std;

    edm::Handle <reco::GenParticleCollection> genParticles;
    evt.getByLabel(src_, genParticles);
    std::vector<const reco::GenParticle *> allgen, leps;
    for(size_t i=0;i<genParticles->size();i++){
        allgen << &genParticles->at(i);
    }
    std::vector<const reco::GenParticle *>* allgenForDec(&allgen);
    ztop::topDecaySelector * topDecay = new topDecaySelector();
    topDecay->setGenCollection(allgenForDec);
    ztop::topDecaySelector::partonShowers ps;
    ps = ztop::topDecaySelector::ps_pythia8;
    topDecay->setPartonShower(ps);
    topDecay->process();
    leps=topDecay->getFinalStateLeptonsFromW();
    int elcounter= 0;
    int mucounter = 0;
    for(size_t i=0;i<leps.size();i++){
        if(std::abs(leps.at(i)->pdgId()) == 11) elcounter ++;
        else if(std::abs(leps.at(i)->pdgId()) == 13) mucounter ++;
    }
    if(mucounter >= 1 && elcounter >= 1) return 1;
    else return 0;
} 


void TopDecayFilter::beginJob() {
  
}



void TopDecayFilter::endJob() {  
}



#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TopDecayFilter);
