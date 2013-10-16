/*
 * NTVertex.h
 *
 *  Created on: Aug 12, 2013
 *      Author: kiesej
 */

#ifndef NTVERTEX_H_
#define NTVERTEX_H_
#include "TVector3.h"

namespace ztop{



class NTVertex{
public:
	NTVertex(){}
	~NTVertex(){}

	void setPos(TVector3 v){pos_=v;}
	void setPosErr(TVector3 v){poserr_=v;}
	void setIdx(int idx){idx_=idx;}
	void setNDof(float ndf){ndof_=ndf;}
	void setChi2(double inchi2){chi2_=inchi2;}

	const TVector3 & position(){return pos_;}
	const TVector3 & positionErr(){return poserr_;}
	const float & ndof(){return ndof_;}
	const double & chi2(){return chi2_;}
	double normChi2(){return chi2_/(double)ndof_;}
	int id(){return idx_;}

private:
	TVector3 pos_;
	TVector3 poserr_;
	double chi2_;
	float ndof_;
	int idx_;

};



}


#endif /* NTVERTEX_H_ */
