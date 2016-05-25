#ifndef NTJet_h
#define NTJet_h

#include "mathdefs.h"
#include <map>
#include "NTLorentzVector.h"


namespace ztop{


class NTJet{
	friend class wNTJetInterface;
public:
	explicit NTJet():
	genid_(0),
	p4_(0,0,0,0),
	genP4_(0,0,0,0),
	genPartonFlav_(0),
	btag_(-1),
	emenergyfraction_(-1),
	id_(false),
	corrfact_(1)
	{}
	explicit NTJet(const NTLorentzVector<float>& p4in):genid_(0),
			p4_(p4in),
			genP4_(0,0,0,0),
			genPartonFlav_(0),
			btag_(-1),
			emenergyfraction_(-1),
			id_(false),
			corrfact_(1)
	{}
	~NTJet(){};
	//sets
	void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
	// void setP4(PolarLorentzVector p4In){p4_=p4In;}
	void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
	//  void setGenP4(PolarLorentzVector genP4In){genP4_=genP4In;}
	void setGenP4(NTLorentzVector<float> genP4In){genP4_=genP4In;}
	void setGenP4(D_LorentzVector p4In){genP4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
	void setGenPartonFlavour(int partonFlavour){genPartonFlav_=partonFlavour;}
	void setBtag(float Btag){btag_=Btag;}
	void setId(bool ID){id_=ID;}
	void setEmEnergyFraction(float EFrac){emenergyfraction_=EFrac;}
	void setCorrFactor(float fac){corrfact_=fac;}
	void setGenMatch(int id_int){genid_=id_int;}

	//gets
	const NTLorentzVector<float> &  p4(){return p4_;}
	NTLorentzVector<float> p4Uncorr(){return p4_ * (1/corrfact_);}
	int genMatch(){return genid_;}

	float pt(){return p4_.Pt();}
	float E() {return p4_.E();}
	float e() {return p4_.E();}
	float phi(){return p4_.Phi();}
	float eta(){return p4_.Eta();}
	float m(){return p4_.M();}
	float btag(){return btag_;}
	bool id(){return id_;}
	float emEnergyFraction(){return emenergyfraction_;}

	//gen
	const NTLorentzVector<float> & genP4(){return genP4_;}
	float genPt(){return genP4_.Pt();}
	float genE() {return genP4_.E();}
	float genPhi(){return genP4_.Phi();}
	float genEta(){return genP4_.Eta();}
	float genM(){return genP4_.M();}
	int genPartonFlavour(){return genPartonFlav_;}

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


	int genid_;
	NTLorentzVector<float> p4_;
	NTLorentzVector<float> genP4_;
	int genPartonFlav_;
	float btag_;
	float emenergyfraction_;
	bool id_;
	float corrfact_;

	std::map<std::string, float> memberss_;
	std::map<int, float> members_;

};
}
#endif
