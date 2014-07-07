/*
 * legendStyle.h
 *
 *  Created on: Feb 26, 2014
 *      Author: kiesej
 */

#ifndef LEGENDSTYLE_H_
#define LEGENDSTYLE_H_

#include <string>

class TLegend;
namespace ztop{
/**
 * empty class for now
 */
class legendStyle{
public:
    legendStyle():x0_(0.65),y0_(0.50),x1_(0.95),y1_(0.90),ncolumns_(1),columnsep_(.1){}
    ~legendStyle(){}



    void readFromFile(const std::string & filename, const std::string& stylename="", bool requireall=true);


    void applyLegendStyle(TLegend* leg)const;
private:
    float x0_,y0_,x1_,y1_;
    int ncolumns_;
    float columnsep_;

};

}



#endif /* LEGENDSTYLE_H_ */
