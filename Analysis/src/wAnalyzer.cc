/*
 * wAnalyzer.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#include "../interface/wAnalyzer.h"
#include "../interface/tTreeHandler.h"
#include "../interface/tBranchHandler.h"
#include "../interface/simpleReweighter.h"

#include "../interface/wNTJetInterface.h"
#include "../interface/wNTLeptonsInterface.h"
#include "../interface/NTFullEvent.h"
#include "../interface/wReweighterInterface.h"
#include "../interface/wNTGenJetInterface.h"
#include "../interface/wNTGenParticleInterface.h"

#include "../interface/wControlPlots.h"
#include "../interface/wControlplots_gen.h"
#include "../interface/analysisPlotsW.h"
#include "TtZAnalysis/DataFormats/interface/helpers.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/GenStatusFlags.h"

#include "../interface/wReweightingPlots.h"

#include "../interface/HTransformToCS.h"
#include "../interface/wGenSelector.h"
#include "../interface/wNTGoodEventInterface.h"

#include "../interface/wChargeFlipGenerator.h"

//small helper




namespace ztop{

void wAnalyzer::pairLeptons( std::vector<NTGenParticle*> * v, NTGenParticle*& lep, NTGenParticle*& neutr, bool isdy)const{
    lep=0;neutr=0;
    if(v->size()<1)return;
    int addtopdg=1;
    if(isdy)
        addtopdg=0;
    size_t startat=0;
    for(size_t i=0;i<v->size();i++){
        NTGenParticle* p=v->at(i);
        if(std::abs(p->pdgId())==15){
            startat=i;
            break;
        }
    }
    size_t i=startat;
    do{
        NTGenParticle* p=v->at(i);
        if(p->pdgId() == 13
                || p->pdgId() == -13
                || p->pdgId() == 11
                || p->pdgId() == -11
                || p->pdgId() == 15   //taus are vetoed later
                || p->pdgId() == -15){
            //){ //try muons, only
            //search for neutrino
            for(size_t j=0;j<v->size();j++){
                if(i==j)continue;
                NTGenParticle* p2=v->at(j);
                if(p2->pdgId() * p->pdgId() <0 &&
                        (int)std::abs(p2->pdgId()) == (int)(std::abs(p->pdgId()))+addtopdg){

                    lep=p;
                    neutr=p2;
                    if(isdy){
                        if(p->q()>0){//make the "lepton" always the negative charge one
                            lep=p2;
                            neutr=p;
                        }
                    }
                    break;
                }
            }

        }
        if(lep&&neutr)
            break;
        if(i+1<v->size())
            i++;
        else
            i=0;

    }while(i!=startat);
}

fileForker::fileforker_status  wAnalyzer::start(){
    allplotsstackvector_.setName(getOutFileName());
    allplotsstackvector_.setSyst(getSyst());
    setOutputFileName((getOutPath()+".ztop").Data());//add extension
    return runParallels(5);
}


fileForker::fileforker_status wAnalyzer::writeOutput(){
    return writeHistos();
}


void wAnalyzer::analyze(size_t id){



    tTreeHandler t;
    if(inputfile_.Contains("treeProducerSusySoftlepton") || inputfile_.Contains("tree_SingleMu"))
        t.load( datasetdirectory_+inputfile_ ,"treeProducerSusySoftlepton");
    else
        t.load( datasetdirectory_+inputfile_ ,"treeProducerA7W");



    createNormalizationInfo(&t);

    const bool isNLO=  signal_ && inputfile_.Contains("new/");
    //	const bool isInclusiveW=inputfile_.Contains("Wjets.root");

    wGenSelector selectGen;
    if(isNLO){
        if(inputfile_.Contains("WLN1J")){
            selectGen.addVeto(15);
            selectGen.addVeto(-15);
        }
    }
    const bool isDYsig=signal_ && inputfile_.Contains("DY");

    NTFullEvent evt;
    wControlPlots c_plots;
    c_plots.linkEvent(evt);
    c_plots.initSteps(6);

    wControlplots_gen c_plots_gen;
    c_plots_gen.linkEvent(evt);
    c_plots_gen.enable(false);

    analysisPlotsW anaplots(5);
    anaplots.setEvent(evt);
    anaplots.enable();
    anaplots.bookPlots();
    histo1DUnfold::setAllListedMC(isMC_);
    histo1DUnfold::setAllListedLumi((float)lumi_);


    wReweightingPlots rewplots;
    rewplots.setWKinematicsOnly(true);//TEST
    rewplots.enable(true);
    rewplots.setSignal(signal_);
    rewplots.setEvent(&evt);
    rewplots.bookPlots();


    wNTJetInterface b_Jets
    (&t, "nJet", "Jet_pt", "Jet_eta", "Jet_phi","Jet_mass","Jet_btagCSV","Jet_puId");

    wNTLeptonsInterface b_goodLeptons=wNTLeptonsInterface
            (&t,"nLepGood","LepGood_pdgId","LepGood_pt","LepGood_eta","LepGood_phi","LepGood_mass",
                    "LepGood_charge","LepGood_tightId","LepGood_relIso04","LepGood_dxy","LepGood_dz");

    wNTLeptonsInterface b_otherLeptons=wNTLeptonsInterface
            (&t,"nLepOther","LepOther_pdgId","LepOther_pt","LepOther_eta","LepOther_phi","LepOther_mass",
                    "LepOther_charge","LepOther_tightId","LepOther_relIso04","LepOther_dxy","LepOther_dz");


    bool enablegenjets=false;//signal_;

    wNTGenJetInterface b_genjets(&t,enablegenjets, "nGenJet", "GenJet_pt", "GenJet_eta", "GenJet_phi", "GenJet_mass");
    wNTGenParticleInterface b_genparticles(&t,signal_, "nGenPart", "GenPart_pt", "GenPart_eta", "GenPart_phi",
            "GenPart_mass","GenPart_pdgId","GenPart_charge","GenPart_status", "GenPart_motherId",
            "GenPart_grandmotherId","GenPart_isPromptHard");


    wNTGoodEventInterface b_goodevent(&t,isMC_);

    wReweighterInterface mcweights=wReweighterInterface(&t,signal_&& isNLO,"ntheoryWeightsValue","theoryWeightsValue_weight");
    if(signal_)
        for(size_t i=0;i<weightindicies_.size();i++)
            mcweights.addWeightIndex(weightindicies_.at(i));
    simpleReweighter lheReweighter;
    if(mcweights.isActive())
        lheReweighter.switchOff(true); //LHE weights are included in mc weights

    //for testing DEBUG if(!isMC_){
    mcweights.disable();
    lheReweighter.switchOff(true);
    tBranchHandler<float>::allow_missing=true;
    //}
    tBranchHandler<float> b_puweight(&t, "puWeight");
    tBranchHandler<float> b_lep1weight(&t, "LepEff_1lep");
    tBranchHandler<float> b_lep2weight(&t, "LepEff_2lep");
    tBranchHandler<float>::allow_missing=false;

    tBranchHandler<int>::allow_missing=!(signal_ && isNLO);
    tBranchHandler<int> b_lheweight(&t, "LHE_originalWeight");
    tBranchHandler<int>::allow_missing=false;

    tBranchHandler<int> b_trigger(&t, "HLT_SingleMu");
    tBranchHandler<int> b_vertices(&t, "nVert");




    Long64_t entries=t.entries();


    //////individual histos
    histo1D::c_makelist=true;
    histo1D h(histo1D::createBinning(6,-.5,6.5),"Steps","","");
    histo1D::c_makelist=false;

    histo2D::c_makelist=true;
    histo2D ptttransA7corr (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
            "ptttransA7corr gen", "2 p_{T}^{#perp}/ M_{W} (gen)", "sin(#phi*)sin(#theta*)","Events");
    histo2D ptttransA7corrrec (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
            "ptttransA7corr reco", "2 p_{T}^{#perp}/ M_{W} (reco)", "2 p_{T}^{#perp}/ M_{W} (gen)","Events");
    histo2D ptttransA7corrgenrec (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
            "ptttransA7corr genreco", "2 p_{T}^{#perp}/ M_{W} (reco)", "sin(#phi*)sin(#theta*)","Events");
    histo2D genstatHistDEtaJWpTW(histo1D::createBinning(100,-5.5,5.5), histo1D::createBinning(100,0,200),
            "DEtaW pTW", "#Delta#eta(W,j)", "p_{T}^{W}","Events");

    histo2D wptjetpt  (histo1D::createBinning(100, 0 , 200), histo1D::createBinning(100, 0 , 200),
            "gen Wpt gen Jetpt", "p_{T}(W) [GeV]", "p_{T}(jet) [GeV]","Events", false);

    histo2D wptjetmulti  (histo1D::createBinning(100, 0 , 200), histo1D::createBinning(5 , -.5 , 4.5 ),
            "gen Wpt gen Jet multi", "p_{T} [GeV]", "N_{jet}","Events", false);

    histo2D jetmultijetpt  (histo1D::createBinning(100,  -.5 , 4.5 ), histo1D::createBinning(100, 0 , 200),
            "gen Jet multi gen Jetpt", "N_{jet}", "p_{T}(jet) [GeV]","Events", false);

    histo2D jetptpttrans  (histo1D::createBinning(100, 0 , 200), histo1D::createBinning(100, -1.1 , 1.1),
            "jet pt vs pttrans", "p_{T}(jet) [GeV]", "2p_{T}^{#perp}/M_{W}","Events", false);

    histo2D detapttrans  (histo1D::createBinning(100, 0 , 6), histo1D::createBinning(100, -1.1 , 1.1),
            "deta vs pttrans", "#Delta#eta(l,j)", "2p_{T}^{#perp}/M_{W}","Events", false);

    histo2D muptpttrans  (histo1D::createBinning(100, 0 , 200), histo1D::createBinning(100, -1.1 , 1.1),
            "muon pt vs pttrans", "p_{T}(µ) [GeV]", "2p_{T}^{#perp}/M_{W}","Events", false);

    histo2D::c_makelist=false;

    HTransformToCS cs_transformer(8000);
    bool rewswon=!getNLOReweighter()->switchedOff() ;
    rewswon = rewswon&& isNLO&& signal_;
    getNLOReweighter()->switchOff(!rewswon);
    getMuonSF()->setIsMC(isMC_);
    getMuonESF()->setIsMC(isMC_);


    if(testmode_)
        entries/=20;//skip=(float) 1./0.05; //only consider 5 % of the events
    if(legendname_=="DUMMY")
        entries=0;

    t.setPreCache();//better performance for large samples

    ///////////////////////////////// Event loop //////////////////////////////
    reportStatus(0,entries);//give first report
    for(Long64_t event=0;event<entries;event++){
        reportStatus(event,entries);

        histo1DUnfold::flushAllListed();
        /*
         * Init event
         */
        size_t step=0;
        evt.reset();
        t.setEntry(event);



        /*
         * Link collections to event
         */
        float puweight=1;
        evt.puweight=&puweight;
        if(signal_ && isNLO){
            float lheweight=(float) *b_lheweight.content() / (10000. * 4.3446); //only sign is important scale to make the weight plots look nice
            lheReweighter.setNewWeight(lheweight);
            lheReweighter.reWeight(puweight);
        }
        mcweights.reWeight(puweight);
        NTEvent ntevt;
        evt.event=&ntevt;
        /*
         * Generator part
         */
        float phi_cs=0,costheta_cs=0,pttrans_gen=0,A7=0;
        std::vector<NTGenJet *> genjets;
        std::vector<NTGenParticle*> allgenparticles;
        NTGenParticle * lepton=0, *neutrino=0;
        NTGenParticle Wbos;
        if(isMC_){
            genjets=produceCollection(b_genjets.content());
            evt.genjets=&genjets;
            allgenparticles=produceCollection(b_genparticles.content());
            //filter status
            if(isNLO){
                std::vector<NTGenParticle*> tempgen;
                for(size_t i=0;i<allgenparticles.size();i++){
                    NTGenParticle * p=allgenparticles.at(i);
                    reco::GenStatusFlags flags;
                    int oldstatus=p->status();
                    flags.splitModifiedStatus(oldstatus);
                    if(isNLO &&  ! flags.isHardProcess())continue;

                    //if(! flags.fromHardProcessBeforeFSR())continue; gives zero particles
                    p->setStatus(3); //for compatibility
                    tempgen.push_back(p);
                }
                allgenparticles=tempgen; //copy back
            }
            evt.allgenparticles=&allgenparticles;

            ////first gen selection step (problem with tau decays)

            //get the first lepton. NO status code check

            //maybe favour masses around W mass if mass < 10 GeV (taus)
            pairLeptons(&allgenparticles,lepton,neutrino,isDYsig);
            if(lepton){//remove from genjets
                for(size_t i=0;i<genjets.size();i++)
                    if(dR(lepton,genjets.at(i))<0.05)
                        genjets.erase(genjets.begin()+i);
            }

            std::vector<NTGenParticle*> leptonsfromW;
            leptonsfromW.push_back(lepton);
            leptonsfromW.push_back(neutrino);

            selectGen.setCollection(&leptonsfromW);
            if(!selectGen.pass()) continue;

            evt.genlepton=lepton;
            evt.genneutrino=neutrino;

            //both always fit another in the samples. no need for special check
            //the tau only sample has no neutrinos
            if(lepton && neutrino){//construct W
                Wbos.setP4( lepton->p4() + neutrino->p4() );
                Wbos.setQ(lepton->q());
                evt.genW=&Wbos;
                cs_transformer.TransformToCS(neutrino,lepton);
                phi_cs=cs_transformer.GetPhi();
                costheta_cs=cs_transformer.GetCosTheta();
                evt.phi_cs=&phi_cs;
                evt.costheta_cs=&costheta_cs;
                pttrans_gen=analysisPlotsW::asymmValue(lepton ,&Wbos,false); //dont invert W direction
                evt.pttrans_gen=&pttrans_gen;
                evt.A7=&A7;

                A7=std::sin(phi_cs)*std::sqrt(1-costheta_cs*costheta_cs);

                if(genjets.size()>0)
                    getNLOReweighter()->prepareWeight(costheta_cs,phi_cs,&Wbos,genjets.at(0));
                else
                    getNLOReweighter()->prepareWeight(costheta_cs,phi_cs,&Wbos);
                //std::cout << puweight << std::endl;
                getNLOReweighter()->reWeight(puweight);
                //std::cout << puweight << std::endl;
                if(genjets.size())
                    ptttransA7corr.fill(pttrans_gen ,A7 ,puweight);
                if(genjets.size())
                    genstatHistDEtaJWpTW.fill(genjets.at(0)->eta()-Wbos.eta(), Wbos.pt(),puweight);
                if(genjets.size())
                    wptjetpt.fill(Wbos.pt(), genjets.at(0)->pt(),puweight);

                wptjetmulti.fill(Wbos.pt(), genjets.size() ,puweight);
                if(genjets.size())
                    jetmultijetpt.fill(genjets.size(),genjets.at(0)->pt(),puweight);
                else
                    jetmultijetpt.fill(0,0,puweight);
            }
            if(signal_ && !lepton) continue; //gen W match requirement
        }

        c_plots_gen.makeControlPlots(0);
        rewplots.fillPlots();
        //(at the end)
        anaplots.fillPlotsGen();

        //add pu weight
        if(isMC_)puweight*=* b_puweight.content();

        if(genonly_)continue;

        if(! b_goodevent.pass())continue;



        //only event requirements: step 0
        h.fill(step,puweight);
        c_plots.makeControlPlots(step++);


        //trigger requirement: step 1
        if(*b_trigger.content()<1) continue;




        ntevt.setVertexMulti((float) (* b_vertices.content()) );

        NTMuon leadinglep;
        evt.leadinglep=&leadinglep;


        h.fill(step,puweight);
        c_plots.makeControlPlots(step++);


        std::vector<NTMuon*>     allgoodmuons=produceCollection<NTMuon>    (b_goodLeptons.mu_content());
        std::vector<NTElectron*> allgoodelecs=produceCollection<NTElectron>(b_goodLeptons.e_content());
        evt.allmuons=&allgoodmuons;
        evt.allelectrons=&allgoodelecs;



        std::vector<NTMuon*> kinmuons,idmuons,isomuons,vetomuons, nonisomuons;
        evt.kinmuons=&kinmuons;
        evt.idmuons=&idmuons;
        evt.isomuons=&isomuons;
        evt.vetomuons=&vetomuons;
        evt.nonisomuons=&nonisomuons;


        /*
         * Lepton selection
         */


        for(size_t i=0;i<allgoodmuons.size();i++){
            NTMuon* muon=allgoodmuons.at(i);
            if(isMC_)
                muon->setP4(muon->p4()*getMuonESF()->getScalefactor(muon->pt()));

            if(muon->pt()<15) continue;
            if(fabs(muon->eta())>2.4)continue;

            vetomuons << muon;
            if(muon->pt()<25) continue;
            if(fabs(muon->eta())>2.1) continue;
            kinmuons << muon;

            if(! muon->isTightID()) continue;
            if(muon->d0V() > 0.2) continue;
            //if(muon->dzV() > 00) continue;
            idmuons << muon;

            if(isMC_)
                getChargeFlip()->flip(muon);

            if(muon->isoVal() < 0.12) { //INVERT SWITCH
                isomuons << muon;
            }
            else if(muon->isoVal() < 0.5){
                nonisomuons << muon;
            }
        }


        //at least one id muon: step 2
        if(idmuons.size()<1) continue;
        if(isMC_){
            puweight*= 1; //DEBUG *b_lep1weight.content();
            puweight*=getMuonSF()->getScalefactor(1);//global
        }

        h.fill(step,puweight);
        c_plots.makeControlPlots(step++);

        std::vector<NTMuon*>     allothermuons=produceCollection<NTMuon>   (b_otherLeptons.mu_content());
        for(size_t i=0;i<allothermuons.size();i++){
            NTMuon* muon=allothermuons.at(i);
            if(muon->pt()<15) continue;
            if(fabs(muon->eta())>2.4)continue;
            vetomuons << muon;
        }




        //veto step: step 3
        //the veto muons include one iso/noniso
        if(vetomuons.size()>1)continue;
        if(isomuons.size()+nonisomuons.size()!=1) continue; //exactly one iso or exactly one noniso
        //now there is exactly one muon
        bool isiso=true;
        if(nonisomuons.size()>0){
            isiso=false;
            leadinglep=*nonisomuons.at(0);
        }
        else{
            leadinglep=*isomuons.at(0);
        }


        std::vector<NTJet*> idjets, hardjets;
        evt.idjets=&idjets;
        evt.hardjets=&hardjets;


        /*
         * Jet selection
         */
        for(size_t i=0;i<b_Jets.content()->size();i++){
            NTJet* jet=&b_Jets.content()->at(i);

            if(isMC_){
                getJecUnc()->applyToJet(jet);
            }
            if(! jet->id()) continue;
            idjets << jet;


            if(jet->pt()<30)continue;
            if(fabs(jet->eta())>2.4)continue;
            hardjets << jet;

        }


        if(isiso){
            h.fill(step,puweight);
            c_plots.makeControlPlots(step);
        }
        step++;

        //at least one jet: step 4
        if(hardjets.size()<1) continue;
        evt.leadingjet=hardjets.at(0);



        float pttrans=analysisPlotsW::asymmValue(&leadinglep,hardjets.at(0));
        evt.pttrans=&pttrans;
        if(isiso && evt.genW){
            ptttransA7corrrec.fill(pttrans, pttrans_gen, puweight );
            ptttransA7corrgenrec.fill(pttrans, A7, puweight );
        }
        if(isiso){
            h.fill(step,puweight);
            c_plots.makeControlPlots(step);
            jetptpttrans.fill( hardjets.at(0)->pt(), pttrans, puweight);
            muptpttrans.fill( leadinglep.pt(), pttrans, puweight);
            detapttrans.fill( fabs(hardjets.at(0)->eta() - leadinglep.eta()), pttrans, puweight);

        }
        step++;



        //add control plots for iso muon and noniso muon areas
        if(isomuons.size()>0){
            anaplots.setIsIso(true);
            //step 5
            h.fill(step,puweight);
            c_plots.makeControlPlots(step);
        }
        else if(nonisomuons.size()>0){
            anaplots.setIsIso(false);
            step++;
            //step 6
            h.fill(step,puweight);
            c_plots.makeControlPlots(step);
        }
        else{
            continue;
        }



        //the class is protected internally against the latter case
        anaplots.fillPlotsReco();

    }

    ////////////////////// Event loop end ///////////////////



    /*
     * get norm right
     */


    std::cout << inputfile_<< ": before renorm " << norm_ << std::endl;
    norm_*=getNLOReweighter()->getRenormalization();
    norm_*=lheReweighter.getRenormalization();
    norm_*=mcweights.getRenormalization();

    std::cout <<  inputfile_<< ": end norm " << norm_ << std::endl;
    std::cout <<  inputfile_ << std::endl;
    t.printStats();

    //	std::cout << "end norm for " << inputfile_ << ": "<< norm_<<std::endl;
    processEndFunction();
    //	tBranchHandlerBase::debug=true;
    //	tTreeHandler::debug=true;

}


void wAnalyzer::createNormalizationInfo(tTreeHandler* t){

    if(!isMC_){ norm_=1; return;}

    size_t idx=ownChildIndex();
    double xsec=norms_.at(idx);
    double entries=t->entries();

    norm_= lumi_ * xsec / entries;
}

}//ns


/* Notes area
 *
 *
 * Error in <TBranch::GetBasket>: File: /nfs/dust/cms/user/kiesej/trees_WA7/new/tree_SingleMuDI.root at byte:1990165022444925819, branch:checktrackingFailure, entry:14787546, badread=0, nerrors=1, basketnumber=2009
 *
 *
 */

