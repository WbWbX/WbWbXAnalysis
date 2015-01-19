/*
 * NTVector.h
 *
 *  Created on: Apr 4, 2014
 *      Author: kiesej
 */

#ifndef NTVECTOR_H_
#define NTVECTOR_H_
#include <cmath>

namespace ztop{
/**
 * Vector class with standard carth coordinates
 */

class NTVector{

public:
    NTVector():x_(0),y_(0),z_(0){}
    NTVector(const float& x,const float& y,const float& z):x_(x),y_(y),z_(z){}
    ~NTVector(){}

    const float& x()const{return x_;}
    const float& y()const{return y_;}
    const float& z()const{return z_;}

    void setX(const float& in){x_=in;}
    void setY(const float& in){y_=in;}
    void setZ(const float& in){z_=in;}

    void setCoords(const float& inx,const float& iny,const float& inz){
        x_=inx;y_=iny;z_=inz;
    }

    NTVector & operator *= (const float& val);
    NTVector operator *(const float& val)const;

    float operator *(const NTVector & rhs)const;
    NTVector & operator += (const NTVector & rhs);
    NTVector operator+(const NTVector & rhs)const;

    float m();

    void norm();

private:
	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////

    float x_, y_, z_;

};



//functions


NTVector bisector(const NTVector & a, const NTVector & b);


}

#endif /* NTVECTOR_H_ */
