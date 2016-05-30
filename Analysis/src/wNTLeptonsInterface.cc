/*
 * wNTLeptonsInterface.cc
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */



#include "../interface/wNTLeptonsInterface.h"
namespace ztop{
wNTLeptonsInterface::wNTLeptonsInterface(tTreeHandler * t,
		const TString& size_branch,
		const TString& pdgid_branch,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& charge_branch,
		const TString& tightID_branch,
		const TString& relIso04_branch): wNTBaseInterface(t){

	addBranch<int>(size_branch); //0
	addBranch<int*>(pdgid_branch);
	addBranch<float*>(pt_branch);
	addBranch<float*>(eta_branch);
	addBranch<float*>(phi_branch);
	addBranch<float*>(m_branch);
	addBranch<int*>(charge_branch);
	addBranch<int*>(tightID_branch);
	addBranch<float*>(relIso04_branch);


}

void wNTLeptonsInterface::readContents(){
	if(isNewEntry()){
		//read here, do nothing else
		size_t nleps= *getBranchContent<int>(0);
		if(nleps>getBranchBuffer(1)) nleps=getBranchBuffer(1);
		mu_content_.clear();
		e_content_.clear();
		l_content_.clear();
		for(size_t i=0;i<nleps;i++){
			int pdgid= (*getBranchContent<int*>(1))[i];
			if(pdgid==11 ||  pdgid==-11){
				NTElectron el;

			}
			else if(pdgid==13 ||  pdgid==-13){
				NTMuon mu;
				mu.p4_.setPt((* getBranchContent<float*> (2))[i]);
				mu.p4_.setEta((* getBranchContent<float*>(3))[i]);
				mu.p4_.setPhi((* getBranchContent<float*>(4))[i]);
				mu.p4_.setM((* getBranchContent<float*>  (5))[i]);

				mu.q_= (* getBranchContent<int*>       (6))[i];

				mu.setIsTightID( (* getBranchContent<int*>(7))[i]>0);;

				//just set one value for iso
				mu.iso_.setNeutralHadronIso(0);
				mu.iso_.setPuChargedHadronIso(0);
				mu.iso_.setPhotonIso(0);
				mu.iso_.setChargedHadronIso((* getBranchContent<float*>  (8))[i] * mu.pt());

				mu_content_.push_back(mu);
			}
		}
		for(size_t i=0;i<mu_content_.size();i++)
			l_content_.push_back(&mu_content_.at(i));
		for(size_t i=0;i<e_content_.size();i++)
			l_content_.push_back(&e_content_.at(i));
	}
}


std::vector<NTMuon>     * wNTLeptonsInterface::mu_content(){
	readContents();
	return &mu_content_;
}
std::vector<NTElectron> * wNTLeptonsInterface::e_content(){
	readContents();
	return &e_content_;
}
std::vector<NTLepton*>  * wNTLeptonsInterface::l_content(){
	readContents();
	return &l_content_;
}

}//ns
