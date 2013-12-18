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
    reweightfunctions(): syst_(nominal),type_(toppt){}
    ~reweightfunctions(){}

    enum functiontype{toppt};
    enum systematics{nominal,up,down};

    void setSystematics(systematics insys){syst_=insys;}
    void setFunction(functiontype func){type_=func;}

    float getWeight(const float&) const;
    float getWeight(const float&,const float&) const;


private:

    systematics syst_;
    functiontype type_;

};

}//namespace


#endif /* REWEIGHTFUNCTIONS_H_ */
