/*
 * wNTGenParticleInterface.cc
 *
 *  Created on: 31 May 2016
 *      Author: jkiesele
 */


#include "../interface/wNTGenParticleInterface.h"

namespace ztop{

wNTGenParticleInterface::wNTGenParticleInterface(tTreeHandler * t,
		const TString& size_branch,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& pdgid_branch,
		const TString& charge_branch,
		const TString& status_branch,
		const TString& motherpdgid_branch,
		const TString& grandmotherpdgid_branch) : wNTBaseInterface(t){

	const size_t bufsize=128;
	addBranch<int>(size_branch);
	addBranch<float*>(pt_branch,bufsize);
	addBranch<float*>(eta_branch,bufsize);
	addBranch<float*>(phi_branch,bufsize);
	addBranch<float*>(m_branch,bufsize);
	addBranch<int*>(pdgid_branch,bufsize);
	addBranch<float*>(charge_branch,bufsize);
	addBranch<int*>(status_branch,bufsize);
	addBranch<int*>(motherpdgid_branch,bufsize);
	addBranch<int*>(grandmotherpdgid_branch,bufsize);


}

std::vector<NTGenParticle> * wNTGenParticleInterface::content(){
	if(isNewEntry()){
		size_t npart=*getBranchContent<int>(0);
		if(npart>getBranchBuffer(1))npart=getBranchBuffer(1);//just check
		content_.resize(npart);
		for(size_t i=0;i<npart;i++){
			NTGenParticle & part=content_.at(i);

			part.p4_.setPt( (* getBranchContent<float*>(1))[i]);
			part.p4_.setEta((* getBranchContent<float*>(2))[i]);
			part.p4_.setPhi((* getBranchContent<float*>(3))[i]);
			part.p4_.setM(  (* getBranchContent<float*>(4))[i]);

			part.setPdgId(  (* getBranchContent<int*>(5))[i] );
			part.setQ(      (* getBranchContent<float*>(6))[i] );
			part.setStatus( (* getBranchContent<int*>(7))[i] );
			part.setMotherPdgID(  (* getBranchContent<int*>(8))[i] );
			part.setGrandMotherPdgID(  (* getBranchContent<int*>(9))[i] );

		}
	}
	return &content_;
}

wNTGenParticleInterface::~wNTGenParticleInterface(){

}



}//ns
