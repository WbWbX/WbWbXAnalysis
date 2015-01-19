/*
 * NTSystWeight.h
 *
 *  Created on: Sep 18, 2014
 *      Author: kiesej
 */

#ifndef NTSYSTWEIGHT_H_
#define NTSYSTWEIGHT_H_


namespace ztop{
/**
 * just wrapper that assures a weight of 1 as default
 */
class NTWeight{
public:
	explicit NTWeight():weight_(1) {}
	explicit NTWeight(const float& weight):weight_(weight) {}

	void setWeight(const float & weight){weight_=weight;}

	const float & getWeight()const{return weight_;}

private:
	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////

	float weight_;
};
}


#endif /* NTSYSTWEIGHT_H_ */
