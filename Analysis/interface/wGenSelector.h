/*
 * wGenSelector.h
 *
 *  Created on: 15 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WGENSELECTOR_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WGENSELECTOR_H_

#include <vector>

namespace ztop{
class NTGenParticle;

class wGenSelector{
public:
	wGenSelector():coll_(0){}
	~wGenSelector(){}
	//=

	void addVeto(int pdgid);
	void setCollection(const std::vector<NTGenParticle *> * coll){coll_=coll;}
	bool pass()const;

	static bool debug;

private:
	const std::vector<NTGenParticle *> * coll_;
	std::vector<int> vetos_;
};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WGENSELECTOR_H_ */
