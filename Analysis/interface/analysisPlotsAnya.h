/*
 * analysisPlotsAnya.h
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#ifndef ANALYSISPLOTSAnya_H_
#define ANALYSISPLOTSAnya_H_

#include "analysisPlots.h"

namespace ztop{

class analysisPlotsAnya : public analysisPlots{
public:
    analysisPlotsAnya(size_t step):analysisPlots(step),

    total(0){}

    ~analysisPlotsAnya(){}

    void bookPlots();
    void fillPlotsReco();
    void fillPlotsGen();

private:

    container1DUnfold

    *total;


};

}
#endif /* ANALYSISPLOTSAnya_H_ */
