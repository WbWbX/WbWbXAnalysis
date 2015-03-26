/*
 * testDiscrFactory.cc
 *
 *  Created on: Mar 27, 2014
 *      Author: kiesej
 */


#include "TtZAnalysis/Analysis/interface/discriminatorFactory.h"
#include "../interface/histo1D.h"
#include "../interface/histoStack.h"
#include "../interface/histoStackVector.h"
#include "../interface/plotterBase.h"

#include <iostream>
#include "TtZAnalysis/Analysis/interface/NTFullEvent.h"
#include "TRandom.h"

void setupDF(ztop::discriminatorFactory & discf, ztop::NTFullEvent& evt){

    discf.setStep(8);
    // discf.setUseLikelihoods(false);
    discf.setNBins(30);
    // discf.linkEvent(&evt);

    discf.addVariable(&evt.detallj,"#Delta#eta(ll,j)");
    discf.addVariable(&evt.dphillj,"#Delta#phi(ll,j)");
    discf.addVariable(&evt.detalljj,"#Delta#eta(ll,jj)");


    discf.setSystematics("nominal");

}


int main (){
    using namespace std;
    using namespace ztop;
    float nenetries=10e5;
    int scount=999;
    NTFullEvent evt;
    float puweight=1;
    evt.puweight=&puweight;

    float binsmax=0.1;
    std::vector<float> bins;
    for(float i=0;i<binsmax;i+=binsmax/100.) bins<<i;

  //  histoStackVector::debug=true;

  //  discriminatorFactory::debug=true;

    discriminatorFactory testread;
    testread.readFromTFile("testDiscrFact_lhds.root","testDF");
       setupDF(testread,evt);
    //dont want them in list here
       histo1D::c_list.clear();

    discriminatorFactory discf("testDF");

    setupDF(discf,evt);

  //  histoStack::debug=true;

    TRandom * r = new TRandom();
    r->SetSeed(scount++);

    /////event loop part
    float d1=1,d2=1,d3=1;
    evt.reset();
    evt.detallj=&d1;
    evt.dphillj=&d2;
    evt.detalljj=&d3;

    ///test to read in again


   histo1D::c_makelist=true;
   histo1D fullout(bins,"full out step 8");

    //generate some stuff for signal MC
    std::cout << "generating signal..." <<std::endl;
    for(size_t i=0;i<nenetries;i++){

        d1=fabs(r->Gaus(0.9,0.3));
        if(d1>1) d1=1;
        d2=fabs(r->Gaus(0.4,0.1));
        if(d2>1) d2=1;
        d3=fabs(r->BreitWigner(0.35,0.4));

        discf.fill(puweight);

        float clh=testread.getCombinedLikelihood();
        fullout.fill(clh);

    }
    TString signalname="signal";
    histoStackVector csv;
    csv.addList(signalname,kRed,1,1);
    csv.addSignal(signalname);

    fullout.clear();
    ////new loop

    histo1D::c_list.clear();
    histo1D::c_list.push_back(&fullout);

    discriminatorFactory discf2("testDF");
    setupDF(discf2,evt);

    //for BG
    std::cout << "generating background..." <<std::endl;
    r->SetSeed(scount++);
    for(size_t i=0;i<nenetries;i++){

        d1=fabs(r->Gaus(0.2,0.3));
        if(d1>1) d1=1;
        d2=fabs(r->Gaus(0.1,1));
        if(d2>1) d2=1;
        d3=fabs(r->BreitWigner(0.15,0.2));

        discf2.fill(puweight);

        float clh=testread.getCombinedLikelihood();
        fullout.fill(clh);

    }

    csv.addList("BG",kBlue,1,2);
    fullout.clear();

    ///and for pseudo data (just for nicer plotting)


    histo1D::c_list.clear();
    histo1D::c_list.push_back(&fullout);
    discriminatorFactory discf3("testDF");
    setupDF(discf3,evt);

    r->SetSeed(scount++);
    std::cout << "generating pseudo-data..." <<std::endl;
    for(size_t i=0;i<nenetries;i++){

        d1=fabs(r->Gaus(0.2,0.3));
        if(d1>1) d1=1;
        d2=fabs(r->Gaus(0.1,1));
        if(d2>1) d2=1;
        d3=fabs(r->BreitWigner(0.15,0.2));

        discf3.fill(puweight);
        float clh=testread.getCombinedLikelihood();
        fullout.fill(clh);

        d1=fabs(r->Gaus(0.9,0.3));
        if(d1>1) d1=1;
        d2=fabs(r->Gaus(0.4,0.1));
        if(d2>1) d2=1;
        d3=fabs(r->BreitWigner(0.35,0.4));

        discf3.fill(puweight);
         clh=testread.getCombinedLikelihood();
        fullout.fill(clh);

    }
  //  plotterBase::debug=true;
    csv.addList("data",kBlack,1,3);

    TFile * f= new TFile("testDiscrFact_out.root","RECREATE");
    discf3.writeToTFile(f);
    f->Close();
    delete f;
    csv.writeAllToTFile("testDiscrFact_out.root",false,false);
    fullout.clear();
    histo1D::c_list.clear();
    histo1D::c_list.push_back(&fullout);

    discriminatorFactory testcreate("testDF");
  //  histo1D::debug=true;
 //   histoStack::debug=true;
    testcreate.extractLikelihoods(csv);
    csv.clear();
    testcreate.setUseLikelihoods(true);

    f= new TFile("testDiscrFact_lhds.root","RECREATE");
    testcreate.writeToTFile(f);
    f->Close();

    setupDF(testcreate,evt);




    csv.addList("data",kBlack,1,0);
    // csv.addList("fake",kGreen,1,1);
    csv.writeAllToTFile("testDiscrFact_lh.root",true,false);

}
