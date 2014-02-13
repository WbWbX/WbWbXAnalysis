/*
 * AnalysisUtils.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#include "../interface/AnalysisUtils.h"

namespace ztop{
void rescaleDY(ztop::container1DStackVector * vec, std::vector<TString> contributions, double scalescale, bool textout, TString identifier){

    using namespace ztop;

    std::vector<TString> ident;ident.clear();
    std::vector<double> scales;scales.clear();
    for(int i=5;i<=9;i++){
        TString stepstring=" step "+toTString(i);
        double dymc = 0;
        std::vector<TString> allbutdyanddata;

        for(unsigned int j=0;j<contributions.size();j++){
            dymc += vec->getStack(identifier+stepstring).getContribution(contributions.at(j)).integral();
            allbutdyanddata << contributions.at(j);
        }
        double d = vec->getStack(identifier+stepstring).getContribution("data").integral();
        allbutdyanddata << "data";
        double rest = vec->getStack(identifier+stepstring).getContributionsBut(allbutdyanddata).integral();
        if(rest==0) rest=1;
        double scale = (d-rest)/dymc;
        scales << scale*scalescale;
        ident << stepstring;
        if(textout) std::cout << "Scalefactor for "<< vec->getName() << " " << stepstring << ": " << scale << std::endl;
    }
    //  ztop::container1DStackVector rescaled=vec;
    for(unsigned int i=0;i<contributions.size();i++){
        vec->multiplyNorms(contributions.at(i), scales, ident);
    }

}




}
