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

    float getRenormalization()const;


    void switchOff(bool switchoff){switchedoff_=switchoff;}

private:
    float tmpweight_;
    float unwcounter_,wcounter_;
protected:
    bool switchedoff_;
};


}


#endif /* SIMPLEREWEIGHTER_H_ */
