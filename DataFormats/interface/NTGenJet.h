#ifndef NTGenJet_h
#define NTGenJet_h
#include "mathdefs.h"
#include "NTLorentzVector.h"
#include <map>
#include <vector>

namespace ztop{

class NTGenParticle;

class NTGenJet{
	friend class wNTGenJetInterface;
public:
	explicit NTGenJet(){}
	~NTGenJet(){}


	//  void setP4(PolarLorentzVector p4In){p4_=p4In;}
	void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
	void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}

	void setMotherIts(const std::vector<int> & its){motherits_=its;}
	void setMothers(const std::vector<ztop::NTGenParticle *> & p){motherPs_=p;}
	void addMotherIt(int it){motherits_.push_back(it);}
	void addMother(ztop::NTGenParticle * it){motherPs_.push_back(it);}

	void setGenId(int genid){genid_=genid;}

	const int& genId()const{return genid_;}
	const std::vector<int> &  motherIts()const{return motherits_;}
	const std::vector<ztop::NTGenParticle *> & mothers()const{return motherPs_;}

	const NTLorentzVector<float>  & p4()const{return p4_;}
	const float& pt()const{return p4_.Pt();}
	float E()const {return p4_.e();}
	float e()const {return p4_.e();}
	const float& phi()const{return p4_.Phi();}
	const float& eta()const{return p4_.Eta();}
	const float& m()const{return p4_.M();}

	//extra

	void setMember(std::string Membername, float value){
		memberss_[Membername]=value;
	}
	float getMember(std::string membername){
		if(memberss_.find(membername) != memberss_.end())
			return memberss_.find(membername)->second;
		else
			return -99999999999;
	}
	void setMember(int Memberidx, float value){
		members_[Memberidx]=value;
	}
	float getMember(int memberidx){
		if(members_.find(memberidx) != members_.end())
			return members_.find(memberidx)->second;
		else
			return -99999999999;
	}


protected:

	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////


	NTLorentzVector<float>  p4_;
	std::vector<int> motherits_;
	std::vector<ztop::NTGenParticle *> motherPs_;
	int genid_;

	std::map<std::string, float> memberss_;
	std::map<int, float> members_;

};
}
#endif
