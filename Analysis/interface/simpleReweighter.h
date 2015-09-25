/*
 * simpleReweighter.h
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */

#ifndef SIMPLEREWEIGHTER_H_
#define SIMPLEREWEIGHTER_H_


namespace ztop{

class simpleReweighter{
public:
    simpleReweighter();
    ~simpleReweighter(){}

    void setNewWeight(const float &w){tmpweight_=w;}

    void reWeight( float &oldweight);

    double getRenormalization()const;


    void switchOff(bool switchoff){switchedoff_=switchoff;}

    bool switchedOff(){return switchedoff_;}

private:
    float tmpweight_;
    double unwcounter_,wcounter_;
protected:
    bool switchedoff_;
};


}


#endif /* SIMPLEREWEIGHTER_H_ */
