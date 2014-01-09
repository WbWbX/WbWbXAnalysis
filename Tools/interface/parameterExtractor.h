/*
 * parameterExtractor.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef PARAMETEREXTRACTOR_H_
#define PARAMETEREXTRACTOR_H_

#include "graph.h"
#include "tObjectList.h"
#include "TString.h"
#include <vector>
class TCanvas;

namespace ztop{

class parameterExtractor : public tObjectList{
public:
    parameterExtractor(): paraname_(""),aname_(""),bname_(""),acolor_(0),bcolor_(632){}
    ~parameterExtractor(){}

    // defaults should suffice

    // parameterExtractor(const parameterExtractor&);
    // parameterExtractor & operator = (const parameterExtractor&);

    void setParaName(const TString&);

    void setInputA(const std::vector<graph>&); //graphs already come with para information; graph per bin
    void setInputB(const std::vector<graph>&);

    void setInputA(const graph &); //for one bin (just internally clears and pushes back)
    void setInputB(const graph &);

    void setNameA(const TString&);
    void setNameB(const TString&);

    void setColorA(int);
    void setColorB(int);

    bool checkSizes();

    ////

    size_t getSize();

    void plotAllInput(TCanvas * ); //plots in canvas with legend etc, keeps track of created graphs


    ////



protected:
    /* */


private:
    TString paraname_;
    std::vector<graph> agraphs_,bgraphs_;
    std::vector<float> avals_,bvals_;
    TString aname_,bname_;
    int acolor_,bcolor_;
};
}//ns



#endif /* PARAMETEREXTRACTOR_H_ */
