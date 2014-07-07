/*
 * legendStyle.cc
 *
 *  Created on: Jun 24, 2014
 *      Author: kiesej
 */



#include "../interface/legendStyle.h"
#include "TLegend.h"

#include "../interface/fileReader.h"

namespace ztop{


void legendStyle::readFromFile(const std::string & filename, const std::string& stylename, bool requireall){
    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    if(stylename.length()>0)
        fr.setStartMarker("[legendStyle - "+stylename+']');
    else
        fr.setStartMarker("[legendStyle]");

    fr.setEndMarker("[end legendStyle]");
    fr.readFile(filename);
    if(fr.nLines()<1 && requireall){
        std::cout << "legendStyle::readFromFile: did not find style "  << stylename <<std::endl;
        throw std::runtime_error("legendStyle::readFromFile: no legendStyle found");
    }
    fr.setRequireValues(requireall);

    x0_= fr.getValue<float>("x0",x0_);
    x1_= fr.getValue<float>("x1",x1_);
    y0_= fr.getValue<float>("y0",y0_);
    y1_= fr.getValue<float>("y1",y1_);
    ncolumns_= fr.getValue<int>("nColumns",ncolumns_);
    columnsep_= fr.getValue<float>("columnSeparator",columnsep_);
}


void legendStyle::applyLegendStyle(TLegend* leg)const{
    leg->SetX1(x0_);
    leg->SetX2(x1_);
    leg->SetY1(y0_);
    leg->SetY2(y1_);

    leg->SetNColumns(ncolumns_);
    leg->SetColumnSeparation(columnsep_);

}


}
