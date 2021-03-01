/*
 * testDauMoth.cc
 *
 *  Created on: Jan 14, 2015
 *      Author: kiesej
 */




#include "../interface/dataFormatHelpers.h"
#include "../interface/NTGenParticle.h"
#include "TTree.h"
#include "TFile.h"
#include "WbWbXAnalysis/Analysis/interface/tBranchHandler.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


/**
 * Fill in a daughter collection not whole b-collection
 * Makes use of the fact that inv mass of b's from gluon splitting is 0
 */
ztop::NTGenParticle * pickDirectDecayB(const std::vector<ztop::NTGenParticle*> inbs){
	using namespace ztop;
	ztop::NTGenParticle * out=0;
	std::vector<size_t> exclude;
	for(size_t i=0;i<inbs.size();i++){
		bool right=true;
		out=inbs.at(i);
		float mass;
		for(size_t j=i;j<inbs.size();j++){
			if(i==j) continue;
			const NTLorentzVector<float>& p1= inbs.at(i)->p4();
			const NTLorentzVector<float>& p2= inbs.at(j)->p4();
			mass=(p1+p2).m();
			if(mass < 9.7){ //two bs.. i!=j does same trick
				right=false;
				//break;
			}
			std::cout << "mass: ("<<i <<","<<j<<") "<< mass << std::endl;
		}
		std::cout << "Energie ("<<inbs.at(i)->genId() <<")" << inbs.at(i)->e()<< " mother: " << inbs.at(i)->mothers().at(0)->e() <<std::endl;

	}
	return out;
}

int main(){
	using namespace ztop;
	using namespace std;
	AutoLibraryLoader::enable();
	TFile * f=new TFile("/nfs/dust/cms/user/kiesej/trees_Oct14/tree_7TeV_emuttbar.root");
	TTree * t = (TTree*) f->Get("PFTree/PFTree");


	tBranchHandler<vector<NTGenParticle> > b_GenTops(t,"NTGenTops");
	tBranchHandler<vector<NTGenParticle> > b_GenWs(t,"NTGenWs");
	tBranchHandler<vector<NTGenParticle> > b_Genbs(t,"NTGenBs");
	tBranchHandler<vector<NTGenParticle> > b_Genbrads(t,"NTGenBsRad");
	tBranchHandler<vector<NTGenParticle> > b_GenLeptons3(t,"NTGenLeptons3");
	tBranchHandler<vector<NTGenParticle> > b_GenLeptons1(t,"NTGenLeptons1");



	for(Long64_t entry=0;entry<5000;entry++){
		b_GenTops.getEntry(entry);
		b_GenWs.getEntry(entry);
		b_Genbs.getEntry(entry);
		b_Genbrads.getEntry(entry);
		b_GenLeptons3.getEntry(entry);


		std::vector<NTGenParticle*> tops=produceCollection(b_GenTops.content());
		std::vector<NTGenParticle*> bs=produceCollection(b_Genbs.content(),&tops);
		std::vector<NTGenParticle*> Ws=produceCollection(b_GenWs.content(),&tops);
		std::vector<NTGenParticle*> brads=produceCollection(b_Genbrads.content(),&bs);
		std::vector<NTGenParticle*> Leps=produceCollection(b_GenLeptons3.content(),&Ws);
		std::vector<NTGenParticle*> Leps1=produceCollection(b_GenLeptons1.content(),&Leps);

		bool coutchain=false;

		for(size_t i=0;i<bs.size();i++){
			if(bs.at(i)->daughters().size()>1){
				coutchain=true;
				//for(size_t j=0;j<bs.at(i)->daughters().size();j++){
					std::cout << "\nid of direct decay: " << pickDirectDecayB(bs.at(i)->daughters())->genId()
							<<std::endl;
				//}
			}
		}

		if(coutchain){
			for(size_t i=0;i<tops.size();i++){
				NTGenParticle * top=tops.at(i);
				std::cout << "Top: pdg " << top->pdgId() << " pt: " << top->pt() << " eta: " << top->eta() <<std::endl;
				for(size_t j=0;j<top->daughters().size();j++){
					NTGenParticle* dau=top->daughters().at(j);
					std::cout << "|--- " << dau->pdgId() << " pt " << dau->pt() << " eta " << dau->eta()
												<<" genid: " << dau->genId()<<" ndau: " << dau->daughters().size()<<std::endl;
					for(size_t k=0;k<dau->daughters().size();k++){
						NTGenParticle* daudau=dau->daughters().at(k);
						std::cout << "     |--- " << daudau->pdgId() << " pt " << daudau->pt() << " eta " << daudau->eta()
								<<" genid: " << daudau->genId()<<std::endl;
						for(size_t l=0;l<daudau->daughters().size();l++){
							NTGenParticle* daudaudau=daudau->daughters().at(l);
							std::cout << "          |--- " << daudaudau->pdgId() << " pt " << daudaudau->pt() << " eta " << daudaudau->eta()
													<<" genid: " << daudaudau->genId()<<std::endl;
						}
					}
				}
				std::cout <<std::endl;

			}
		}

	}


	return 0;
}
