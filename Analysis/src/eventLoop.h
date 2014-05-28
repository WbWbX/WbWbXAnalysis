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
#include "../interface/analysisPlotsAnya.h"

#include "../interface/discriminatorFactory.h"

#include "../interface/tBranchHandler.h"

#include <sys/types.h>

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
void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color,size_t legord, size_t anaid){

    bool doLargeAcceptance=false;
    /*means right now:
     *
     * leps: 20/10
     * jets: 30/20
     *
     */

    using namespace std;
    using namespace ztop;

    bool issignal=issignal_.at(anaid);

    bool isMC=true;
    if(legendname==dataname_) isMC=false;


    //some mode options
    /* implement here not to overload MainAnalyzer class with private members
     *  they are even allowed to overwrite existing configuration
     *  with the main purpose of trying new things
     *
     *  each option has to start with a CAPITAL letter and must not contain other capital letters
     *  This allows for parsing of more than one option at once
     *
     *  These options should get a cleanup once in a while
     *  The standard configuration should be visible on top
     */

    TString mettype="NTT0T1TxyMet";
    TString electrontype="NTPFElectrons";
    bool mode_samesign=false;
    bool mode_invertiso=false;
    bool usemvamet=false;
    bool onejet=false;
    bool zerojet=false;
    bool usetopdiscr=false;

    float normmultiplier=1; //use in case modes need to change norm

    if(mode_.Contains("Samesign")){
        mode_samesign=true;
        std::cout << "entering same sign mode" <<std::endl;
    }

    if(mode_.Contains("Invertiso")){
        mode_invertiso=true;
        std::cout << "entering invert iso mode" <<std::endl;
    }
    if(mode_.Contains("Mvamet")){
        mettype="NTMvaMet";
        usemvamet=true;
        std::cout << "entering mvamet mode" <<std::endl;
    }
    if(mode_.Contains("Pfmet")){
        mettype="NTMet";
        std::cout << "entering pfmet mode" <<std::endl;
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
    if(mode_.Contains("Onejet")){
        onejet=true;
        std::cout << "entering Onejet mode" <<std::endl;
    }
    if(mode_.Contains("Zerojet")){
        zerojet=true;
        std::cout << "entering Onejet mode" <<std::endl;
    }

    if(mode_.Contains("Topdiscr")){
        usetopdiscr=true;
        std::cout << "entering Topdiscr mode" <<std::endl;
    }
    //hmm this should actually go to readFiles function.. but then interferes with the mode idea.. leave it here for the moment
    if(mode_.Contains("Mgdecays")){
        std::cout << "entering Mgdecays mode: norm will be adapted" <<std::endl;
        if(inputfile.Contains("ttbar.root")){
            inputfile.ReplaceAll("ttbar.root", "ttbar_mgdecays.root");
            // infiles_.at(anaid).ReplaceAll("ttbar.root", "ttbar_mgdecays.root");
            normmultiplier=0.1049; //fully leptonic branching fraction
        }
        else if(inputfile.Contains("ttbarviatau.root")){
            inputfile.ReplaceAll("ttbarviatau.root", "ttbarviatau_mgdecays.root");
            // infiles_.at(anaid).ReplaceAll("ttbar.root", "ttbar_mgdecays.root");
            normmultiplier=0.1049;
        }
    }
    else{
        //adapt wrong MG BR for madspin and syst samples
        if(inputfile.Contains("ttbar.root") || inputfile.Contains("ttbarviatau.root")
                || inputfile.Contains("ttbar_") ||
                inputfile.Contains("ttbarviatau_") ){
            normmultiplier=0.1049/0.1111202;
        }
    }

    if(mode_.Contains("Notoppt")){
        getTopPtReweighter()->setFunction(reweightfunctions::flat);
    }
    bool fakedata=false,isfakedatarun=false;
    if(mode_.Contains("Fakedata")){
        if(legendname ==  dataname_)
            fakedata=true;
        isfakedatarun=true;
        isMC=true;
        std::cout << "THIS IS A PSEUDO-DATA RUN:" <<std::endl;// all samples MC samples will be used without weights!" << std::endl;
    }


    /*
     * end of mode switches
     */

    //fake data configuration
    if(fakedata){
        //this whole section is bad style because the configuration should be done from the outside
        //e.g. in analyse.cc
        //but for pseudo data there needs to be an additional case distinction in addition to MC/data

        //turn off systematic variations here

        getTriggerSF()->setSystematics("nom");


        getElecEnergySF()->setSystematics("nom");
        getElecSF()->setSystematics("nom");
        getMuonEnergySF()->setSystematics("nom");
        getMuonSF()->setSystematics("nom");


        // getPUReweighter()-> //setSystematics("nom");
        getBTagSF()->setSystematic(NTBTagSF::nominal);
        getJECUncertainties()->setSystematics("no");
        getJERAdjuster()->setSystematics("def");

        getTopPtReweighter()->setSystematics(reweightfunctions::nominal); //setSystematics("nom");

        getPdfReweighter()->setPdfIndex(0);

        //for other parameters reread config
        fileReader fr;
        fr.setComment("$");
        fr.setDelimiter(",");
        fr.setStartMarker("[parameters-begin]");
        fr.setEndMarker("[parameters-end]");
        fr.readFile(pathtoconffile_.Data());

        getPUReweighter()->setDataTruePUInput(((std::string)getenv("CMSSW_BASE")+fr.getValue<string>("PUFile") +".root").data());


        //re-adjust systematic filenames
        for(size_t i=0;i<ftorepl_.size();i++){
            if(inputfile.EndsWith(ftorepl_.at(i))){
                inputfile.ReplaceAll(fwithfix_.at(i),ftorepl_.at(i));
            }
        }

        //this is bad style.. anyway
    }


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

    NTFullEvent evt;

    ////////////////////  configure discriminator factories here  ////////////////////
    TString factname="topLH_"+channel_+"_"+energy_+"_"+topmass_;
    discriminatorFactory::c_makelist=true;
    discriminatorFactory toplikelihood(factname.Data());

    discriminatorFactory::c_makelist=false;

    if(usediscr_){
        toplikelihood.readFromTFile(discrInput_,toplikelihood.getIdentifier());

    }
    else{
        toplikelihood.setNBins(40);
        toplikelihood.setStep(7); //includes met for ee/mumu
    }

    toplikelihood.setUseLikelihoods(usediscr_);

    toplikelihood.setSaveCorrelationPlots(false);

    toplikelihood.addVariable(&evt.lhi_dphillj,"#Delta#phi(ll,j)",0,M_PI);
    toplikelihood.addVariable(&evt.lhi_cosleplepangle,"#cos#theta_{ll}",-1.,1.);
    // if(!usediscr_)
    //      toplikelihood.addVariable(&evt.lhi_leadjetbtag,"D_{b}^{jet1}",-1,1);
    toplikelihood.addVariable(&evt.lhi_sumdphimetl,"#Delta#phi(met,l_{1})+#Delta#phi(met,l_{2})",0,2*M_PI);
    //  toplikelihood.addVariable(&evt.lhi_seljetmulti,"N_{jets}",-0.5,4.5);
    //  toplikelihood.addVariable(&evt.lhi_selbjetmulti,"N_{b-jets}",-0.5,4.5);
    toplikelihood.addVariable(&evt.lhi_leadleppt,"p_{T}^{1st lep}",0,150);
    toplikelihood.addVariable(&evt.lhi_secleadleppt,"p_{T}^{2nd lep}",0,150);
    toplikelihood.addVariable(&evt.lhi_leadlepeta,"#eta^{1st lep}",-2.4,2.4);
    toplikelihood.addVariable(&evt.lhi_secleadlepeta,"#eta^{2nd lep}",-2.4,2.4);
    toplikelihood.addVariable(&evt.lhi_leadlepphi,"#phi^{1st lep}",-M_PI,M_PI);
    toplikelihood.addVariable(&evt.lhi_secleadlepphi,"#phi^{2nd lep}",-M_PI,M_PI);
    //  toplikelihood.addVariable(&evt.lhi_leadjetpt,"p_{T}^{1st jet}",0,150);
    toplikelihood.addVariable(&evt.lhi_mll,"m_{ll}",0.,300);
    // toplikelihood.addVariable(&evt.lhi_ptllj,"p_{T}(llj) [GeV]",50,400);



    toplikelihood.addVariable(&evt.lhi_cosllvsetafirstlep,"cos#theta vs #eta_{l1}",-4,2);
    toplikelihood.addVariable(&evt.lhi_etafirstvsetaseclep,"#eta_{l1} vs #eta_{l2}",-3,3);
    toplikelihood.addVariable(&evt.lhi_deltaphileps,"#Delta#phi(l_{1},l_{2})",-2*M_PI,2*M_PI);
    toplikelihood.addVariable(&evt.lhi_coslepanglevsmll,"cos#theta vs m_{ll}",-1,1);
    toplikelihood.addVariable(&evt.lhi_mllvssumdphimetl,"m_{ll} vs #Delta#phi(met,l_{1})+#Delta#phi(met,l_{2})",-10,0.5);


    if(usediscr_){
        toplikelihood.setSystematics(getSyst());
    }
    else{
        toplikelihood.setSystematics("nominal");
    }

    /////////////////////////////////////////////////////////////////////////////////

    //just a test

    if(testmode_)
        std::cout << "testmode("<< anaid << "): preparing container1DUnfolds" << std::endl;

    //////////////analysis plots/////////////


    analysisPlotsJan jansplots_step8(8);
    analysisPlotsAnya anyasplots_step8(8);
    if((TString)getenv("USER") == (TString)"kiesej"){
        jansplots_step8.enable();
    }
    if((TString)getenv("USER") == (TString)"dolinska"){
        jansplots_step8.enable();
    }

    jansplots_step8.bookPlots();
    anyasplots_step8.bookPlots();

    //global settings for analysis plots
    container1DUnfold::setAllListedMC(isMC && !fakedata);
    container1DUnfold::setAllListedLumi((float)lumi_);
    if(testmode_)
        container1DUnfold::setAllListedLumi((float)lumi_*0.08);

    //setup everything for control plots

    ttbarControlPlots plots;//for the Z part just make another class (or add a boolian)..
    ZControlPlots zplots;
    plots.linkEvent(evt);
    zplots.linkEvent(evt);
    plots.initSteps(8);
    zplots.initSteps(8);
    jansplots_step8.setEvent(evt);
    anyasplots_step8.setEvent(evt);


    //get normalization
    double norm=createNormalizationInfo(f,isMC,anaid);

    norm*= normmultiplier;
    /////////////////////////// configure scalefactors ////


    //init b-tag scale factor utility
    if(testmode_)
        std::cout << "testmode("<< anaid << "): preparing btag SF" << std::endl;

    //make sure the nominal scale factors are used for varations of the SF
    TString btagSysAdd=topmass_+"_"+getSyst();
    if(getBTagSF()->getSystematic() != bTagBase::nominal)
        btagSysAdd=topmass_+"_nominal";

    if(fakedata) //always use nominal
        btagSysAdd=topmass_+"_nominal"+"_fakedata";

    if(getBTagSF()->setSampleName((channel_+"_"+btagSysAdd+"_"+toString(inputfile)).Data()) < 0){
        reportError(-3,anaid);
        return;
    }
    getBTagSF()->setIsMC(isMC);


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

    if(!isMC)
        getPdfReweighter()->switchOff(true);




    /*
     * Open Main tree,
     * Set branches
     * the handler is only a small wrapper
     */
    TTree * t = (TTree*) f->Get("PFTree/PFTree");

    tBranchHandler<std::vector<bool> >     b_TriggerBools(t,"TriggerBools");
    tBranchHandler<vector<NTElectron> >    b_Electrons(t,electrontype);
    tBranchHandler<vector<NTMuon> >        b_Muons(t,"NTMuons");
    tBranchHandler<vector<NTJet> >         b_Jets(t,"NTJets");
    tBranchHandler<NTMet>                  b_Met(t,mettype);
    tBranchHandler<NTEvent>                b_Event(t,eventbranch_);
    tBranchHandler<vector<NTGenParticle> > b_GenTops(t,"NTGenTops");
    tBranchHandler<vector<NTGenParticle> > b_GenZs(t,"NTGenZs");
    tBranchHandler<vector<NTGenParticle> > b_GenBs(t,"NTGenBs");
    tBranchHandler<vector<NTGenParticle> > b_GenBHadrons(t,"NTGenBHadrons");
    tBranchHandler<vector<NTGenParticle> > b_GenLeptons3(t,"NTGenLeptons3");
    tBranchHandler<vector<NTGenParticle> > b_GenLeptons1(t,"NTGenLeptons1");
    tBranchHandler<vector<NTGenJet> >      b_GenJets(t,"NTGenJets");
    tBranchHandler<vector<NTGenParticle> > b_GenNeutrinos(t,"NTGenNeutrinos");

    //some helpers
    double sel_step[]={0,0,0,0,0,0,0,0,0};
    float count_samesign=0;

    float genvisPScounter=0;

    if(!testmode_){
        // this enables some caching while reading the tree. Speeds up batch mode
        // significantly!
        setCacheProperties(t,datasetdirectory_+inputfile);
    }

    Long64_t nEntries=t->GetEntries();
    if(norm==0) nEntries=0; //skip for norm0
    if(testmode_) nEntries*=0.08;
    Long64_t firstentry=0;
    //for fake data all signal samples are assumed to be used as fake data
    //split at 10%
    if(isfakedatarun){
        float splitat=0.9;
        if(issignal || fakedata){
            if(issignal){
                nEntries*=splitat;
                norm*= 1/splitat;
            }
            else if(fakedata){
                firstentry=nEntries*splitat;
                norm*=1/(1-splitat);
            }

            //readapt norm to right mtop value



        }
    }

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


    for(Long64_t entry=firstentry;entry<nEntries;entry++){

        t->LoadTree(entry);

        ////////////////////////////////////////////////////
        ////////////////////  INIT EVENT ///////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////


        size_t step=0;
        evt.reset();
        evt.event=b_Event.content();
        container1DUnfold::flushAllListed(); //important to call each event

        //reports current status to parent
        reportStatus(entry,nEntries,anaid);

        b_Event.getEntry(entry);
        float puweight=1;
        if (isMC) puweight = getPUReweighter()->getPUweight(b_Event.content()->truePU());
        if(testmode_ && entry==0)
            std::cout << "testmode("<< anaid << "): got first event entry" << std::endl;

        evt.puweight=&puweight;

        getPdfReweighter()->setNTEvent(b_Event.content());
        getPdfReweighter()->reWeight(puweight);


        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////// Generator Information////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////

        ////define all collections
        // do not move somewhere else!

        vector<NTGenParticle*> gentops,genbs;
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

            b_GenLeptons3.getEntry(entry);
            if(b_GenLeptons3.content()->size()>1){ //gen info there

                b_GenTops.getEntry(entry);
                b_GenBHadrons.getEntry(entry);
                b_GenJets.getEntry(entry);
                b_GenLeptons1.getEntry(entry);
                //recreate mother daughter relations?!
                b_GenBHadrons.getEntry(entry);
                b_GenBs.getEntry(entry);
                b_GenJets.getEntry(entry);
                b_GenLeptons1.getEntry(entry);
                ///////////////TOPPT REWEIGHTING////////


                if(b_GenTops.content()->size()>1){ //ttbar sample

                    getTopPtReweighter()->reWeight(b_GenTops.content()->at(0).pt(),b_GenTops.content()->at(1).pt() ,puweight);

                    gentops.push_back(&b_GenTops.content()->at(0));
                    gentops.push_back(&b_GenTops.content()->at(0));
                    evt.gentops=&gentops;

                    for(size_t i=0;i<b_GenBs.content()->size();i++)
                        genbs.push_back(&b_GenBs.content()->at(i));
                    evt.genbs=&genbs;
                }

                if(testmode_ && entry==0){
                    std::cout << "testmode("<< anaid << "): entered signal genInfo part" << std::endl;
                }


                //only format change
                genleptons1=produceCollection<NTGenParticle>(b_GenLeptons1.content());
                genleptons3=produceCollection<NTGenParticle>(b_GenLeptons3.content());
                genjets=produceCollection<NTGenJet>(b_GenJets.content());


                //define visible phase space
                float ps_ptlepmin=20;
                float ps_etalmax=2.4;

                float ps_ptjetmin=30;
                float ps_etajetmax=2.4;



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
                for(size_t i=0;i<b_GenBHadrons.content()->size();i++){
                    NTGenParticle * bhad=&b_GenBHadrons.content()->at(i);
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
                    if(fabs(bjet->eta()) >ps_etajetmax) continue;
                    genvisbjetsfromtop << bjet;
                }
                //gen acceptance counter
                if(genvisjets.size()>1 && genvisleptons3.size()>1 && genvisbjetsfromtop.size()>0){
                    genvisPScounter+=puweight;
                }

            }
            if(!fakedata){
                /*
                 * fill gen info here
                 */
                jansplots_step8.fillPlotsGen();
                anyasplots_step8.fillPlotsGen();
            }
        } /// isMC ends

        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        ///////////////////       Reco Part      ////////////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////



        /*
         *  Trigger
         */
        b_TriggerBools.getEntry(entry);
        if(testmode_ && entry==0)
            std::cout << "testmode("<< anaid << "): got trigger boolians" << std::endl;
        if(!checkTrigger(b_TriggerBools.content(),b_Event.content(), isMC,anaid)) continue;


        /*
         * Muons
         */
        b_Muons.getEntry(entry);


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
        for(size_t i=0;i<b_Muons.content()->size();i++){
            NTMuon* muon = & b_Muons.content()->at(i);
            if(isMC)
                muon->setP4(muon->p4() * getMuonEnergySF()->getScalefactor(muon->eta()));
            allleps << muon;
            if(!gotfirst && muon->pt() < 20)       continue;
            if(doLargeAcceptance) gotfirst=true;
            if(muon->pt() < 10) continue;
            if(fabs(muon->eta())>2.4) continue;
            kinmuons << &(b_Muons.content()->at(i));
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
                idmuons <<  &(b_Muons.content()->at(i));
                continue; //no double counting
            }
            //usetight=false;

            ///end tight
            //  loosemuons <<  &(b_Muons.content()->at(i));
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

        b_Electrons.getEntry(entry);

        vector<NTElectron *> kinelectrons,idelectrons,isoelectrons;
        evt.kinelectrons=&kinelectrons;
        evt.idelectrons=&idelectrons;
        evt.isoelectrons=&isoelectrons;
        gotfirst=false;
        for(size_t i=0;i<b_Electrons.content()->size();i++){
            NTElectron * elec=&(b_Electrons.content()->at(i));
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
            if(abseta > 2.4) continue;
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
        b_Jets.getEntry(entry);

        vector<NTJet *> treejets,idjets,medjets,hardjets;
        evt.idjets=&idjets;
        evt.medjets=&medjets;
        evt.hardjets=&hardjets;
        for(size_t i=0;i<b_Jets.content()->size();i++){
            treejets << &(b_Jets.content()->at(i));
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
            if(fabs(treejets.at(i)->eta())>2.4) continue;
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



        b_Met.getEntry(entry);

        NTMet adjustedmet = *b_Met.content();
        evt.simplemet=b_Met.content();
        evt.adjustedmet=&adjustedmet;
        double nmpx=b_Met.content()->p4().Px() + dpx;
        double nmpy=b_Met.content()->p4().Py() + dpy;
        if(!usemvamet)
            adjustedmet.setP4(D_LorentzVector(nmpx,nmpy,0,sqrt(nmpx*nmpx+nmpy*nmpy))); //COMMENTED FOR MVA MET

        ///////////////combined variables////////////

        float dphillj=M_PI,dphilljj=M_PI,detallj=0,detalljj=0;
        float pi=M_PI;
        bool midphi=false;

        if(hardjets.size() >0){
            dphillj=fabs( (leadingptlep->p4()+secleadingptlep->p4()).Phi() - hardjets.at(0)->p4().Phi() );
            if(dphillj>pi)
                dphillj=2*pi-dphillj;
            //dphillj=pi-fabs(dphillj-pi);
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


        ///////////////calculate likelihood inputs///////////




        float lhi_dphillj=dphillj;
        evt.lhi_dphillj=&lhi_dphillj;

        float lhi_cosleplepangle=cosleplepangle;
        evt.lhi_cosleplepangle=&lhi_cosleplepangle;

        float lhi_leadjetbtag=1;
        if(selectedjets->size()>0)
            lhi_leadjetbtag=(selectedjets->at(0)->btag());
        evt.lhi_leadjetbtag =&lhi_leadjetbtag;

        float lhi_sumdphimetl=0;
        lhi_sumdphimetl=absNormDPhi(leadingptlep->p4(),adjustedmet.p4()) + absNormDPhi(secleadingptlep->p4(),adjustedmet.p4());
        evt.lhi_sumdphimetl=&lhi_sumdphimetl;

        float lhi_seljetmulti=0;
        lhi_seljetmulti=selectedjets->size();
        evt.lhi_seljetmulti=&lhi_seljetmulti;

        float lhi_selbjetmulti=selectedbjets.size();
        evt.lhi_selbjetmulti=&lhi_selbjetmulti;

        float lhi_leadleppt=leadingptlep->pt();
        evt.lhi_leadleppt=&lhi_leadleppt;

        float lhi_secleadleppt=secleadingptlep->pt();
        evt.lhi_secleadleppt=&lhi_secleadleppt;

        float lhi_leadlepeta=leadingptlep->eta();
        evt.lhi_leadlepeta=&lhi_leadlepeta;

        float lhi_secleadlepeta=secleadingptlep->eta();
        evt.lhi_secleadlepeta=&lhi_secleadlepeta;

        float lhi_leadlepphi=leadingptlep->phi();
        evt.lhi_leadlepphi=&lhi_leadlepphi;

        float lhi_secleadlepphi=secleadingptlep->phi();
        evt.lhi_secleadlepphi=&lhi_secleadlepphi;

        float lhi_leadjetpt=0;
        if(selectedjets->size()>0)
            lhi_leadjetpt=selectedjets->at(0)->pt();
        evt.lhi_leadjetpt=&lhi_leadjetpt;

        evt.lhi_mll=&mll;

        float lhi_drlbl=0;
        if(selectedbjets.size()>0){
            NTLorentzVector<float> lbp4=selectedbjets.at(0)->p4()+leadingptlep->p4();
            NTLorentzVector<float> lbp42=selectedbjets.at(0)->p4()+secleadingptlep->p4();
            if(lbp4.m() > 165 && lbp42.m() < 165){
                lhi_drlbl=dR_3d(lbp42,leadingptlep->p4());
            }
            else{
                lhi_drlbl=dR_3d(lbp4,secleadingptlep->p4());
            }
            evt.lhi_drlbl=&lhi_drlbl;
        }

        evt.lhi_ptllj=&ptllj;


        //correlated variables


        float lhi_cosllvsetafirstlep =
                lhi_cosleplepangle - (0.2*(fabs(lhi_leadlepeta)-2.4)*(fabs(lhi_leadlepeta)-2.4) +1);
        if(lhi_cosleplepangle < -0.4)
            lhi_cosllvsetafirstlep =
                    lhi_cosleplepangle - (0.2*(fabs(lhi_leadlepeta)+2.4)*(fabs(lhi_leadlepeta)+2.4) +1);

        evt.lhi_cosllvsetafirstlep=&lhi_cosllvsetafirstlep;
        float lhi_etafirstvsetaseclep =
                lhi_leadlepeta + lhi_secleadlepeta;
        evt.lhi_etafirstvsetaseclep=&lhi_etafirstvsetaseclep;
        float lhi_deltaphileps =
                lhi_leadlepphi - lhi_secleadlepphi;
        evt.lhi_deltaphileps=&lhi_deltaphileps;
        float  lhi_coslepanglevsmll =
                -lhi_cosleplepangle - 1/65 * (mll-90);
        evt.lhi_coslepanglevsmll=&lhi_coslepanglevsmll;
        float lhi_mllvssumdphimetl =
                1/14 * (mll-25) - lhi_sumdphimetl;
        evt.lhi_mllvssumdphimetl=&lhi_mllvssumdphimetl;



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

        if(!zerojet && selectedjets->size() < 1) continue;

        if(getBTagSF()->getMode() == NTBTagSF::shapereweighting_mode){

            if(onejet)
                puweight*=getBTagSF()->getNTEventWeight(selectedjets->at(0));
            else
                puweight*=getBTagSF()->getNTEventWeight(*selectedjets);
        }

        //ht+=adjustedmet.met();
        //double mllj=0;
        //double phijl=0;


        float lh_toplh=0;
        evt.lh_toplh=&lh_toplh;

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
        if(!zerojet && !onejet && selectedjets->size() < 2) continue;
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

            lh_toplh=toplikelihood.getCombinedLikelihood();
            toplikelihood.fill(puweight);

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
        // if(usetopdiscr && topdiscr3<0.9) continue;
        if(usetopdiscr && lh_toplh<0.3) continue;
        if(getBTagSF()->getMode() != NTBTagSF::shapereweighting_mode){
            puweight*=getBTagSF()->getNTEventWeight(*selectedjets);
        }



        if(analysisMllRange){

            // std::cout << selectedjets->at(0)->pt() << std::endl;

            plots.makeControlPlots(step);
            sel_step[8]+=puweight;

            jansplots_step8.fillPlotsReco();
            anyasplots_step8.fillPlotsReco();

        }
        if(isZrange){
            zplots.makeControlPlots(step);
        }

    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////    MAIN LOOP ENDED    /////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////    POST PROCESSING    /////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    //renorm for topptreweighting
    norm *= getTopPtReweighter()->getRenormalization();

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

        csv->addList1DUnfold(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 1DUnfold List"<< std::endl;
        csv->addList2D(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 2D List"<< std::endl;
        csv->addList(legendname,color,norm,legord);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): added 1D List"<< std::endl;

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


        //write discriminatorFactory configuration to newly created outfile (last job will persist)
        discriminatorFactory::debug=true;
        discriminatorFactory::writeAllToTFile(outfile);
        if(testmode_ )
            std::cout << "testmode("<< anaid << "): written "<< discriminatorFactory::c_list.size()<< " discriminatorFactorys"<< std::endl;

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

        std::cout << "\nEvents in defined visible phase space (normalized): "
                << genvisPScounter*norm << "\n" << std::endl;
        std::cout << "\nEvents total (normalized): "
                << nEntries*norm << "\n" << std::endl;


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
