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



void ttbarControlPlots::setJetCategory(size_t nbjets,size_t njets){
	if(nbjets < 1 || nbjets>2){
		if(njets<1)
			jetcategory=cat_0bjet0jet;
		else if(njets<2)
			jetcategory=cat_0bjet1jet;
		else if(njets<3)
			jetcategory=cat_0bjet2jet;
		else if(njets>2)
			jetcategory=cat_0bjet3jet;
	}
	else if(nbjets < 2){
		if(njets<1)
			jetcategory=cat_1bjet0jet;
		else if(njets<2)
			jetcategory=cat_1bjet1jet;
		else if(njets<3)
			jetcategory=cat_1bjet2jet;
		else if(njets>2)
			jetcategory=cat_1bjet3jet;
	}
	else if(nbjets < 3){
		if(njets<1)
			jetcategory=cat_2bjet0jet;
		else if(njets<2)
			jetcategory=cat_2bjet1jet;
		else if(njets<3)
			jetcategory=cat_2bjet2jet;
		else if(njets>2)
			jetcategory=cat_2bjet3jet;
	}
}

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
	if(limittostep_>-1 && (size_t)limittostep_!=step)
		return;
	using namespace std;

	bool middphi=false;
	bool middphiInfo=false;
	if(event()->midphi){
		middphiInfo=true;
		middphi=*(event()->midphi);
	}
	///GEN
	SETBINSRANGE(50,166,180);
	addPlot("top mass", "m_{t}[GeV]","N_{t}/GeV");
	FILLFOREACH(gentops,m());


	///LEPTONS

	SETBINS << -2.4 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.4;
	addPlot("electron eta", "#eta_{l}","N_{e}");
	FILLFOREACH(isoelectrons,eta());

	SETBINS << -2.4 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.4;
	addPlot("kin electron eta pt 20-30", "#eta_{l}","N_{e}");
	if(event()->kinelectrons && event()->kinelectrons->size()>0 && event()->kinelectrons->at(0)
			&& event()->kinelectrons->at(0)->pt()<30)
		FILL(kinelectrons,at(0)->eta());
	if(event()->kinelectrons && event()->kinelectrons->size()>1  && event()->kinelectrons->at(1)
			&& event()->kinelectrons->at(1)->pt()<30)
		FILL(kinelectrons,at(1)->eta());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("kinelectron E", "p_{T} [GeV]", "N_{e}");
	FILLFOREACH(kinelectrons,E());



	SETBINSRANGE(50,0,1);
	addPlot("electron isolation", "Iso", "N_{e}");
	FILLFOREACH(idelectrons,rhoIso());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("electron pt", "p_{T} [GeV]", "N_{e}");
	FILLFOREACH(isoelectrons,pt());

	SETBINSRANGE(50,0,1);
	addPlot("electron mva id", "Id", "N_{e}");
	FILLFOREACH(kinelectrons,storedId());

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

	SETBINSRANGE(30,0,0.2);
	addPlot("muon dZV", "Dz", "N_{#mu}/bw");
	FILLFOREACH(idmuons,dzV());


	SETBINSRANGE(30,0,0.2);
	addPlot("muon d0V", "D0", "N_{#mu}/bw");
	FILLFOREACH(idmuons,d0V());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("lead lepton pt", "p_{T,l1} [GeV]", "N_{evt}/GeV");
	FILL(leadinglep,pt());

	addPlot("seclead lepton pt", "p_{T,l2} [GeV]", "N_{evt}/GeV");
	FILL(secleadinglep,pt());

	SETBINSRANGE(20,-2.4,2.4);
	addPlot("lead lepton eta", "#eta_{l1}", "N_{evt}/0.25");
	FILL(leadinglep,eta());

	addPlot("seclead lepton eta", "#eta_{l2}", "N_{evt}/0.25");
	FILL(secleadinglep,eta());

	SETBINS << -2.4 << -1.5 << -0.5 << 0.5 << 1.5 << 2.4;
	addPlot("lead lepton eta coarse", "#eta_{l1}", "N_{evt}/1");
	FILL(leadinglep,eta());

	addPlot("seclead lepton eta coarse", "#eta_{l2}", "N_{evt}/1");
	FILL(secleadinglep,eta());


	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
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


	SETBINSRANGE(20,-2.4,2.4);
	addPlot("leading jet eta","#eta_{1^{st}jet}","evt/bw");
	if(event()->selectedjets && event()->selectedjets->size()>0)
		FILL(selectedjets->at(0),eta()) ;

	SETBINSRANGE(20,-2.4,2.4);
	addPlot("leading b-jet eta","#eta_{b}","N_{evt}/0.25");
	if(event()->selectedbjets && event()->selectedbjets->size()>0)
		FILL(selectedbjets->at(0),eta()) ;

	SETBINSRANGE(5,-2.4,2.4);
	addPlot("leading b-jet eta coarse","#eta_{b}","N_{evt}/1");
	if(event()->selectedbjets && event()->selectedbjets->size()>0)
		FILL(selectedbjets->at(0),eta()) ;


	SETBINSRANGE(20,-2.4,2.4);
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

	SETBINSRANGE(50,0,300);
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


	///vars for single top disambiguation
	//some thoughts

	SETBINSRANGE(50,1,10);
	addPlot("lep pt ratio","R_{p_{T}}","N_{evt}/bw");
	if(event()->leadinglep && event()->secleadinglep){

		last()->fill(event()->leadinglep->pt()/event()->secleadinglep->pt(),*event()->puweight);

	}


	SETBINSRANGE(35,0,35);
	addPlot("vertex multiplicity", "n_{vtx}", "N_{evt}");
	FILL(event,vertexMulti());

	SETBINSRANGE(50,0,2);
	addPlot("event weight","w","N_{evt}");
	FILLSINGLE(puweight);




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



	////////top xsec
	size_t nbjets=0;
	size_t naddjets=0;
	if(event()->selectedbjets && event()->selectedjets){
		nbjets=event()->selectedbjets->size();
		naddjets=event()->selectedjets->size() - nbjets;
	}

	setJetCategory(nbjets,naddjets);


	SETBINSRANGE(15,20,300);
	addPlot("mll 0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(nbjets==0 || nbjets>2)
		FILLSINGLE(mll);

	SETBINSRANGE(15,20,300);
	addPlot("mll 1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(nbjets==1)
		FILLSINGLE(mll);

	SETBINSRANGE(15,20,300);
	addPlot("mll 2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(nbjets==2)
		FILLSINGLE(mll);



	//end more dimensions

	SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
	addPlot("lead lep pt 0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(nbjets==0 || nbjets>2)
		FILL(leadinglep,pt());


	addPlot("lead lep pt 1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(nbjets==1)
		FILL(leadinglep,pt());

	addPlot("lead lep pt 2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(nbjets==2)
		FILL(leadinglep,pt());





	////end more dimensions

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 0 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
	if(nbjets==0 || nbjets>2)
		FILL(secleadinglep,pt());

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 1 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
	if(nbjets==1)
		FILL(secleadinglep,pt());

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 2 b-jets","p_{t}^{l2} [GeV]","N_{evt}/GeV");
	if(nbjets==2)
		FILL(secleadinglep,pt());







	SETBINSRANGE(1,-0.5,0.5);
	addPlot("total yield","","N_{evt}");
	if(event()->puweight)
		last()->fill(1,*event()->puweight);

}

}//namespace

