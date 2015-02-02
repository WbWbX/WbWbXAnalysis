/*
 * MCReweighter.h
 *
 *  Created on: Jan 30, 2015
 *      Author: agrohsje
 */
#ifndef MCREWEIGHTER_H_
#define MCREWEIGHTER_H_

#include "../interface/simpleReweighter.h"
#include <cstddef>
#include <iostream>

namespace ztop{

class MCReweighter: public simpleReweighter{
public:
        MCReweighter();
	~MCReweighter();
 
        void setMCWeight(float wght){mcWeight_=wght;}
        void reWeight(float& oldweight);
    
private:

	void setNewWeight(const float &w){simpleReweighter::setNewWeight(w);}
        
        float mcWeight_;
};


}



#endif /* MCREWEIGHTER_H_ */
