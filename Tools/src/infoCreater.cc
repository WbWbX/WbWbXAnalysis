/*
 * infoCreater.cc
 *
 *  Created on: Aug 6, 2014
 *      Author: kiesej
 */

#include "../interface/infoCreater.h"
#include "TCanvas.h"
#include "../interface/textFormatter.h"
#include "TPavesText.h"

namespace ztop{


TCanvas * infoCreater::createInfoCanvas(const TString& text){

	TCanvas * c=addObject(new TCanvas());
	textFormatter formattext;
	formattext.setDelimiter("\n");
	formattext.setComment("");
	formattext.setTrim(""); //only get lines


	std::vector<std::string> confstrings=formattext.getFormatted(text.Data());
	TPavesText * configurationtext=addObject( new TPavesText(0.1,0.1,0.9,0.9));
	for(size_t i=0;i<confstrings.size();i++){
		TString tmptext=confstrings.at(i);
		tmptext.ReplaceAll("\t","    ");
		if(makeroottex_){
			tmptext.ReplaceAll("$"," ");
			tmptext.ReplaceAll("\\","#");
		}
		configurationtext->AddText(tmptext);
	}
	configurationtext->Draw();

	return c;
}


}
