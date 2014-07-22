/*
 * testmarkerStuff.cc
 *
 *  Created on: Jun 23, 2014
 *      Author: kiesej
 */

#include "../interface/fileReader.h"
#include <iostream>
#include "../interface/plotterControlPlot.h"

int main(){
    using namespace ztop;

    fileReader specialcplots;

//    fileReader::debug=true;
    specialcplots.setComment("$");
  //  specialcplots.setStartMarker("[additional plots]");
  //  specialcplots.setEndMarker("[end additional plots]");
    specialcplots.setDelimiter(",");
    specialcplots.readFile("/afs/desy.de/user/k/kiesej/xxl-af-cms/recProject/CMSSW_5_3_14/src/TtZAnalysis/Analysis/configs/mtExtractor_config.txt");

    //get plot names
    std::vector<std::string> plotnames= specialcplots.getMarkerValues("plot");

    for(size_t i=0;i<plotnames.size();i++){

        std::cout << plotnames.at(i) <<std::endl;;

    }
    std::string tmpfile=specialcplots.dumpFormattedToTmp();
    std::cout << specialcplots.dumpFormattedToTmp() <<std::endl;


    plotterControlPlot pl;
    pl.addStyleFromFile(tmpfile);


return 0;
}
