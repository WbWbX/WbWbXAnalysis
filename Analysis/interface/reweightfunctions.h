/*
 * reweightfunctions.h
 *
 *  Created on: Dec 18, 2013
 *      Author: kiesej
 */

#ifndef REWEIGHTFUNCTIONS_H_
#define REWEIGHTFUNCTIONS_H_
#include "simpleReweighter.h"

namespace ztop{

class reweightfunctions: public simpleReweighter{
public:
    reweightfunctions():simpleReweighter(), syst_(nominal),type_(toppt){}
    ~reweightfunctions(){}

    enum functiontype{toppt,flat};
    enum systematics{nominal,up,down};

    void setSystematics(systematics insys){syst_=insys;}
    void setFunction(functiontype func);
    systematics getSystematic(){return syst_;}

    /**
     * reweights the input weight.
     * Example use:
     * reWeight(someinputvar, puweight);
     */
    void reWeight(const float& var, float& previousweight) ;
    void reWeight(const float& var1,const float& var2 , float& previousweight) ;


private:

    void setNewWeight(const float &w){simpleReweighter::setNewWeight(w);}
    void reWeight( float &oldweight){simpleReweighter::reWeight(oldweight);}


    systematics syst_;
    functiontype type_;

};

}//namespace


#endif /* REWEIGHTFUNCTIONS_H_ */
