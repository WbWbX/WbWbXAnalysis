/*
 * systAdder.h
 *
 *  Created on: Sep 3, 2014
 *      Author: kiesej
 */

#ifndef SYSTADDER_H_
#define SYSTADDER_H_

#include <vector>
#include <string>
#include "TString.h"

namespace ztop{


/**
 * This class is meant as a base class for all inheriting classes that need
 * to be able to extract information from a file that provides information how to add systematics
 * to groups
 *
 * There are no elaborate specific error checks
 */
class systAdder{

public:

	/**
	 * expects the following format:
	 * <mergedname> <mergeoption> <contribution1> <contribution2> ....
	 * has to be in section[merge syst] -> [end-merge syst]
	 * Comments: #
	 */
	void readMergeVariationsFileInCMSSW(const std::string&);
	/**
	 * expects the following format:
	 * <mergedname> <mergeoption> <contribution1> <contribution2> ....
	 * has to be in section[merge syst] -> [end-merge syst]
	 *  Comments: %
	 *
	 *  entries can refer to each other, e.g.
	 *
	 *  BLA   squared  subBla1 subBla2
	 *  BLU   linear  subBLU1 subBLU2 subBLU3
	 *
	 *  ALL   squared BLA BLU
	 *
	 */
	void readMergeVariationsFile(const std::string&);

	size_t mergeVariationsSize(){return tomerge.size();}

	TString              getMergedName(size_t idx)const;
	std::vector<TString> getToBeMergedName(size_t idx)const;

	bool                 getToBeMergedLinearly(size_t idx)const;


	static bool debug;

private:

	class systMergeDescription {
	public:
		systMergeDescription():linearly(false){}

		TString mergename;
		std::vector<TString> tobemerged;
		bool linearly;
	};

	std::vector<systMergeDescription> tomerge;


};

}
#endif /* SYSTADDER_H_ */
