/*
 * pdfReweighter.h
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */

#ifndef PDFREWEIGHTER_H_
#define PDFREWEIGHTER_H_

#include "../interface/simpleReweighter.h"
#include <cstddef>

namespace ztop{

class NTEvent;

class pdfReweighter: public simpleReweighter{
public:
    pdfReweighter();
    ~pdfReweighter();

    void setPdfIndex(size_t idx){pdfidx_=idx;}
    void setNTEvent(NTEvent * evt){ntevent_=evt;}

    void reWeight( float &oldweight);

private:

    void setNewWeight(const float &w){simpleReweighter::setNewWeight(w);}

    NTEvent * ntevent_;
    size_t pdfidx_;

};


}



#endif /* PDFREWEIGHTER_H_ */
