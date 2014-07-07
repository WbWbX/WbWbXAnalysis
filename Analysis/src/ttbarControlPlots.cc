/*
 * ttbarControlPlots.cc
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#include "../interface/ttbarControlPlots.h"
#include "../interface/NTFullEvent.h"
#include "TtZAnalysis/DataFormats/interface/mathdefs.h"

#include "TtZAnalysis/DataFormats/src/classes.h"

#include <math.h>

namespace ztop{
/**
 * setbins, addplot, FILL
 *
 * NEVER restrict the addPlot statement by an (if) statement
 * 	-> SegFaults if plots are filled/init before variable is available.
 * Otherwise safe
 *
 */
void ttbarControlPlots::makeControlPlots(const size_t & step){

    if(!switchedon_) return;


    initStep(step);
    using namespace std;

    bool middphi=false;
    bool middphiInfo=false;
    if(event()->midphi){
        middphiInfo=true;
        middphi=*(event()->midphi);
    }
    ///GEN
    SETBINSRANGE(50,0,500);
    addPlot("top pt", "p_{T}^{t}[GeV]","N_{t}/GeV");
    FILLFOREACH(gentops,pt());


    ///LEPTONS

    SETBINS << -2.5 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.5;
    addPlot("electron eta", "#eta_{l}","N_{e}");
    FILLFOREACH(isoelectrons,eta());


    SETBINSRANGE(50,0,1);
    addPlot("electron isolation", "Iso", "N_{e}");
    FILLFOREACH(idelectrons,rhoIso());

    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot("electron pt", "p_{T} [GeV]", "N_{e}");
    FILLFOREACH(isoelectrons,pt());

    SETBINSRANGE(50,0,1);
    addPlot("electron mva id", "Id", "N_{e}");
    FILLFOREACH(kinelectrons,mvaId());

    SETBINSRANGE(8,-0.5,7.5);
    addPlot("iso lepton multi","N_{l,iso}","N_{evt}");
    if(event()->isoleptons){
        last()->fill(event()->isoleptons->size(),*event()->puweight);
    }

    SETBINSRANGE(8,-0.5,7.5);
    addPlot("very loose lepton multi","N_{l_[vloose}}","N_{evt}");
    if(event()->allleptons){
        last()->fill(event()->allleptons->size(),*event()->puweight);
    }

    ////////angluar stuff! beware - needs to be done properly later


    SETBINSRANGE(80,-1.0,1.0);
    addPlot("cos_Angle(ll)", "cos#theta_{ll}","Nleptons/bw");
    FILLSINGLE(cosleplepangle);

    SETBINSRANGE(40,0.0,(2*M_PI));
    addPlot("delta_theta_lep combined with dphi(ll,jet)", "(cos#theta_{ll}+1)*#Delta#phi(ll,j)","N_{evt}/bw");
    if(middphiInfo){
        float test_var1;
        test_var1 =  ((*(event()->cosleplepangle)+1)* (*(event()->dphillj)));
        last()->fill(test_var1,*(event()->puweight));
    }

    SETBINSRANGE(40,0.0,M_PI);
    addPlot("dphi(ll,jets)", "#Delta#phi(ll,jets)","N_{evt}/bw");
    if(event()->leadinglep && event()->secleadinglep
            && event()->hardjets){
        NTLorentzVector<float> jetsp4;
        for(size_t i=0;i<event()->hardjets->size();i++)
            jetsp4= jetsp4+event()->hardjets->at(i)->p4();
        NTLorentzVector<float> lepp4=event()->leadinglep->p4()+event()->secleadinglep->p4();
        float dphilljets=fabs(jetsp4.Phi()-lepp4.Phi());
        dphilljets=M_PI-fabs(dphilljets-M_PI);
        last()->fill(dphilljets ,*(event()->puweight));

    }


    SETBINSRANGE(80,-5.0,5.0);
    addPlot("delta_eta_lep", "#Delta#eta_{ll}","Nleptons/bw");
    if(event()->leadinglep && event()->secleadinglep){
        float delta_eta;

        delta_eta = event()->leadinglep->p4().Eta() - event()->secleadinglep->p4().Eta();

        last()->fill(delta_eta,*(event()->puweight));
    }

    SETBINSRANGE(80,0.0,12.0);
    addPlot("delta_eta_lep combined with dphi(ll,jet)", "#Delta#eta_{ll}*(#Delta#phi - #pi)","Nleptons/bw");
    if(middphiInfo){
        float delta_theta, test_var1;

        delta_theta = event()->leadinglep->p4().Eta() - event()->secleadinglep->p4().Eta();
        test_var1 =  fabs(delta_theta)*fabs((*(event()->dphillj) - M_PI));

        last()->fill(test_var1,*(event()->puweight));
    }

    SETBINSRANGE(50,0,6);
    addPlot("leadlep-secleadlep dR", "dR", "N_{e}*N_{#mu}/bw",true);
    FILLSINGLE(leplepdr);



    SETBINS << -2.4 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.4;
    addPlot("muon eta", "#eta_{l}","N_{#mu}");
    FILLFOREACH(isomuons,eta());

    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot( "muon pt", "p_{T} [GeV]", "N_{#mu}/bw");
    FILLFOREACH(isomuons,pt());

    SETBINSRANGE(50,0,1);
    addPlot("muon isolation", "Iso", "N_{#mu}/bw");
    FILLFOREACH(idmuons,isoVal());

    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot("lead lepton pt", "p_{T,l1} [GeV]", "N_{evt}/GeV");
    FILL(leadinglep,pt());

    addPlot("seclead lepton pt", "p_{T,l2} [GeV]", "N_{evt}/GeV");
    FILL(secleadinglep,pt());

    SETBINSRANGE(20,-2.5,2.5);
    addPlot("lead lepton eta", "#eta_{l1}", "N_{evt}/0.25");
    FILL(leadinglep,eta());

    addPlot("seclead lepton eta", "#eta_{l2}", "N_{evt}/0.25");
    FILL(secleadinglep,eta());


    addPlot("leadAllLep pt", "p_{T} [GeV]", "N_{l}/GeV");
    if(event()->allleptons && event()->allleptons->size()>0)
        FILL(allleptons->at(0),pt());
    addPlot("secLeadAllLep pt", "p_{T} [GeV]", "N_{l}/GeV");
    if(event()->allleptons && event()->allleptons->size()>1)
        FILL(allleptons->at(1),pt());

    SETBINSRANGE(30,0,0.01);
    addPlot("lept d0V", "d0V [cm]", "N_{l}[1/cm]");
    FILLFOREACH(allleptons,d0V());

    SETBINSRANGE(150,-1,1);
    addPlot("lep-lep dZ", "dZ [cm]", "N_{l}[1/cm]");
    if(event()->leadinglep && event()->secleadinglep){
        last()->fill(event()->leadinglep->dzV()-event()->secleadinglep->dzV(),*(event()->puweight));
    }


    SETBINSRANGE(100,-1,1);
    addPlot("lept dZV", "dZV [cm]", "N_{l}[1/cm]");
    FILLFOREACH(allleptons,dzV());

    ////JETS & MET

    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot("leading id jet pt","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>0)
        FILL(idjets->at(0),pt());
    addPlot("secleading id jet pt","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>1)
        FILL(idjets->at(1),pt());
    addPlot("idjets pt","p_{T} [GeV]", "N_{j}/GeV");
    FILLFOREACH(idjets,pt());


    addPlot("leading b-jet pt","p_{T,b} [GeV]", "N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()>0)
        FILL(selectedbjets->at(0),pt());

    SETBINSRANGE(10,-0.5,9.5);
    addPlot("selected jets multi", "N_{jet}","N_{evt}");
    FILL(selectedjets,size());
  /*  addPlot("med jet multi", "N_{jet}", "N_{evt}");
    FILL(medjets,size());
    addPlot("dphillj jets multi", "N_{jet}", "N_{evt}");
    FILL(dphilljjets,size());
    addPlot("dphiplushard jets multi", "N_{jet}", "N_{evt}");
    FILL(dphiplushardjets,size()); */
    SETBINSRANGE(6,-0.5,5.5);
    addPlot("selected b-jet multi","N_{b}","N_{evt}",true);
    FILL(selectedbjets,size());



    SETBINSRANGE(40,-1.2,1.2);
    addPlot("leading jet btag","D_{1^{st}jet}","evt/bw");
    if(event()->selectedjets && event()->selectedjets->size()>0)
        FILL(selectedjets->at(0),btag()) ;

    SETBINSRANGE(40,-1.2,1.2);
    addPlot("secleading jet btag","D_{1^{st}jet}","evt/bw");
    if(event()->selectedjets && event()->selectedjets->size()>1)
        FILL(selectedjets->at(1),btag()) ;

    SETBINSRANGE(40,-1.2,1.2);
    addPlot("all jets btags","D_{b-tag}","N_{jets}/bw");
    FILLFOREACH(selectedjets,btag());


    SETBINSRANGE(20,-2.5,2.5);
    addPlot("leading jet eta","#eta_{1^{st}jet}","evt/bw");
    if(event()->selectedjets && event()->selectedjets->size()>0)
        FILL(selectedjets->at(0),eta()) ;

    SETBINSRANGE(20,-2.5,2.5);
    addPlot("leading b-jet eta","#eta_{b}","N_{evt}/0.25");
    if(event()->selectedbjets && event()->selectedbjets->size()>0)
        FILL(selectedbjets->at(0),eta()) ;


    SETBINSRANGE(20,-2.5,2.5);
    addPlot("secleading jet eta","#eta_{1^{st}jet}","evt/bw");
    if(event()->selectedjets && event()->selectedjets->size()>1)
        FILL(selectedjets->at(1),eta()) ;


    SETBINSRANGE(80,0,400);
    addPlot("met adjusted","E_{T}^{miss} [GeV]","N_{evt}/GeV");
    FILL(adjustedmet,met());


    SETBINSRANGE(40,-M_PI,M_PI);
    addPlot("met adjusted phi","#phi(E_{T}^{miss})","N_{evt}/bw");
    FILL(adjustedmet,phi());



    SETBINSRANGE(80,-0.5,0.5);
    addPlot("jet phi resolution", "#Delta_{}^{#phi}(gen,reco)","/bw");
    if(event()->idjets){
        for(size_t i=0;i<event()->idjets->size();i++){
            float genpt=event()->idjets->at(i)->genP4().Pt();
            if(genpt >0){
                last()->fill((event()->idjets->at(i)->phi() - event()->idjets->at(i)->genP4().Phi()),*(event()->puweight));
            }
        }
    }

    SETBINSRANGE(80,0,300);
    addPlot("S4 pt","p_{T}^{S4} [GeV]","N_{evt}/GeV");
    FILL(S4,pt());

    SETBINSRANGE(80,0,400);
    addPlot("allobjects pt","p_{T}^{all} [GeV]","N_{evt}/GeV");
    FILL(allobjects4,pt());

    //Combined vars

    SETBINSRANGE(150,0,300);
    addPlot("mll Range","m_{ll} [GeV]","N_{evt}/GeV");
    FILLSINGLE(mll);

    SETBINSRANGE(40,0,300);
    addPlot("HT","H_{T}","N_{evt}/GeV");
    FILLSINGLE(ht);

    SETBINSRANGE(20,60,300);
    addPlot("pt llj","p_{T}(llj)","N_{evt}/GeV");
    FILLSINGLE(ptllj);

    SETBINSRANGE(30,0,M_PI);
    addPlot("dphi (ll,j)","#Delta#phi(ll,j)","N_{evt}/GeV");
    FILLSINGLE(dphillj);

    SETBINSRANGE(30,0,M_PI);
    addPlot("dphi (ll,jj)","#Delta#phi(ll,jj)","N_{evt}/GeV");
    FILLSINGLE(dphilljj);

    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,j)","#Delta#eta(ll,j)","N_{evt}/GeV");
    FILLSINGLE(detallj);

    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,jj)","#Delta#eta(ll,jj)","N_{evt}/GeV");
    FILLSINGLE(detalljj);

    SETBINSRANGE(30, 0, 2*M_PI);
    addPlot("sum dphi l1met l2met","#Delta#phi(met,l_{1})+#Delta#phi(met,l_{2})","N_{evt}/bw");
    FILLSINGLE(lhi_sumdphimetl);

    ///middphi range


    SETBINSRANGE(30,0,M_PI);
    addPlot("dphi ((ll),j) midphi","#Delta#phi(ll,j)","N_{evt}/GeV");
    if(middphi && middphiInfo) FILLSINGLE(dphillj);

    SETBINSRANGE(20,60,300);
    addPlot("pt llj midphi","p_{T}(llj)","N_{evt}/GeV");
    if(middphi && middphiInfo) FILLSINGLE(ptllj);

    SETBINSRANGE(10,-0.5,9.5);
    addPlot("med jet multi midphi", "N_{jet}","N_{evt}");
    if(middphi && middphiInfo) FILL(medjets,size());
    addPlot("hard jet multi midphi", "N_{jet}","N_{evt}");
    if(middphi && middphiInfo) FILL(hardjets,size());


    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot("leading id jet pt midphi","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>0)
        if(middphi && middphiInfo) FILL(idjets->at(0),pt());
    addPlot("secleading id jet pt midphi","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>1)
        if(middphi && middphiInfo) FILL(idjets->at(1),pt());

    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,j) midphi","#Delta#eta(ll,j)","N_{evt}/GeV");
    if(middphi && middphiInfo)FILLSINGLE(detallj);

    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,jj) midphi","#Delta#eta(ll,jj)","N_{evt}/GeV");
    if(middphi && middphiInfo)FILLSINGLE(detalljj);

    ///NOT MIDPHI RANGE


    SETBINSRANGE(30,0,M_PI);
    addPlot("dphi ((ll),j) NOTmidphi","#Delta#phi(ll,j)","N_{evt}/GeV");
    if(!middphi && middphiInfo) FILLSINGLE(dphillj);

    SETBINSRANGE(20,60,300);
    addPlot("pt llj NOTmidphi","p_{T}(llj)","N_{evt}/GeV");
    if(!middphi && middphiInfo) FILLSINGLE(ptllj);

    SETBINSRANGE(10,-0.5,9.5);
    addPlot("med jet multi NOTmidphi", "N_{jet}","N_{evt}");
    if(!middphi && middphiInfo) FILL(medjets,size());
    addPlot("hard jet multi NOTmidphi", "N_{jet}","N_{evt}");
    if(!middphi && middphiInfo) FILL(hardjets,size());

    SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
    addPlot("leading id jet pt NOTmidphi","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>0)
        if(!middphi && middphiInfo) FILL(idjets->at(0),pt());
    addPlot("secleading id jet pt NOTmidphi","p_{T} [GeV]", "N_{j}/GeV");
    if(event()->idjets && event()->idjets->size()>1)
        if(!middphi && middphiInfo) FILL(idjets->at(1),pt());


    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,j) NOTmidphi","#Delta#eta(ll,j)","N_{evt}/bw");
    if(!middphi && middphiInfo) FILLSINGLE(detallj);

    SETBINSRANGE(30,-8,8);
    addPlot("deta (ll,jj) NOTmidphi","#Delta#eta(ll,jj)","N_{evt}/bw");
    if(!middphi && middphiInfo) FILLSINGLE(detalljj);


    ///fancy vars

    SETBINSRANGE(50,0,1);
    addPlot("top discr","D_{top}=(cos#theta_{ll}+1)/2*#Delta#phi(ll,j)/#pi*(-D_{b-tag}+1)","N_{evt}/bw",true);
    FILLSINGLE(topdiscr);

    SETBINSRANGE(50,0,1);
    addPlot("top discr2","D_{top,2}=(cos#theta_{ll}+1)/2*(-D_{b-tag}+1)","N_{evt}/bw",true);
    FILLSINGLE(topdiscr2);

    SETBINSRANGE(50,0,1);
    addPlot("top discr3","D_{top,3}=(#Delta#phi(ll,j)/#pi*(-D_{b-tag}+1)","N_{evt}/bw",true);
    FILLSINGLE(topdiscr3);


    ///vars for single top disambitguation
    //some thoughts

    SETBINSRANGE(50,1,10);
    addPlot("lep pt ratio","R_{p_{T}}","N_{evt}/bw");
    if(event()->leadinglep && event()->secleadinglep){

        last()->fill(event()->leadinglep->pt()/event()->secleadinglep->pt(),*event()->puweight);

    }
    /*

    SETBINSRANGE(50,0,300);
    addPlot("non lb lep pt", "p_{T}^{l_{not}} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->leadinglep && event()->secleadinglep){
        if(event()->selectedbjets->size() > 0){
            const NTLorentzVector<float> &firstjet=event()->selectedbjets->at(0)->p4();
            const NTLorentzVector<float> &leadlep=event()->leadinglep->p4();
            const NTLorentzVector<float> &secleadlep=event()->secleadinglep->p4();
            NTLepton * usedlep=event()->leadinglep;
            NTLepton * otherlep=event()->secleadinglep;
            NTLorentzVector<float> lbp4=firstjet+leadlep;
            if(lbp4.m() > 165){
                lbp4=firstjet+secleadlep;
                usedlep=event()->secleadinglep;
                otherlep=event()->leadinglep;
            }

            last()->fill(otherlep->pt(),*event()->puweight);

        }
    }

    SETBINSRANGE(50,0,100);
    addPlot("lb pt-otherlep pt", "p_{T}^{l_{not}} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->leadinglep && event()->secleadinglep){
        if(event()->selectedbjets->size() > 0){
            const NTLorentzVector<float> &firstjet=event()->selectedbjets->at(0)->p4();
            const NTLorentzVector<float> &leadlep=event()->leadinglep->p4();
            const NTLorentzVector<float> &secleadlep=event()->secleadinglep->p4();
            NTLepton * usedlep=event()->leadinglep;
            NTLepton * otherlep=event()->secleadinglep;
            NTLorentzVector<float> lbp4=firstjet+leadlep;
            if(lbp4.m() > 165){
                lbp4=firstjet+secleadlep;
                usedlep=event()->secleadinglep;
                otherlep=event()->leadinglep;
            }

            last()->fill(fabs(lbp4.pt( )- otherlep->pt()),*event()->puweight);

        }
    }
    SETBINSRANGE(50,0,2*M_PI);
    addPlot("lb phi-met phi", "p_{T}^{l_{not}} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->leadinglep && event()->secleadinglep && event()->adjustedmet){
        if(event()->selectedbjets->size() > 0){
            const NTLorentzVector<float> &firstjet=event()->selectedbjets->at(0)->p4();
            const NTLorentzVector<float> &leadlep=event()->leadinglep->p4();
            const NTLorentzVector<float> &secleadlep=event()->secleadinglep->p4();
            NTLepton * usedlep=event()->leadinglep;
            NTLepton * otherlep=event()->secleadinglep;
            NTLorentzVector<float> lbp4=firstjet+leadlep;
            if(lbp4.m() > 165){
                lbp4=firstjet+secleadlep;
                usedlep=event()->secleadinglep;
                otherlep=event()->leadinglep;
            }

            last()->fill(fabs(lbp4.phi( )- event()->adjustedmet->phi()),*event()->puweight);

        }
    }

    SETBINSRANGE(50,0,5.9);
    addPlot("dR(lb,l)", "p_{T}^{l_{not}} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->leadinglep && event()->secleadinglep && event()->adjustedmet){
        if(event()->selectedbjets->size() > 0){
            const NTLorentzVector<float> &firstjet=event()->selectedbjets->at(0)->p4();
            const NTLorentzVector<float> &leadlep=event()->leadinglep->p4();
            const NTLorentzVector<float> &secleadlep=event()->secleadinglep->p4();
            NTLepton * usedlep=event()->leadinglep;
            NTLepton * otherlep=event()->secleadinglep;
            NTLorentzVector<float> lbp4=firstjet+leadlep;
            if(lbp4.m() > 165){
                lbp4=firstjet+secleadlep;
                usedlep=event()->secleadinglep;
                otherlep=event()->leadinglep;
            }

            last()->fill(dR_3d(lbp4,otherlep->p4()),*event()->puweight);

        }
    }
    ///EVT VARS
     */

    SETBINSRANGE(35,0,35);
    addPlot("vertex multiplicity", "n_{vtx}", "N_{evt}");
    FILL(event,vertexMulti());

    SETBINSRANGE(50,0,2);
    addPlot("event weight","w","N_{evt}");
    FILLSINGLE(puweight);


    ///for likelihoods

    SETBINSRANGE(100,0,1);
    addPlot("top likelihood output","D","N_{evt}/bw");
    FILLSINGLE(lh_toplh);






    SETBINSRANGE(40,0,1);
    addPlot("top likelihood output less bins","D","N_{evt}/bw");
    FILLSINGLE(lh_toplh);

    SETBINSRANGE(100,-200,100);
    addPlot("DXi","D_{#xi}","N_{evt}/bw");
    if(event()->leadinglep && event()->secleadinglep && event()->adjustedmet){
        /*
         * from alexei
         */
        NTVector lep1p=event()->leadinglep->p4().getNTVector();
        NTVector lep2p=event()->secleadinglep->p4().getNTVector();
        NTVector metp=event()->adjustedmet->p4().getNTVector();
        NTVector bis=bisector(lep1p,lep2p);
        float pxivis=(lep1p + lep2p) * bis;
        float pnots=metp*bis;

        float dxi=pnots- 0.85* pxivis;
        //std::cout << metp.x() << " "<< metp.y() << " "<< metp.z() << " "<< pxivis<< " " << pnots<< " " <<dxi << std::endl;
        last()->fill(dxi,*event()->puweight);

    }


    ///FOR EXTRACTION!! DO NOT CHANGE THESE NAMES!

    SETBINSRANGE(15,0,1);
    addPlot("top likelihood output 0 b-jets","D","N_{evt}/bw");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILLSINGLE(lh_toplh);

    SETBINSRANGE(15,0,1);
    addPlot("top likelihood output 1 b-jets","D","N_{evt}/bw");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILLSINGLE(lh_toplh);

    SETBINSRANGE(15,0,1);
    addPlot("top likelihood output 2 b-jets","D","N_{evt}/bw");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILLSINGLE(lh_toplh);

    SETBINSRANGE(15,0,1);
    addPlot("top likelihood output 3+ b-jets","D","N_{evt}/bw");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILLSINGLE(lh_toplh);





    SETBINSRANGE(15,20,300);
    addPlot("mll 0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILLSINGLE(mll);

    SETBINSRANGE(15,20,300);
    addPlot("mll 1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILLSINGLE(mll);

    SETBINSRANGE(15,20,300);
    addPlot("mll 2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILLSINGLE(mll);

    SETBINSRANGE(15,20,300);
    addPlot("mll 3+ b-jets","m_{ll} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILLSINGLE(mll);





    SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
    addPlot("lead lep pt 0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILL(leadinglep,pt());


    SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
    addPlot("lead lep pt 1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILL(leadinglep,pt());

    SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
    addPlot("lead lep pt 2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILL(leadinglep,pt());

    SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
    addPlot("lead lep pt 3+ b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILL(leadinglep,pt());




    SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
    addPlot("sec lep pt 0 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILL(secleadinglep,pt());

    SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
    addPlot("sec lep pt 1 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILL(secleadinglep,pt());

    SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
    addPlot("sec lep pt 2 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILL(secleadinglep,pt());

    SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
    addPlot("sec lep pt 3+ b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILL(secleadinglep,pt());






    SETBINS << 0 << 1 << 1.5 << 2 << 2.25 << 2.5 <<2.75 << 3 << 3.2 << 3.4 << 3.6 <<3.8 << 4 <<4.5 << 5<<6;
    addPlot("dRll 0 b-jets","#Delta R(ll)","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILLSINGLE(leplepdr);

    SETBINS << 0 << 1 << 1.5 << 2 << 2.25 << 2.5 <<2.75 << 3 << 3.2 << 3.4 << 3.6 <<3.8 << 4 <<4.5 << 5<<6;
    addPlot("dRll 1 b-jets","#Delta R(ll)","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILLSINGLE(leplepdr);

    SETBINS << 0 << 1 << 1.5 << 2 << 2.25 << 2.5 <<2.75 << 3 << 3.2 << 3.4 << 3.6 <<3.8 << 4 <<4.5 << 5<<6;
    addPlot("dRll 2 b-jets","#Delta R(ll)","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILLSINGLE(leplepdr);

    SETBINS << 0 << 1 << 1.5 << 2 << 2.25 << 2.5 <<2.75 << 3 << 3.2 << 3.4 << 3.6 <<3.8 << 4 <<4.5 << 5<<6;
    addPlot("dRll 3+ b-jets","#Delta R(ll)","N_{evt}/GeV");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILLSINGLE(leplepdr);



    SETBINSRANGE(1,0,1);
    addPlot("total 0 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        last()->fill(1,*event()->puweight);

    SETBINSRANGE(1,0,1);
    addPlot("total 1 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        last()->fill(1,*event()->puweight);

    SETBINSRANGE(1,0,1);
    addPlot("total 2 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        last()->fill(1,*event()->puweight);

    SETBINSRANGE(1,0,1);
    addPlot("total 3+ b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        last()->fill(1,*event()->puweight);



    SETBINSRANGE(7,-0.5,6.5);
    addPlot("Njets 0 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==0)
        FILL(selectedjets,size());

    SETBINSRANGE(7,-0.5,6.5);
    addPlot("Njets 1 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==1)
        FILL(selectedjets,size());

    SETBINSRANGE(7,-0.5,6.5);
    addPlot("Njets 2 b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()==2)
        FILL(selectedjets,size());

    SETBINSRANGE(7,-0.5,6.5);
    addPlot("Njets 3+ b-jets","","N_{evt}");
    if(event()->selectedbjets && event()->selectedbjets->size()>2)
        FILL(selectedjets,size());





}

}//namespace

