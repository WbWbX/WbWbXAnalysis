/*
 * wNTGoodEventInterface.cc
 *
 *  Created on: 16 Jun 2016
 *      Author: jkiesele
 */


#include "../interface/wNTGoodEventInterface.h"

namespace ztop{

wNTGoodEventInterface::wNTGoodEventInterface(tTreeHandler * t, bool isMC):wNTBaseInterface(t,!isMC),pass_(false){
	addBranch<int>("checkEcalDead");
	addBranch<int>("checkhcalLaser");
	addBranch<int>("checktrackingFailure");
	addBranch<int>("checkprimaryVertex");
	addBranch<int>("checknoscraping");
	//addBranch<int>("checktrackIsolationMaker");
	addBranch<int>("checkmetNoiseCleaning");
	addBranch<int>("checkeeBadSc");
	//addBranch<int>("checkecalLaser");
	//addBranch<int>("checktotalKinematics");
	addBranch<int>("checkCSCTightHalo");
	addBranch<int>("checkHBHENoise");

	if(isMC) pass_=true;

	maxidx_=associatedBranches().size();
}
const bool& wNTGoodEventInterface::check(const size_t& idx){
	if(!pass_) return pass_;
	pass_ = * getBranchContent<int>(idx) >0;
	return pass_;
}
const bool& wNTGoodEventInterface::pass(){
	if(!enabled()) return pass_;//is true, never changes
	if(isNewEntry()){
		pass_=true;
		for(size_t i=0;i<maxidx_;i++)
			if(!check(i))break; //stop once first ok
	}
	return pass_;
}
}
