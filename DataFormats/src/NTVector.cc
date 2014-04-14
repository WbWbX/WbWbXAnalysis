/*
 * NTVector.cc
 *
 *  Created on: Apr 4, 2014
 *      Author: kiesej
 */


#include "../interface/NTVector.h"
namespace ztop{


NTVector& NTVector::operator *= (const float& val){
    setCoords(x_*val,y_*val,z_*val);
    return *this;
}
NTVector NTVector::operator *(const float& val)const{
    NTVector out=*this;
    out*=val;
    return out;
}


float NTVector::operator *(const NTVector & rhs)const{ //scalar product
    return x_*rhs.x_+y_*rhs.y_+z_*rhs.z_;
}
NTVector & NTVector::operator += (const NTVector & rhs){ //add
    setCoords(x_+rhs.x_,y_+rhs.y_,z_+rhs.z_);
    return *this;
}
NTVector NTVector::operator+(const NTVector & rhs)const{
    NTVector out=*this;
    out+=rhs;
    return out;
}

float NTVector::m(){
    return sqrt(x_*x_+y_*y_+z_*z_);
}

void NTVector::norm(){
    float mag=m();
    if(mag!=0)
        x_*=1/mag;y_*=1/mag;z_*=1/mag;
}

NTVector bisector(const NTVector & a, const NTVector & b){
    NTVector  acopy=a;
    acopy.norm();
    NTVector  bcopy=b;
    bcopy.norm();

    acopy+=bcopy;
    acopy.norm();
    return acopy;
}


}


