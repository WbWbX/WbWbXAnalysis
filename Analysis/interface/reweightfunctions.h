/*
 * reweightfunctions.h
 *
 *  Created on: Dec 18, 2013
 *      Author: kiesej
 */

#ifndef REWEIGHTFUNCTIONS_H_
#define REWEIGHTFUNCTIONS_H_


namespace ztop{

class reweightfunctions{
public:
    reweightfunctions(): syst_(nominal),type_(toppt),unwcounter_(0),wcounter_(0),switchedoff_(false){}
    ~reweightfunctions(){}

    enum functiontype{toppt,flat};
    enum systematics{nominal,up,down};

    void setSystematics(systematics insys){syst_=insys;}
    void setFunction(functiontype func){type_=func;}

    void switchOff(bool switchoff){switchedoff_=switchoff;}

    /**
     * reweights the input weight.
     * Example use:
     * puweight = reWeight(someinputvar, puweight);
     * Do NOT do:
     * puweight *= reWeight(someinputvar, 1);
     * if you intent to use getRenormalization() at some point
     */
    float reWeight(const float& var, float previousweight) ;
    float reWeight(const float& var1,const float& var2 , float previousweight) ;

    float getRenormalization()const{if(wcounter_!=0)return unwcounter_/wcounter_; else return 1;}

private:

    systematics syst_;
    functiontype type_;
    float unwcounter_,wcounter_;
    bool switchedoff_;

};

}//namespace


#endif /* REWEIGHTFUNCTIONS_H_ */
