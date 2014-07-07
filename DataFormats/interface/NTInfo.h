/*
 * NTInfo.h
 *
 *  Created on: Jun 25, 2014
 *      Author: kiesej
 */

#ifndef NTINFO_H_
#define NTINFO_H_

#include "TString.h"
#include <map>

namespace ztop{

class NTInfo{
public:
    NTInfo(){}
    ~NTInfo(){}

    void addInfo(const TString & descr, const TString& infostr);
    void getInfo(const TString & descr);

    std::vector<const TString *> getAvailDescr()const;
    std::vector<const TString *> getAvailInfos()const;


private:
    std::map<TString,TString > info_;


};
}//ns

#endif /* NTINFO_H_ */
