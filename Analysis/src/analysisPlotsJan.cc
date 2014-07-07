/*
 * analysisPlotsJan.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */


#include "../interface/analysisPlotsJan.h"
#include "../interface/AnalysisUtils.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{

void analysisPlotsJan::bookPlots(){
    if(!use()) return;
    using namespace std;
    using namespace ztop;

    vector<float> genmlb_bins;
    for(float i=0;i<=350;i+=10)
        genmlb_bins << i;

    vector<float> ivangen_mlbbins;
    ivangen_mlbbins << 0 << 70 << 116 << 150 <<400;
    vector<float> mlb_bins=ztop::subdivide<float>(genmlb_bins,2);
    vector<float> ivan_mlbbins=subdivide<float>(ivangen_mlbbins,5);

    Mlb=addPlot(genmlb_bins,mlb_bins,"m_lb leading unfold","M_{lb} [GeV]", "N_{evt}/GeV");

    mlbcombthresh_=165;

    mlb=addPlot(genmlb_bins,mlb_bins,"m_lb","m_{lb}* [GeV]", "N_{evt}/GeV");

    mlbmin=addPlot(genmlb_bins,mlb_bins,"m_lb min","m_{lb}^{min} [GeV]", "N_{evt}/GeV");

    mlbivansbins=addPlot(ivangen_mlbbins,ivan_mlbbins,"m_lb ivansbins","m_{lb}^{ivan} [GeV]", "N_{evt}/GeV");

    vector<float> inclbins; inclbins << 0.5 << 1.5; //vis PS, fullPS

    total=addPlot(inclbins,inclbins,"total","bin","N_{evt}");
    total->setBinByBin(true); //independent bins
    total->addTag(taggedObject::dontDivByBW_tag);


    vistotal=addPlot(inclbins,inclbins,"vis total","bin","N_{evt}");
    vistotal->setBinByBin(true); //independent bins
    vistotal->addTag(taggedObject::dontDivByBW_tag);
}


void analysisPlotsJan::fillPlotsGen(){
    if(!use()) return;
    if(!event()) return;
    using namespace std;
    using namespace ztop;

    if(event()->gentops && event()->gentops->size()>0)
        total->fillGen(1.,puweight());

    if(event()->genvisleptons3 && event()->genvisjets){
        if(event()->genvisleptons3->size() > 1 && event()->genvisjets->size()>1){ //vis PS
            vistotal->fillGen(1.,puweight());
        }
    }

    //calculate mlbs based on ME leptons
    if(event()->genvisleptons3 && event()->genbs){
        if(event()->genvisleptons3->size()>1 && event()->genbs->size()>0 ){

            //vis ps cuts on b-jets
            if(event()->genbs->at(0)->pt()>30){

                NTLorentzVector<float> bjetp4=event()->genbs->at(0)->p4();
                NTLorentzVector<float> llepp4=event()->genvisleptons3->at(0)->p4();
                NTLorentzVector<float> slepp4=event()->genvisleptons3->at(1)->p4();

                float fMlb=(bjetp4+llepp4).m();
                float fm2lb=(bjetp4+slepp4).m();
                float fmlb=fMlb;
                float fmlbmin=fMlb;
                if(fMlb>mlbcombthresh_ && fm2lb<mlbcombthresh_)  fmlb=fm2lb;
                if(fMlb>fm2lb) fmlbmin=fm2lb;

                Mlb->fillGen(fMlb,puweight());
                mlb->fillGen(fmlb,puweight());
                mlbmin->fillGen(fmlbmin,puweight());

                mlbivansbins->fillGen(fmlb,puweight());
            }
        }
    }


}

void analysisPlotsJan::fillPlotsReco(){
    if(!use()) return;
    if(!event()) return;
    using namespace std;
    using namespace ztop;

    total->fillReco(1,puweight());

    vistotal->fillReco(1,puweight());


    /*
     * this is not an event selection. just safety measures
     */
    if(event()->leadinglep && event()->secleadinglep && event()->selectedbjets ){
        if(event()->selectedbjets->size()>0 ){
            NTLorentzVector<float> bjetp4=event()->selectedbjets->at(0)->p4();
            NTLorentzVector<float> llepp4=event()->leadinglep->p4();
            NTLorentzVector<float> slepp4=event()->secleadinglep->p4();

            float fMlb=(bjetp4+llepp4).m();
            float fm2lb=(bjetp4+slepp4).m();
            float fmlb=fMlb;
            float fmlbmin=fMlb;
            if(fMlb>mlbcombthresh_ && fm2lb<mlbcombthresh_)  fmlb=fm2lb;
            if(fMlb>fm2lb) fmlbmin=fm2lb;

            Mlb->fillReco(fMlb,puweight());
            mlb->fillReco(fmlb,puweight());
            mlbmin->fillReco(fmlbmin,puweight());

            mlbivansbins->fillReco(fmlb,puweight());
        }
    }


}

}//ns
