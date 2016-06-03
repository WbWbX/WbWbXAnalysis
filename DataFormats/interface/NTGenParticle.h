#ifndef NTGenParticle_h
#define NTGenParticle_h

#include <vector>
#include "mathdefs.h"
#include <map>
#include "NTLorentzVector.h"
#include <algorithm>

//debug
#include <iostream>

#define NTGENPARTICLE_MOTHERPDGID_MEMIDX 99
#define NTGENPARTICLE_GRANDMOTHERPDGID_MEMIDX 100

namespace ztop{

class NTGenParticle{
	friend class wNTGenParticleInterface;
public:
	explicit NTGenParticle():pdgid_(0),status_(9999),genid_(0),q_(0){}
	~NTGenParticle(){
		//	std::cout << "~NTGenParticle" <<std::endl;
		removeRelations();
		//	std::cout << "~NTGenParticle succ" <<std::endl;
	}

	void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
	void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
	void setPdgId(int id){pdgid_=id;}

	void setGenId(int genid){genid_=genid;};
	void setQ(int charge){q_=charge;}

	const int & genId()const{return genid_;}


	/**
	 * use this function to get proper relations!
	 * On analysis level! for reference, look at
	 * the helpers located in this sub-package (produceCollection)
	 */
	void relateWithMother( ztop::NTGenParticle  * p){
		p->addDaughter(this);
		addMother(p);
	}

	/**
	 * sets indices of associated entries in daughter collection (vector indices)
	 * Only use on tree-writing level (fixed indices!)
	 */
	void setDaughterIts(const std::vector<int> & Daughters){daughterits_=Daughters;}
	/**
	 * sets indices of associated entries in mother collection (vector indices)
	 * Only use on tree-writing level (fixed indices!)
	 */
	void setMotherIts  (const std::vector<int> & Mothers)  {motherits_=Mothers;}

	/**
	 * adds index of associated entry in mother collection (vector indices)
	 * Only use on tree-writing level (fixed indices!)
	 */
	void addMotherIt(int it){motherits_.push_back(it);}

	/**
	 * adds index of associated entry in daughter collection (vector indices)
	 * Only use on tree-writing level (fixed indices!)
	 */
	void addDaughterIt(int it){daughterits_.push_back(it);}

	void setStatus(int Status){status_=Status;}

	const int& pdgId()const{return pdgid_;}
	const int& status()const{return status_;}

	const NTLorentzVector<float> & p4(){return p4_;}
	float pt()const{return p4_.Pt();}
	float  E()const {return p4_.E();}
	float  e()const {return p4_.E();}
	float  phi()const{return p4_.Phi();}
	float  eta()const{return p4_.Eta();}
	float  m()const{return p4_.M();}
	const int& q()const{return q_;}


	/**
	 * Do not use those directly on analysis level. Produce relations with
	 * bare tree-input, and then use daughters() and mothers() instead!
	 * For reference, look at the helpers located in this sub-package (produceCollection)
	 */
	std::vector<int> & daughterIts(){return daughterits_;}
	/**
	 * Do not use those directly on analysis level. Produce relations with
	 * bare tree-input, and then use daughters() and mothers() instead!
	 * For reference, look at the helpers located in this sub-package (produceCollection)
	 */
	std::vector<int> & motherIts(){return motherits_;}

	/**
	 * access function for daughter particles
	 */
	const std::vector<NTGenParticle *> & daughters(){return daughterPs_;}
	/**
	 * access function for mother particles
	 */
	const std::vector<NTGenParticle *> & mothers(){return motherPs_;}

	//extra

	void setMember(std::string Membername, float value){
		memberss_[Membername]=value;
	}
	float getMember(std::string membername)const{
		if(memberss_.find(membername) != memberss_.end())
			return memberss_.find(membername)->second;
		else
			return -99999999999;
	}
	void setMember(int Memberidx, float value){
		members_[Memberidx]=value;
	}
	float getMember(int memberidx)const {
		if(members_.find(memberidx) != members_.end())
			return members_.find(memberidx)->second;
		else
			return -99999999999;
	}

	//extra W analysis
	void setMotherPdgID(int id){setMember((int)NTGENPARTICLE_MOTHERPDGID_MEMIDX,id);}
	int motherPdgID()const{return (int)getMember(NTGENPARTICLE_MOTHERPDGID_MEMIDX);}
	void setGrandMotherPdgID(int id){setMember((int)NTGENPARTICLE_GRANDMOTHERPDGID_MEMIDX,id);}
	int grandMotherPdgID()const{return (int)getMember(NTGENPARTICLE_GRANDMOTHERPDGID_MEMIDX);}


protected:

	void removeRelations(){
		for(size_t i=0;i<daughterPs_.size();i++){ //remove any references to this one!
			NTGenParticle * daug=daughterPs_.at(i);
			std::vector<NTGenParticle*>::iterator it=std::find(daug->motherPs_.begin(),daug->motherPs_.end(),this);
			if(it!=daug->motherPs_.end())
				daug->motherPs_.erase(it);
		}
		for(size_t i=0;i<motherPs_.size();i++){
			NTGenParticle * moth=motherPs_.at(i);
			std::vector<NTGenParticle*>::iterator it=std::find(moth->daughterPs_.begin(),moth->daughterPs_.end(),this);
			if(it!=moth->daughterPs_.end())
				moth->daughterPs_.erase(it);
		}
	}

	void addMother( ztop::NTGenParticle  * p){
		if(std::find(motherPs_.begin(),motherPs_.end(),p)==motherPs_.end())
			motherPs_.push_back(p);
	}
	void addDaughter( ztop::NTGenParticle * p){
		if(std::find(daughterPs_.begin(),daughterPs_.end(),p)==daughterPs_.end())
			daughterPs_.push_back(p);
	}
	void setMothers(const std::vector<ztop::NTGenParticle *> & p){motherPs_=p;}
	void setDaughters(const std::vector<ztop::NTGenParticle *> & p){daughterPs_=p;}


	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////

	int pdgid_, status_, genid_,q_;
	NTLorentzVector<float> p4_;
	std::vector<int> motherits_,daughterits_;
	std::vector<ztop::NTGenParticle *> daughterPs_,motherPs_;
	//std::vector<int> mothers_, daughters_;

	std::map<std::string, float> memberss_;
	std::map<int, float> members_;

};
}

#undef NTGENPARTICLE_MOTHERPDGID_MEMIDX
#undef NTGENPARTICLE_GRANDMOTHERPDGID_MEMIDX

#endif
