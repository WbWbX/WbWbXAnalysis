/*
 * eventLoop.h
 *
 *  Created on: Jul 17, 2013
 *      Author: kiesej
 *
 *
 *      THIS IS NOT REALLY A HEADER FILE
 *
 *      it only defines the analyze() function of MainAnalyzer and is put in a
 *      separate file only for structure reasons!
 *      Do not include it in any other file than MainAnalyzer.cc
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "../interface/MainAnalyzer.h"
#include "../interface/AnalysisUtils.h"
#include "../interface/NTFullEvent.h"
#include "../interface/ttbarControlPlots.h"
#include "../interface/ZControlPlots.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"

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
#include "TtZAnalysis/DataFormats/interface/NTLorentzVector.h"

#include "../interface/analysisPlotsJan.h"


/*
 * Running on the samples is parallelized.
 * This function is called for each sample individually.
 *
 * Do not just return from this function, you must give a report to
 * the parent process, why and that the function returned.
 *
 * This is automatically done after the event loop during the process of writing
 * output.
 * In case you want to indicate an abort due to an error, use the following syntax
 * if(error){
 *    //cout something in addition or so...
 *    reportError(<error code>, anaid);
 *    return;
 * }
 * This way the main program knows what happened and indicates that in the summary.
 * If you do not return this way, the main program will be stuck in an infinite loop.
 * (Sleeping 99.99% of the time)
 * Error codes will always be negative. Positive values are converted
 *
 * Please indicate the meaning of the error code in the cout at the end of ../app_src/analyse.cc
 */
void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm,size_t legord, size_t anaid){

    bool doLargeAcceptance=false;
    /*means right now:
     *
     * leps: 20/10
     * jets: 30/20
     *
     */

    using namespace std;
    using namespace ztop;

    TString mettype="NTMet";
    TString electrontype="NTPFElectrons";

    //some mode options
    /* implement here not to overload MainAnalyzer class with private members
     *  they are even allowed to overwrite existing configuration
     *  with the main purpose of trying new things
     *
     *  each option has to start with a CAPITAL letter and must not contain other capital letters
     *  This allows for parsing of more than one option at once
     *
     *  These options should get a cleanup once in a while
     */



    bool mode_samesign=false;
    if(mode_.Contains("Samesign")){
        mode_samesign=true;
        std::cout << "entering same sign mode" <<std::endl;
    }

    bool mode_invertiso=false;
    if(mode_.Contains("Invertiso")){
        mode_invertiso=true;
        std::cout << "entering invert iso mode" <<std::endl;
    }
    bool usemvamet=false;
    if(mode_.Contains("Mvamet")){
        mettype="NTMvaMet";
        usemvamet=true;
        std::cout << "entering mvamet mode" <<std::endl;
    }
    if(mode_.Contains("Pfmet")){
        mettype="NTMet";
        std::cout << "entering pfmet mode" <<std::endl;
    }
    if(mode_.Contains("T0t1txymet")){
        mettype="NTT0T1TxyMet";
        std::cout << "entering t0t1txymet mode" <<std::endl;
    }
    if(mode_.Contains("T0t1met")){
        mettype="NTT0T1Met";
        std::cout << "entering t0t1met mode" <<std::endl;
    }
    if(mode_.Contains("T1txymet")){
        mettype="NTT1TxyMet";
        std::cout << "entering t1txymet mode" <<std::endl;
    }
    if(mode_.Contains("Gsfelectrons")){
        electrontype="NTElectrons";
        std::cout << "entering gsfelectrons mode" <<std::endl;
    }
    if(mode_.Contains("Pfelectrons")){
        electrontype="NTPFElectrons";
        std::cout << "entering pfelectrons mode" <<std::endl;
    }

    if(mode_.Contains("Btagcsvt")){
        getBTagSF()->setWorkingPoint("csvt");
        std::cout << "entering btagcsvt mode" <<std::endl;
    }
    if(mode_.Contains("Btagcsvm")){
        getBTagSF()->setWorkingPoint("csvm");
        std::cout << "entering btagcsvm mode" <<std::endl;
    }
    bool onejet=false;
    if(mode_.Contains("Onejet")){
        onejet=true;
        std::cout << "entering Onejet mode" <<std::endl;
    }
    bool twodiffbtags=false;
    if(mode_.Contains("Twodiffbtags")){
        twodiffbtags=true;
        std::cout << "entering Twodiffbtags mode" <<std::endl;
    }
    bool usetopdiscr=false;
    if(mode_.Contains("Topdiscr")){
        usetopdiscr=true;
        std::cout << "entering Topdiscr mode" <<std::endl;
    }

    bool issignal=issignal_.at(anaid);

    bool isMC=true;
    if(legendname==dataname_) isMC=false;



    //per sample configuration


    //check if file exists
    if(testmode_)
        std::cout << "testmode("<< anaid << "): check input file "<<inputfile << " (isMC)"<< isMC << std::endl;

    TFile *f;
    if(!fileExists((datasetdirectory_+inputfile).Data())){
        std::cout << datasetdirectory_+inputfile << " not found!!" << std::endl;
        reportError(-1,anaid);
        return;
    }
    else{
        f=TFile::Open(datasetdirectory_+inputfile);
    }


    //define containers here

    //   define some norm containers



    if(testmode_)
        std::cout << "testmode("<< anaid << "): preparing container1DUnfolds" << std::endl;

    //////////////analysis plots/////////////


    analysisPlotsJan jansplots_step8(8);
    if((TString)getenv("USER") == (TString)"kiesej"){
        jansplots_step8.enable();
    }
    /*   if((TString)getenv("USER") == (TString)"dolinska"){
        jansplots_step8.enable();
    } */

    jansplots_step8.bookPlots();

    //global settings for analysis plots
    container1DUnfold::setAllListedMC(isMC);
    container1DUnfold::setAllListedLumi((float)lumi_);


    //setup everything for control plots
    NTFullEvent evt;
    ttbarControlPlots plots;//for the Z part just make another class (or add a boolian)..
    ZControlPlots zplots;
    plots.setEvent(evt);
    zplots.setEvent(evt);
    plots.initSteps(8);
    zplots.initSteps(8);
    jansplots_step8.setEvent(evt);


    //get normalization

    norm=createNormalizationInfo(f,isMC,anaid);

    /////////////////////////// configure scalefactors ////


    //init b-tag scale factor utility
    if(testmode_)
        std::cout << "testmode("<< anaid << "): preparing btag SF" << std::endl;

    //make sure the nominal scale factors are used for varations of the SF
    TString btagSysAdd=topmass_+"_"+getSyst();
    if(getBTagSF()->getSystematic() != bTagBase::nominal)
        btagSysAdd=topmass_+"_nominal";

    if(getBTagSF()->setSampleName((channel_+"_"+btagSysAdd+"_"+toString(inputfile)).Data()) < 0){
        reportError(-3,anaid);
        return;
    }
    getBTagSF()->setIsMC(isMC);

    ////TEST
    //another btag SF util for tight working point
    // NTBTagSF medBTagSF = *getBTagSF();
    // std::cout << "the following message does NOT apply to the normal selection!" << std::endl;
    // medBTagSF.setWorkingPoint("csvm");


    //range check switched off because of different ranges in bins compared to diff Xsec (leps)
    getTriggerSF()->setRangeCheck(false);
    getElecSF()->setRangeCheck(false);
    getMuonSF()->setRangeCheck(false);
    getElecSF()->setIsMC(isMC);
    getMuonSF()->setIsMC(isMC);
    getTriggerSF()->setIsMC(isMC);

    //some global checks
    getElecEnergySF()->setRangeCheck(false);
    getMuonEnergySF()->setRangeCheck(false);
    getElecEnergySF()->setIsMC(isMC);
    getMuonEnergySF()->setIsMC(isMC);



    /////////open main tree and prepare branches / collections


    TTree * t = (TTree*) f->Get("PFTree/PFTree");

    TBranch * b_TriggerBools=0;
    std::vector<bool> * p_TriggerBools=0;
    t->SetBranchAddress("TriggerBools",&p_TriggerBools, &b_TriggerBools);
    TBranch * b_Electrons=0;
    vector<NTElectron> * pElectrons = 0;
    t->SetBranchAddress(electrontype,&pElectrons,&b_Electrons);
    TBranch * b_Muons=0;
    vector<NTMuon> * pMuons = 0;
    t->SetBranchAddress("NTMuons",&pMuons, &b_Muons);
    TBranch * b_Jets=0;         // ##TRAP##
    vector<NTJet> * pJets=0;
    t->SetBranchAddress("NTJets",&pJets, &b_Jets);
    TBranch * b_Met=0;
    NTMet * pMet = 0;
    t->SetBranchAddress(mettype,&pMet,&b_Met);
    TBranch * b_Event=0;
    NTEvent * pEvent = 0;
    t->SetBranchAddress("NTEvent",&pEvent,&b_Event);
    TBranch * b_GenTops=0;
    vector<NTGenParticle> * pGenTops=0;
    t->SetBranchAddress("NTGenTops",&pGenTops,&b_GenTops);
    // TBranch * b_GenWs=0;
    TBranch * b_GenZs=0;
    vector<NTGenParticle> * pGenZs=0;
    t->SetBranchAddress("NTGenZs",&pGenZs,&b_GenZs);
    // TBranch * b_GenBs=0;
    TBranch * b_GenBHadrons=0;
    vector<NTGenParticle> * pGenBHadrons=0;
    t->SetBranchAddress("NTGenBHadrons",&pGenBHadrons,&b_GenBHadrons);
    TBranch * b_GenLeptons3=0;
    vector<NTGenParticle> *pGenLeptons3 =0;
    t->SetBranchAddress("NTGenLeptons3",&pGenLeptons3,&b_GenLeptons3);
    TBranch * b_GenLeptons1=0;
    vector<NTGenParticle> *pGenLeptons1 =0;
    t->SetBranchAddress("NTGenLeptons1",&pGenLeptons1,&b_GenLeptons1);
    TBranch * b_GenJets=0;
    vector<NTGenJet> * pGenJets=0;
    t->SetBranchAddress("NTGenJets",&pGenJets,&b_GenJets);
    TBranch * b_GenNeutrinos=0;
    vector<NTGenParticle> * pGenNeutrinos=0;
    t->SetBranchAddress("NTGenNeutrinos",&pGenNeutrinos,&b_GenNeutrinos);


    //some helpers
    double sel_step[]={0,0,0,0,0,0,0,0,0};
    float count_samesign=0;


    size_t pdfwidx=0;
    if(usepdfw_>-1) pdfwidx=usepdfw_;

    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////// start main loop /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////

    if(testmode_)
        std::cout << "testmode("<< anaid << "): starting main loop" << std::endl;

    Long64_t nEntries=t->GetEntries();
    if(norm==0) nEntries=0; //skip for norm0
    if(testmode_) nEntries*=0.08;
    for(Long64_t entry=0;entry<nEntries;entry++){


        ////////////////////////////////////////////////////
        ////////////////////  INIT EVENT ///////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////


        size_t step=0;
        evt.reset();
        evt.event=pEvent;
        container1DUnfold::flushAllListed(); //important to call each event

        //reports current status to parent
        reportStatus(entry,nEntries,anaid);

        b_Event->GetEntry(entry);
        float puweight=1;
        if (isMC) puweight = getPUReweighter()->getPUweight(pEvent->truePU());
        if(testmode_ && entry==0)
            std::cout << "testmode("<< anaid << "): got first event entry" << std::endl;

        evt.puweight=&puweight;
        if(usepdfw_>-1 && isMC){
            //check once
            if(entry<1){
                if(pdfwidx >= pEvent->PDFWeightsSize()){
                    throw std::out_of_range("PDF weight index not found");
                }
            }

            puweight*=pEvent->PDFWeight(pdfwidx);
        }

        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////// Generator Information////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////

        ////define all collections
        // do not move somewhere else!

        vector<NTGenParticle*> gentops;
        vector<NTGenParticle *> genleptons1,genleptons3;
        vector<NTGenJet *> genjets;
        vector<NTGenParticle *> genbhadrons;
        vector<NTGenJet *> genbjetsfromtop;
        vector<NTGenParticle *> genvisleptons1,genvisleptons3;
        vector<NTGenJet *> genvisjets;
        vector<NTGenJet *> genvisbjetsfromtop;


        evt.genleptons1=&genleptons1;
        evt.genleptons3=&genleptons3;
        evt.genjets=&genjets;
        evt.genvisleptons1=&genvisleptons1;
        evt.genvisleptons3=&genvisleptons3;
        evt.genvisjets=&genvisjets;
        evt.genvisbjetsfromtop=&genvisbjetsfromtop;

        if(isMC){

            if(testmode_ && entry==0)
                std::cout << "testmode("<< anaid << "): got first MC gen entry" << std::endl;

            b_GenLeptons3->GetEntry(entry);
            if(pGenLeptons3->size()>1){ //gen info there

                b_GenTops->GetEntry(entry);
                b_GenBHadrons->GetEntry(entry);
                b_GenJets->GetEntry(entry);
                b_GenLeptons1->GetEntry(entry);
                //recreate mother daughter relations?!
                b_GenBHadrons->GetEntry(entry);
                b_GenJets->GetEntry(entry);
                b_GenLeptons1->GetEntry(entry);
                ///////////////TOPPT REWEIGHTING////////


                if(pGenTops->size()>1){ //ttbar sample
                    puweight *= sqrt(getTopPtReweighter()->getWeight(pGenTops->at(0).pt()) *
                            getTopPtReweighter()->getWeight(pGenTops->at(1).pt()));
                    gentops.push_back(&pGenTops->at(0));
                    gentops.push_back(&pGenTops->at(0));
                    evt.gentops=&gentops;
                }

                if(testmode_ && entry==0){
                    std::cout << "testmode("<< anaid << "): entered signal genInfo part" << std::endl;
                }


                //only format change
                genleptons1=produceCollection<NTGenParticle>(pGenLeptons1);
                genleptons3=produceCollection<NTGenParticle>(pGenLeptons3);
                genjets=produceCollection<NTGenJet>(pGenJets);


                //define visible phase space
                float ps_ptlepmin=20;
                float ps_etalmax=2.5;

                float ps_ptjetmin=30;
                float ps_etajetmax=2.5;



                for(size_t i=0;i<genleptons1.size();i++){
                    NTGenParticle * lep=(genleptons1.at(i));
                    if(lep->pt()>ps_ptlepmin && fabs(lep->eta())<ps_etalmax)
                        genvisleptons1.push_back(lep);
                }
                for(size_t i=0;i<genleptons3.size();i++){
                    NTGenParticle * lep=(genleptons3.at(i));
                    if(lep->pt()>ps_ptlepmin && fabs(lep->eta())<ps_etalmax)
                        genvisleptons3.push_back(lep);
                }

                for(size_t i=0;i<genjets.size();i++){
                    NTGenJet * genjet=(genjets.at(i));
                    if(genjet->pt()>ps_ptjetmin && fabs(genjet->eta())<ps_etajetmax)
                        genvisjets.push_back(genjet);
                }

                //the mothers are only filled if b hadrons originate from a b that itself originates from a top,
                //so its sufficient to check whether there is any mother iterator
                std::vector<int> bhadids;
                for(size_t i=0;i<pGenBHadrons->size();i++){
                    NTGenParticle * bhad=&pGenBHadrons->at(i);
                    if(bhad->motherIts().size()>0)
                        bhadids<<bhad->genId();

                }

                NTLorentzVector<float>  p4genbjet;

                for(size_t i=0;i<genjets.size();i++){
                    NTGenJet * genjet=genjets.at(i);
                    if(genjet->motherIts().size()>0){
                        int motherid=genjet->motherIts().at(0);
                        if(std::find(bhadids.begin(),bhadids.end(),motherid) != bhadids.end()){
                            genbjetsfromtop << genjet;
                        }
                    }
                }
                for(size_t i=0;i<genbjetsfromtop.size();i++){
                    NTGenJet * bjet=genbjetsfromtop.at(i);
                    if(bjet->pt()<30) continue;
                    if(fabs(bjet->eta()) >2.5) continue;
                    genvisbjetsfromtop << bjet;
                }


            }
            /*
             * fill gen info here
             */
            jansplots_step8.fillPlotsGen();

        } /// isMC ends

        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        ///////////////////       Reco Part      ////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////



        /*
         *  Trigger
         */
        b_TriggerBools->GetEntry(entry);
        if(testmode_ && entry==0)
            std::cout << "testmode("<< anaid << "): got trigger boolians" << std::endl;
        if(!checkTrigger(p_TriggerBools,pEvent, isMC,anaid)) continue;


        /*
         * Muons
         */
        b_Muons->GetEntry(entry);


        vector<NTLepton *> allleps;
        std::vector<NTLepton *> isoleptons;
       // TBI std::vector<NTLepton *> vetoleps;
        evt.allleptons=&allleps;
        evt.isoleptons=&isoleptons;

        vector<NTMuon*> kinmuons,idmuons,isomuons;
        evt.kinmuons=&kinmuons;
        evt.idmuons=&idmuons;
        evt.isomuons=&isomuons;

        bool gotfirst=false;
        for(size_t i=0;i<pMuons->size();i++){
            NTMuon* muon = & pMuons->at(i);
            if(isMC)
                muon->setP4(muon->p4() * getMuonEnergySF()->getScalefactor(muon->eta()));
            allleps << muon;
            if(!gotfirst && muon->pt() < 20)       continue;
            if(doLargeAcceptance) gotfirst=true;
            if(muon->pt() < 10) continue;
            if(fabs(muon->eta())>2.4) continue;
            kinmuons << &(pMuons->at(i));
            ///select id muons
            if(!(muon->isGlobal() || muon->isTracker()) ) continue;
            //try with tight muons

            if(muon->isGlobal()
                    && muon->normChi2()<10.
                    && muon->muonHits()>0
                    && muon->matchedStations() >1  //not in trees
                    && fabs(muon->d0V())<0.2
                    && fabs(muon->dzV())<0.2
                    && muon->pixHits()>0
                    && muon->trkHits()>5){
                idmuons <<  &(pMuons->at(i));
                continue; //no double counting
            }
            //usetight=false;

            ///end tight
          //  loosemuons <<  &(pMuons->at(i));
        }



        for(size_t i=0;i<idmuons.size();i++){
            NTMuon * muon =  idmuons.at(i);
            if(!mode_invertiso && muon->isoVal() > 0.15) continue;
            if(mode_invertiso && muon->isoVal() < 0.15) continue;
            isomuons <<  muon;
            isoleptons << muon;
        }

        /*
         * Electrons
         */

        b_Electrons->GetEntry(entry);

        vector<NTElectron *> kinelectrons,idelectrons,isoelectrons;
        evt.kinelectrons=&kinelectrons;
        evt.idelectrons=&idelectrons;
        evt.isoelectrons=&isoelectrons;
        gotfirst=false;
        for(size_t i=0;i<pElectrons->size();i++){
            NTElectron * elec=&(pElectrons->at(i));
            float ensf=1;
            if(isMC)
                ensf=getElecEnergySF()->getScalefactor(elec->eta());

            elec->setECalP4(elec->ECalP4() * ensf);
            elec->setP4(elec->ECalP4() * ensf); //both the same now!!
            allleps << elec;
            if(!gotfirst && elec->pt() < 20)  continue;
            if(doLargeAcceptance) gotfirst=true;
            if( elec->pt() < 10) continue;
            float abseta=fabs(elec->eta());
            if(abseta > 2.5) continue;
            kinelectrons  << elec;

            ///select id electrons
            if(fabs(elec->d0V()) >0.04 ) continue;
            if(!(elec->isNotConv()) ) continue;
            //
            // from https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA
            if(abseta < 0.8){
                if(elec->mvaId() < 0.94) continue;}
            else if(abseta < 1.479){
                if(elec->mvaId() < 0.85) continue;}
            else if(abseta < 2.5){
                if(elec->mvaId() < 0.92) continue;}

            if(elec->mHits() > 0) continue;

            idelectrons <<  elec;

            //select iso electrons
            if(!mode_invertiso && elec->rhoIso()>0.15) continue;
            if(mode_invertiso && elec->rhoIso()<0.15) continue;
            isoelectrons <<  elec;
            isoleptons << elec;
        }

        /*
         * make all lepton collection (merged muons and electrons)
         */
        std::sort(allleps.begin(),allleps.end(), comparePt<ztop::NTLepton*>);




        /*
         * Step 0 after trigger and ntuple cuts on leptons
         */
        sel_step[0]+=puweight;
        plots.makeControlPlots(step);
        zplots.makeControlPlots(step);


        //////////two ID leptons STEP 1///////////////////////////////
        step++;


        if(b_ee_ && idelectrons.size() < 2) continue;
        if(b_mumu_ && (idmuons.size() < 2 )) continue;
        if(b_emu_ && (idmuons.size() + idelectrons.size() < 2 )) continue;

        sel_step[1]+=puweight;
        plots.makeControlPlots(step);
        zplots.makeControlPlots(step);


        //////// require two iso leptons  STEP 2  //////////////////////////
        step++;

        if(b_ee_ && isoelectrons.size() < 2) continue;
        if(b_mumu_ && isomuons.size() < 2 ) continue;
        if(b_emu_ && isomuons.size() + isoelectrons.size() < 2) continue;

        //make pair
        pair<vector<NTElectron*>, vector<NTMuon*> > oppopair,sspair;
        oppopair = ztop::getOppoQHighestPtPair(isoelectrons, isomuons);
        sspair = ztop::getOppoQHighestPtPair(isoelectrons, isomuons,-1);

        pair<vector<NTElectron*>, vector<NTMuon*> > *leppair=&oppopair;
        if(mode_samesign)
            leppair=&sspair;

        //fast count_samesign counting
        if(b_ee_ && sspair.first.size() >1)
            count_samesign+=puweight;
        else if(b_mumu_&&sspair.second.size() > 1)
            count_samesign+=puweight;
        else if(b_emu_ && sspair.first.size() > 0 && sspair.second.size() > 0)
            count_samesign+=puweight;

        float mll=0;
        NTLorentzVector<float>  dilp4;

        /*
         * prepare dilepton pairs
         *
         */

        NTLepton * firstlep=0,*seclep=0, *leadingptlep=0, *secleadingptlep=0;
        double lepweight=1;
        if(b_ee_){
            if(leppair->first.size() <2) continue;
            dilp4=leppair->first[0]->p4() + leppair->first[1]->p4();
            mll=dilp4.M();
            firstlep=leppair->first[0];
            seclep=leppair->first[1];
            lepweight*=getElecSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
            lepweight*=getElecSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
            lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
        }
        else if(b_mumu_){
            if(leppair->second.size() < 2) continue;
            dilp4=leppair->second[0]->p4() + leppair->second[1]->p4();
            mll=dilp4.M();
            firstlep=leppair->second[0];
            seclep=leppair->second[1];
            lepweight*=getMuonSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
            lepweight*=getMuonSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
            lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
        }
        else if(b_emu_){
            if(leppair->first.size() < 1 || leppair->second.size() < 1) continue;
            dilp4=leppair->first[0]->p4() + leppair->second[0]->p4();
            mll=dilp4.M();
            firstlep=leppair->first[0];
            seclep=leppair->second[0];
            lepweight*=getElecSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
            lepweight*=getMuonSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
            lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
        }
        //channel defined
        if(firstlep->pt() > seclep->pt()){
            leadingptlep=firstlep;
            secleadingptlep=seclep;
        }
        else{
            leadingptlep=seclep;
            secleadingptlep=firstlep;
        }
        //just to avoid warnings
        evt.leadinglep=leadingptlep;
        evt.secleadinglep=secleadingptlep;
        evt.mll=&mll;

        float leplepdr=dR_3d(leadingptlep->p4(),secleadingptlep->p4());
        evt.leplepdr=&leplepdr;
        float cosleplepangle=cosAngle_3d(leadingptlep->p4(),secleadingptlep->p4());
        evt.cosleplepangle=&cosleplepangle;

        puweight*=lepweight;

        //just a quick faety net against very weird weights
        if(isMC && fabs(puweight) > 99999){
            reportError(-88,anaid);
            return;
        }

        sel_step[2]+=puweight;
        plots.makeControlPlots(step);
        zplots.makeControlPlots(step);



        ///////// 20 GeV cut /// STEP 3 ///////////////////////////////////////
        step++;

        if(mll < 20)
            continue;

        // create jec jets for met and ID jets
        // create ID Jets and correct JER
        b_Jets->GetEntry(entry);

        vector<NTJet *> treejets,idjets,medjets,hardjets;
        evt.idjets=&idjets;
        evt.medjets=&medjets;
        evt.hardjets=&hardjets;
        for(size_t i=0;i<pJets->size();i++){
            treejets << &(pJets->at(i));
        }

        double dpx=0;
        double dpy=0;
        gotfirst=false;
        for(size_t i=0;i<treejets.size();i++){ //ALSO THE RESOLUTION AFFECTS MET. HERE INTENDED!!! GOOD?
            NTLorentzVector<float>  oldp4=treejets.at(i)->p4();
            if(isMC){// && !is7TeV_){
                bool useJetForMet=false;
                if(treejets.at(i)->emEnergyFraction() < 0.9 && treejets.at(i)->pt() > 10)
                    useJetForMet=true; //dont even do something

                getJECUncertainties()->applyToJet(treejets.at(i));
                getJERAdjuster()->correctJet(treejets.at(i));
                //corrected
                if(useJetForMet){
                    dpx += oldp4.Px() - treejets.at(i)->p4().Px();
                    dpy += oldp4.Py() - treejets.at(i)->p4().Py();
                }
            }
            if(!(treejets.at(i)->id())) continue;
            if(!noOverlap(treejets.at(i), isomuons,     0.4)) continue;
            if(!noOverlap(treejets.at(i), isoelectrons, 0.4)) continue;
            if(fabs(treejets.at(i)->eta())>2.5) continue;
            if(treejets.at(i)->pt() < 10) continue;
            idjets << (treejets.at(i));

            if(treejets.at(i)->pt() < 10) continue;
            medjets << treejets.at(i);
            if(treejets.at(i)->pt() < 30) continue;
            hardjets << treejets.at(i);
        }
        //jets
        //		std::sort
        std::sort(idjets.begin(),idjets.end(), comparePt<ztop::NTJet*>);
        std::sort(medjets.begin(),medjets.end(), comparePt<ztop::NTJet*>);
        std::sort(hardjets.begin(),hardjets.end(), comparePt<ztop::NTJet*>);



        b_Met->GetEntry(entry);

        NTMet adjustedmet = *pMet;
        evt.simplemet=pMet;
        evt.adjustedmet=&adjustedmet;
        double nmpx=pMet->p4().Px() + dpx;
        double nmpy=pMet->p4().Py() + dpy;
        if(!usemvamet)
            adjustedmet.setP4(D_LorentzVector(nmpx,nmpy,0,sqrt(nmpx*nmpx+nmpy*nmpy))); //COMMENTED FOR MVA MET

        ///////////////combined variables////////////

        float dphillj=M_PI,dphilljj=M_PI,detallj=0,detalljj=0;
        float pi=M_PI;
        bool midphi=false;

        if(hardjets.size() >0){
            dphillj=fabs( (leadingptlep->p4()+secleadingptlep->p4()).Phi() - hardjets.at(0)->p4().Phi() );
            dphillj=pi-fabs(dphillj-pi);
            evt.dphillj=&dphillj;
            evt.midphi=&midphi;
            detallj=(leadingptlep->p4()+secleadingptlep->p4()).Eta() - hardjets.at(0)->p4().Eta();
            evt.detallj=&detallj;
        }



        if(hardjets.size() >1){
            dphilljj=fabs( (leadingptlep->p4()+secleadingptlep->p4()).Phi() - (hardjets.at(0)->p4()+hardjets.at(1)->p4()).Phi() );
            dphilljj=pi-fabs(dphilljj-pi);
            evt.dphilljj=&dphilljj;
            detalljj=(leadingptlep->p4()+secleadingptlep->p4()).Eta() - (hardjets.at(0)->p4()+hardjets.at(1)->p4()).Eta();
            evt.detalljj=&detalljj;
        }

        float ptllj=leadingptlep->pt()+secleadingptlep->pt();
        evt.ptllj=&ptllj;
        if(hardjets.size() >0)
            ptllj+=hardjets.at(0)->pt();





        vector<NTJet*> hardbjets;
        evt.hardbjets=&hardbjets;
        vector<NTJet*> medbjets;
        evt.medbjets=&medbjets;

        //PHI stuff
        if(dphillj > M_PI-1)
            midphi=true;
        //float jetpt_phi=dphillj*dphillj*40/(M_PI*M_PI);
        float jetptphifunc=(dphillj-M_PI+1)*20+1;
        float jetpt_phi=std::max(10.0f,jetptphifunc);
        vector<NTJet*> dphilljjets;
        for(size_t i=0;i<idjets.size();i++){
            if(idjets.at(i)->pt()<jetpt_phi) continue;
            dphilljjets << idjets.at(i);
        }

        vector<NTJet*> dphiplushardjets=mergeVectors(hardjets,dphilljjets);
        std::sort(dphiplushardjets.begin(),dphiplushardjets.end(), comparePt<ztop::NTJet*>);



        vector<NTJet*> * selectedjets=&hardjets;





        evt.dphilljjets=&dphilljjets;
        evt.dphiplushardjets=&dphiplushardjets;
        evt.selectedjets=selectedjets;


        NTLorentzVector<float> H4;
        for(size_t i=0;i<idjets.size();i++)
            H4+=idjets.at(i)->p4();
        float ht=H4.Pt();
        evt.ht=&ht;

        NTLorentzVector<float> S4=leadingptlep->p4() + secleadingptlep->p4() -H4;
        evt.S4=&S4;

        NTLorentzVector<float> allobjects4=S4+ H4 + H4;
        evt.allobjects4=&allobjects4;


        vector<NTJet*>  selectedbjets;
        evt.selectedbjets=&selectedbjets;


        getBTagSF()->changeNTJetTags(selectedjets);
        for(size_t i=0;i<selectedjets->size();i++){
            if(selectedjets->at(i)->btag() < getBTagSF()->getWPDiscrValue())
                continue;
            selectedbjets.push_back(selectedjets->at(i));
        }

        float btagscontr=1;
        for(size_t i=0 ;i<selectedjets->size();i++){
            //if(i>0) break; //only use first jet for now
            if(selectedjets->at(i)->btag()>0 && selectedjets->at(i)->btag()<1){
                btagscontr*=(- selectedjets->at(i)->btag() +1);
                break;
            }
        }
        float topdiscr=1;
        evt.topdiscr=&topdiscr;
        topdiscr=1- ((cosleplepangle+1)/2 * dphillj/M_PI * btagscontr);

        float topdiscr2=1;
        evt.topdiscr2=&topdiscr2;
        topdiscr2=1- ((cosleplepangle+1)/2 * btagscontr);

        float topdiscr3=1;
        evt.topdiscr3=&topdiscr3;
        topdiscr3=1- (dphillj/M_PI * btagscontr);


        sel_step[3]+=puweight;
        plots.makeControlPlots(step);
        zplots.makeControlPlots(step);


        ////////////////////Z Selection//////////////////
        step++;
        bool isZrange=false;

        if(mll > 76 && mll < 106){
            isZrange=true;
        }

        bool analysisMllRange=!isZrange;
        if(b_emu_) analysisMllRange=true; //no z veto on emu

        ////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////



        if(analysisMllRange){

            plots.makeControlPlots(step);
            sel_step[4]+=puweight;

        }
        if(isZrange){
            zplots.makeControlPlots(step);

        }


        ///////////////////// at least one jet cut STEP 5 ////////////
        step++;

        if(selectedjets->size() < 1) continue;


        //ht+=adjustedmet.met();
        //double mllj=0;
        //double phijl=0;


        if(analysisMllRange){

            sel_step[5]+=puweight;
            plots.makeControlPlots(step);


        }
        if(isZrange){
            zplots.makeControlPlots(step);
        }



        /////////////////////// at least two jets STEP 6 /////////////
        step++;

        //if(midphi && dphiplushardjets.size()<2) continue;
        if(!onejet && selectedjets->size() < 2) continue;
        //if(!midphi && medjets.size()<2) continue;
        //if(ptllj<140) continue;
        /*if(dphillj > 2.65 && dphillj < 3.55){
			if(ptllj < 190)
				continue;
		} */

        if(analysisMllRange){

            plots.makeControlPlots(step);
            sel_step[6]+=puweight;
        }
        if(isZrange){

            zplots.makeControlPlots(step);
        }


        //////////////////// MET cut STEP 7//////////////////////////////////
        step++;
        if(!b_emu_ && adjustedmet.met() < 40) continue;


        if(analysisMllRange){

            plots.makeControlPlots(step);
            sel_step[7]+=puweight;
        }
        if(isZrange){
            zplots.makeControlPlots(step);

        }

        //make the b-tag SF
        for(size_t i=0;i<selectedbjets.size();i++){
            getBTagSF()->fillEff(selectedjets->at(i),puweight);
        }

        ///////////////////// btag cut STEP 8 //////////////////////////
        step++;
        if(!usetopdiscr && selectedbjets.size() < 1) continue;
        if(usetopdiscr && topdiscr3<0.9) continue;
        double bsf=1;
        bsf=getBTagSF()->getNTEventWeight(*selectedjets);
        puweight*= bsf;


        if(analysisMllRange){


            plots.makeControlPlots(step);
            sel_step[8]+=puweight;

            jansplots_step8.fillPlotsReco();

        }
        if(isZrange){
            zplots.makeControlPlots(step);
        }
        ///////////////////// 2btag cut STEP 9 //////////////////////////
/*
        step++;
        if(hardbjets.size() < 2) continue;



        if(analysisMllRange){
            plots.makeControlPlots(step);

        }
        if(isZrange){
            zplots.makeControlPlots(step);
        }
*/
    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////    MAIN LOOP ENDED    /////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////    POST PROCESSING    /////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    container1DUnfold::flushAllListed(); // call once again after last event processed

    if(testmode_ )
        std::cout << "testmode("<< anaid << "): finished main loop" << std::endl;


    // Fill all containers in the stackVector

    // std::cout << "Filling containers to the Stack\n" << std::endl;
    btagsf_.makeEffs(); //only does that if switched on, so safe

    if(testmode_ )
        std::cout << "testmode("<< anaid << "): filled all b-tag effs" << std::endl;

    if(!getBTagSF()->getMakeEff())
        std::cout << "B-Tag nan count: " << getBTagSF()->getNanCount() << std::endl;

    // delete t;
    f->Close(); //deletes t
    delete f;
    /*s
	for(size_t i=0;i<ntestconts;i++)
			delete testconts.at(i);
     */

    ///////////////////////////////
    ///////////////////////////////
    ///////////////////////////////
    ///////////////////////////////
    //     WRITE OUTPUT PART     //
    ///////////////////////////////
    ///////////////////////////////
    ///////////////////////////////
    ///////////////////////////////

    if(!singlefile_)
        p_askwrite.get(anaid)->pwrite(anaid);
    //std::cout << anaid << " (" << inputfile << ")" << " asking for write permissions to " <<getOutPath() << endl;
    int canwrite=0;
    if(!singlefile_)
        canwrite=p_allowwrite.get(anaid)->pread();
    if(canwrite>0 || singlefile_){ //wait for permission

        if(testmode_ )
            std::cout << "testmode("<< anaid << "): allowed to write to file " << getOutPath()+".root"<< std::endl;

        TFile * outfile;

        if(!fileExists((getOutPath()+".root").Data())) {
            outfile=new TFile(getOutPath()+".root","RECREATE");
        }
        else{
            outfile=new TFile(getOutPath()+".root","READ");
        }

        TTree * outtree;
        ztop::container1DStackVector * csv=&allplotsstackvector_;

        // std::cout << "trying to get tree" <<std::endl;

        if(outfile->Get("containerStackVectors")){
            outtree=(TTree*)outfile->Get("containerStackVectors");
            if(outtree->GetBranch("containerStackVectors")){ //exists already others are filled
                // csv->loadFromTree(outtree,csv->getName()); //makes copy
                csv=csv->getFromTree(outtree,csv->getName());
            }
        }

        csv->addList(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 1D List"<< std::endl;
        csv->addList2D(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 2D List"<< std::endl;
        csv->addList1DUnfold(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 1DUnfold List"<< std::endl;

        if(issignal)
            csv->addSignal(legendname);

        outfile->Close();
        delete outfile;

        outfile=new TFile((getOutPath()+".root"),"RECREATE");
        outfile->cd();

        outtree= new TTree("containerStackVectors","containerStackVectors");
        outtree->Branch("containerStackVectors",&csv);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added Branch"<< std::endl;
        outtree->Fill();
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): filled new outfile tree"<< std::endl;

        outtree->Write("",TObject::kOverwrite);//"",TObject::kOverwrite);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): written new outfile tree"<< std::endl;

        outfile->Close();
        delete outfile;

        if(testmode_ )
            std::cout << "testmode("<< anaid << "): written main output"<< std::endl;


        ///btagsf
        if(btagsf_.makesEff()){
            if(fileExists(btagsffile_.Data())){
                if(testmode_ )
                    std::cout << "testmode("<< anaid << "): reading btag file"<< std::endl;
                ztop::NTBTagSF  btsf;
                btsf.readFromTFile(btagsffile_);
                btagsf_ = (btagsf_ + btsf);
            }
            if(testmode_ )
                std::cout << "testmode("<< anaid << "): writing btag file"<< std::endl;
            btagsf_.writeToTFile(btagsffile_); //recreates the file
        }///makes eff

        std::cout << inputfile << ": " << std::endl;
        for(unsigned int i=0;i<9;i++){
            std::cout << "selection step "<< toTString(i)<< " "  << sel_step[i];
            if(i==3)
                cout << "  => sync step 1 \tmll>20";
            if(i==4)
                cout << "  => sync step 2 \tZVeto";
            if(i==6)
                cout << "  => sync step 3 \t2 Jets";
            if(i==7)
                cout << "  => sync step 4 \tMET";
            if(i==8)
                cout << "  => sync step 5 \t1btag";
            std::cout  << std::endl;
        }


        if(singlefile_) return;

        //all operations done
        p_finished.get(anaid)->pwrite(1); //turns of write blocking, too
    }
    else{
        std::cout << "testmode("<< anaid << "): failed to write to file " << getOutPath()+".root"<< std::endl;
        p_finished.get(anaid)->pwrite(-2); //write failed
    }




}



bool MainAnalyzer::checkTrigger(std::vector<bool> * p_TriggerBools,ztop::NTEvent * pEvent, bool isMC,size_t anaid){


    //do trigger stuff - onlye 8TeV for now
    if(!is7TeV_){
        if(p_TriggerBools->size() < 3)
            return false;

        if(b_mumu_){
            if(!(p_TriggerBools->at(1) || p_TriggerBools->at(2)))
                return false;
            else
                return true;
        }
        else if(b_ee_){
            if(!p_TriggerBools->at(0))
                return false;
            else
                return true;
        }
        else if(b_emu_){
            if(p_TriggerBools->size()<10){
                //p_finished.get(anaid)->pwrite(-3);
                return false;
            }
            if(!(p_TriggerBools->at(10) || p_TriggerBools->at(11)))
                return false;
            else
                return true;
        }
    }
    else{ //is7TeV_
        if(p_TriggerBools->size() < 3)
            return false;

        if(b_mumu_){
            if(isMC && !p_TriggerBools->at(5))
                return false;
            if(!isMC && pEvent->runNo() < 163869 && !p_TriggerBools->at(5))
                return false;
            if(!isMC && pEvent->runNo() >= 163869 && !p_TriggerBools->at(6))
                return false;
        }
        else if(b_ee_){
            if(!(p_TriggerBools->at(3) || p_TriggerBools->at(4) || p_TriggerBools->at(1)))
                return false;
        }
        else if(b_emu_){
            std::cout << "emu channel at 7TeV not supported yet (triggers missing)" << std::endl;
            // p_finished.get(anaid)->pwrite(-4);
            return false;

            if(p_TriggerBools->size()<10){
                // p_finished.get(anaid)->pwrite(-3);
                return false;
            }
            if(!(p_TriggerBools->at(10) || p_TriggerBools->at(11)))
                return false;
        }
    }
    return true;
}



#endif /* EVENTLOOP_H_ */
