/*
 * wNTGenParticleInterface.cc
 *
 *  Created on: 31 May 2016
 *      Author: jkiesele
 */


#include "../interface/wNTGenParticleInterface.h"

namespace ztop{

wNTGenParticleInterface::wNTGenParticleInterface(tTreeHandler * t,
		bool enable,
		const TString& size_branch,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& pdgid_branch,
		const TString& charge_branch,
		const TString& status_branch,
		const TString& motherpdgid_branch,
		const TString& grandmotherpdgid_branch) : wNTBaseInterface(t,enable),missingstatusbranch_(false){

	const size_t bufsize=128;
	addBranch<int>(size_branch);              //0
	addBranch<float*>(pt_branch,bufsize);     //1
	addBranch<float*>(eta_branch,bufsize);    //2
	addBranch<float*>(phi_branch,bufsize);    //3
	addBranch<float*>(m_branch,bufsize);      //4
	addBranch<int*>(pdgid_branch,bufsize);    //5
	addBranch<float*>(charge_branch,bufsize); //6
	tBranchHandler<int*>::allow_missing=true;
	tBranchHandler<int*> * b=addBranch<int*>(status_branch,bufsize); //7
	if(b)
		missingstatusbranch_=b->ismissing();
	tBranchHandler<int*>::allow_missing=false;
	addBranch<int*>(motherpdgid_branch,bufsize); //8
	addBranch<int*>(grandmotherpdgid_branch,bufsize); //9


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
			float charge= (* getBranchContent<float*>(6))[i] ;
			if(charge>0)
				part.setQ( 1 );
			else
				part.setQ( -1 );
			if(!missingstatusbranch_)
				part.setStatus( (* getBranchContent<int*>(7))[i] );
			else
				part.setStatus(3);
			part.setMotherPdgID(  (* getBranchContent<int*>(8))[i] );
			part.setGrandMotherPdgID(  (* getBranchContent<int*>(9))[i] );

		}
	}
	return &content_;
}

wNTGenParticleInterface::~wNTGenParticleInterface(){

}



}//ns
