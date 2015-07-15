/*
 * ZControlPlots.h
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#ifndef ZControlPlots_H_
#define ZControlPlots_H_

#include "controlPlotBasket.h"
#include "TtZAnalysis/DataFormats/interface/NTLepton.h"

namespace ztop{

class ZControlPlots : public controlPlotBasket{
public:
	ZControlPlots():controlPlotBasket(){}
	~ZControlPlots(){}

	void makeControlPlots(const size_t& step);

	enum etapten{
		pt2030_eta05   =0,
		pt2030_eta0515 =1,
		pt2030_eta1521 =2,
		pt2030_eta2124 =3,

		pt3040_eta05   =4,
		pt3040_eta0515 =5,
		pt3040_eta1521 =6,
		pt3040_eta2124 =7,

		pt4060_eta05   =8,
		pt4060_eta0515 =9,
		pt4060_eta1521 =10,
		pt4060_eta2124 =11,

		pt60100_eta05   =12,
		pt60100_eta0515 =13,
		pt60100_eta1521 =14,
		pt60100_eta2124 =15,

		pt100inf_eta05   =16,
		pt100inf_eta0515 =17,
		pt100inf_eta1521 =18,
		pt100inf_eta2124 =19,


		invalid =20

	} ptetacategory_;

	void setCategory(const NTLepton* l1,const NTLepton* l2);

	bool categoryCutsFullfilled(const NTLepton* l1, const NTLepton* l2, const float& ptcutlow, const float & ptcuth,const float& etacutlow, const float & etacuth)const;


};
}//namespace

#endif /* ZControlPlots_H_ */
