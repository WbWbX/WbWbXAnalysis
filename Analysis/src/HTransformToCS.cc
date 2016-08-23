#include "../interface/HTransformToCS.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

//--------------------------------------------------------------------------------------------
HTransformToCS::HTransformToCS(const Double_t& energy,const Double_t& mass)
{

	using namespace std;
	HbeamPartMass=mass;
	SetCmsEnergy(energy);
}

//--------------------------------------------------------------------------------------------
void HTransformToCS::SetCmsEnergy(const Double_t& cms_energy)
{
	using namespace std;
	if(HcmsEnergy!=cms_energy) {
		HcmsEnergy = cms_energy;
		//	cout << "HTransformToCS::SetCmsEnergy: Set center of mass energy: " << HcmsEnergy << endl;
		//	cout << "HTransformToCS::SetCmsEnergy: Mass of beam particles:    " << HbeamPartMass << endl;

		PF_org.SetPxPyPzE(0.,0., sqrt(0.25*HcmsEnergy*HcmsEnergy-HbeamPartMass*HbeamPartMass),0.5*HcmsEnergy);
		PW_org.SetPxPyPzE(0.,0.,-sqrt(0.25*HcmsEnergy*HcmsEnergy-HbeamPartMass*HbeamPartMass),0.5*HcmsEnergy);
		/*
    PF_org.SetPxPyPzE(0.,0., 0.5*HcmsEnergy,0.5*HcmsEnergy);
    PW_org.SetPxPyPzE(0.,0.,-0.5*HcmsEnergy,0.5*HcmsEnergy);
		 */
	}
}
//ztop interface
void HTransformToCS::TransformToCS(ztop::NTGenParticle * neutrino,ztop::NTGenParticle * muon){
	TLorentzVector mu(muon->p4().px(),
			muon->p4().py(),
			muon->p4().pz(),
			muon->p4().e());
	TLorentzVector neu(neutrino->p4().px(),
			neutrino->p4().py(),
			neutrino->p4().pz(),
			neutrino->p4().e());
	//next try:
	// SetPtEtaPhiM


	TransformToCS(neu,mu,muon->q());
}

//--------------------------------------------------------------------------------------------
void HTransformToCS::TransformToCS(TLorentzVector neutr,TLorentzVector muon,const int& mucharge)
{
	using namespace std;
	float muforward= muon.Eta()*mucharge;

	TLorentzVector & muplus=muon;
	TLorentzVector & muminus=neutr;
	/*
	 *
	 */
	Wv= muplus+muminus;// this is the Z boson 4vector
	b = Wv.BoostVector();
	muplus.Boost(-b);
	muminus.Boost(-b);

	PF = PF_org;
	PW = PW_org;

	PF.Boost(-b);
	PW.Boost(-b);
	PFMinus= true;
	// choose what to call proton and what anti-proton
	//if(Wv.Angle(PF.Vect())<Wv.Angle(PW.Vect()))
	if(muforward>0)
	{
		PW= -PW;
	}
	else
	{
		PF= -PF;
		PFMinus=false;
	}
	PF=PF*(1.0/PF.Vect().Mag());
	PW=PW*(1.0/PW.Vect().Mag());

	// Bisector is the new Z axis
	PBiSec =PW+PF;
	PhiSecZ =  PBiSec.Vect().Unit();

	PhiSecY = (PhiSecZ.Cross(Wv.Vect().Unit())).Unit();

	muminusVec = muminus.Vect();
	TRotation roataeMe;

	// build matrix for transformation into CS frame
	roataeMe.RotateAxes(PhiSecY.Cross(PhiSecZ),PhiSecY,PhiSecZ);
	roataeMe.Invert();
	// tranfor into CS alos the "debugging" vectors
	muminusVec.Transform(roataeMe);

	theta_cs = muminusVec.Theta();
	cos_theta_cs = TMath::Cos(theta_cs);
	phi_cs = muminusVec.Phi();

	return;



	Wv= neutr+muon;// this is the Z boson 4vector
	b = Wv.BoostVector();
	neutr.Boost(-b);
	muon.Boost(-b);

	PF = PF_org;
	PW = PW_org;

	PF.Boost(-b);
	PW.Boost(-b);


	const bool jansimpl=false;
	/*
	 * new impl (Jan)
	 */
	//z axis
	TVector3 p1v=PF.Vect().Unit();
	TVector3 p2v=PW.Vect().Unit();
	TVector3 zaxis;
	TVector3 muonvec=muon.Vect();
	if(muforward>0)
		zaxis=(p1v-p2v).Unit();
	else
		zaxis=(p2v-p1v).Unit();

	TVector3 xaxis=-(p1v+p2v);
	TVector3 yaxis;
	if( xaxis.Mag() < 1e-15){ //happens with about 0 boost, runs out of numerical limits, but might as well use the standard coordinate system
		yaxis.SetXYZ(0,1,0);
		xaxis=yaxis.Cross(zaxis);
	}else{
		xaxis=xaxis.Unit();
		yaxis=zaxis.Cross(xaxis);
	}

	//not needed

	TRotation csrot;
	csrot.RotateAxes(xaxis,yaxis,zaxis);
	csrot.Invert();
	muonvec.Transform(csrot);


	if(jansimpl){
		cos_theta_cs=muonvec.CosTheta();
		phi_cs=muonvec.Phi();
		return;
	}


	/*
	 * end new impl (Jan)
	 */

	PFMinus= true;
	// choose what to call proton and what anti-proton
	//if(Wv.Angle(PF.Vect())<Wv.Angle(PW.Vect()))
	if(muforward>0)
	{
		PW= -PW;
	}
	else
	{
		PF= -PF;
		PFMinus=false;
	}
	PF=PF*(1.0/PF.Vect().Mag());
	PW=PW*(1.0/PW.Vect().Mag());

	// Bisector is the new Z axis
	PBiSec =PW+PF;
	PhiSecZ =  PBiSec.Vect().Unit();

	PhiSecY = (PhiSecZ.Cross(Wv.Vect().Unit())).Unit();

	muminusVec = muon.Vect();
	TRotation roataeMe;
	TVector3 PhiSecX=PhiSecY.Cross(PhiSecZ);

	// build matrix for transformation into CS frame
	roataeMe.RotateAxes(PhiSecX,PhiSecY,PhiSecZ);
	roataeMe.Invert();
	// tranfor into CS alos the "debugging" vectors
	muminusVec.Transform(roataeMe);

	theta_cs = muminusVec.Theta();


	cos_theta_cs = TMath::Cos(theta_cs);
	phi_cs = muminusVec.Phi();


}
