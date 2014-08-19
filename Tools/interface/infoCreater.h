/*
 * infoCreater.h
 *
 *  Created on: Aug 6, 2014
 *      Author: kiesej
 */

#ifndef INFOCREATER_H_
#define INFOCREATER_H_

#include "TString.h"
#include "tObjectList.h"
class TCanvas;

namespace ztop{

/**
 * small class to write out info strings to files
 */
class infoCreater: tObjectList{
public:

	infoCreater():tObjectList(),makeroottex_(false){}


	/**
	 * creates info canvas with text
	 * text format is:
	 * \n denotes a line break, \t will be four spaces
	 * comments are not allowed
	 *
	 * cleanMem() will delete canvas and contents
	 */
	TCanvas * createInfoCanvas(const TString& text);

	void setMakeRootTex(bool set){makeroottex_=set;}

private:
	bool makeroottex_;

};

}



#endif /* INFOCREATER_H_ */
