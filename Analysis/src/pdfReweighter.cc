/*
 * pdfReweighter.cc
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */


#include "../interface/pdfReweighter.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include <stdexcept>

namespace ztop{

pdfReweighter::pdfReweighter():simpleReweighter(),ntevent_(0),pdfidx_(0),rewtonomev_(false){
	switchOff(true);
}

pdfReweighter::~pdfReweighter(){}

void pdfReweighter::reWeight( float &oldweight){

    if(switchedoff_){
        setNewWeight(1);
    }
    else{
        if(ntevent_->PDFWeightsSize()<=pdfidx_){
            throw std::out_of_range("pdfReweighter::reWeight: pdf index out of range");
        }
        float newweight = ntevent_->PDFWeight(pdfidx_);
        if(rewtonomev_){
        	newweight /= ntevent_->PDFWeight(0);
        }

        setNewWeight(newweight);
    }
    simpleReweighter::reWeight(oldweight);
}

}



