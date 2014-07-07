/*
 * formatter.h
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */

#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <cmath>

namespace ztop{
/**
 * just a bunch of helpers
 */
class formatter{
    //use standard const..


public:

    template <class T>
    T round(T f,float pres)
    {
        return (T) (floor(f*(1.0f/pres) + 0.5)/(1.0f/pres));
    }


    template<class t>
    TString toTString(t in) {
        std::ostringstream s;
        s << in;
        TString out = s.str();
        return out;
    }

};

}


#endif /* FORMATTER_H_ */
