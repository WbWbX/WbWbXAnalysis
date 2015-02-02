/*
 * MCReweighter.cc
 *
 *  Created on: Jan 30, 2015
 *      Author: agrohsje
 */


#include "../interface/MCReweighter.h"
#include <stdexcept>

namespace ztop{

    MCReweighter::MCReweighter():simpleReweighter(),mcWeight_(1.){
	switchOff(false); //agrohsje true 
    
    }
    
    MCReweighter::~MCReweighter(){}
    
    void MCReweighter::reWeight(float &oldweight){
	
	if(switchedoff_) setNewWeight(1);
	else setNewWeight(mcWeight_);
	
	simpleReweighter::reWeight(oldweight);
    }
}



