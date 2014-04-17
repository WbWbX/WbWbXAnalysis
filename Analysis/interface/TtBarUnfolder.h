/*
 * TtBarUnfolder.h
 *
 *  Created on: Feb 21, 2014
 *      Author: kiesej
 */

#ifndef TTBARUNFOLDER_H_
#define TTBARUNFOLDER_H_

#include "TString.h"
#include <vector>
class TCanvas;

namespace ztop{

class TtBarUnfolder{
public:
    TtBarUnfolder():verbose(false),printpdfs(false),brcorr_(-1){}
    ~TtBarUnfolder(){}
    void setBRCorr(const float& br){brcorr_=br;}
    void addToBeCorrectedPlotID(const TString & pl){plotids_.push_back(pl);}
    void clearToBeCorrectedPlotIDs(){plotids_.clear();}
    void setUnits(const TString & un){units_=un;}

    TString unfold(TString out,TString in)const;

    bool verbose;
    bool printpdfs;

private:
    void setNameAndTitle(TCanvas *c, TString name)const;
    float brcorr_;
    std::vector<TString> plotids_;
    TString units_;
};

}


#endif /* TTBARUNFOLDER_H_ */
