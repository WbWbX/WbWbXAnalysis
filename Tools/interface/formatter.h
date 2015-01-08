/*
 * formatter.h
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */

#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <cmath>
#include <map>
#include <string>
#include <sstream>

#include "TString.h"

namespace ztop{
/**
 * just a bunch of helpers
 */
class formatter{
    //use standard const..


public:
	formatter(): rootlatex_(false),invertrootlatex_(false){}

    template <class T>
    T round(T f,float pres)
    {
        return (T) (floor(f*(1.0f/pres) + 0.5)/(1.0f/pres));
    }

    void setRootLatex(bool doit){rootlatex_=doit;}
    template<class t>
    TString toTString(t in) {
        std::ostringstream s;
        s << in;
        TString out = s.str();
        return out;
    }

    //name formatting
    /**
     * reads in format file and ADDS information or overwrites existing one
     * Format:
     * Comments: #
     * Entries separated by commas
     * <techname>, <realname>, TBI
     */
    void readInNameTranslateFile(const std::string & pathtofile);
     TString translateName(const TString & techname)const;
    void clearTranslateRules(){cachedNames_.clear();}

    /**
     * makes "_" --> "\\_" if not in math mode ($ ... $)
     */
    TString makeTexCompatible(const TString& in)const;

    static bool debug;

private:
    std::map<TString,TString> cachedNames_;
    bool rootlatex_;
    bool invertrootlatex_;

};

}


#endif /* FORMATTER_H_ */
