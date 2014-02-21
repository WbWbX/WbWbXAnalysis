/*
 * TtBarUnfolder.h
 *
 *  Created on: Feb 21, 2014
 *      Author: kiesej
 */

#ifndef TTBARUNFOLDER_H_
#define TTBARUNFOLDER_H_

#include "TString.h"
class TCanvas;

namespace ztop{

class TtBarUnfolder{
public:
    TtBarUnfolder():verbose(false){}
    ~TtBarUnfolder(){}

    TString unfold(TString out,TString in)const;

    bool verbose;

private:
    void setNameAndTitle(TCanvas *c, TString name)const;

};

}


#endif /* TTBARUNFOLDER_H_ */
