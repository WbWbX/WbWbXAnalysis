/*
 * testStatStuff.cc
 *
 *  Created on: Jul 17, 2014
 *      Author: kiesej
 */

#include "../interface/container.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TRandom.h"
#include <vector>
#include "TCanvas.h"
#include <cmath>
#include "../interface/plotterMultiplePlots.h"

int main(){

    using namespace ztop;

    float bindiv=0.01;

    std::vector<float> bins;
    for(float i=0;i<=2000;i+=bindiv) bins<<i;

    container1D c1(bins),c2(bins),c3(bins),c4(bins);
  //  c3.setMergeUnderFlowOverFlow(true);
  //  c4.setMergeUnderFlowOverFlow(true);



//for(int j=0;j<100;j++)
    for(float i=bindiv;i<2000;i+=bindiv){
      /*  c1.fill(i,exp(shiftedLnPoisson(50,10,i)));
        c2.fill(i,exp(shiftedLnPoisson(50,100,i)));
        c3.fill(i,exp(shiftedLnPoisson(50,10000,i)));
        c4.fill(i,shiftedLnPoisson(50,1000000,i));
*/
        c1.fill(i,exp(shiftedLnPoisson(10,1,i)));
        c2.fill(i,exp(shiftedLnPoissonMCStat(1900,10,0.5,i)));
      //  c3.fill(i,exp(shiftedLnPoissonMCStat(7,7,20,i)));
        c3.fill(i,exp(shiftedLnPoissonMCStat(100,10,0.5,i)));
        c4.fill(i,exp(shiftedLnPoissonMCStat(5,2,0.20,i)));

    }


    TCanvas cv1;
    c1.setAllErrorsZero();
    c2.setAllErrorsZero();
    c3.setAllErrorsZero();
    c4.setAllErrorsZero();

    std::cout << "1: " << c1.integral(true)*bindiv <<std::endl;
    std::cout << "2: " << c2.integral(true)*bindiv <<std::endl;
    std::cout << "3: " << c3.integral(true)*bindiv <<std::endl;
    std::cout << "4: " << c4.integral(true)*bindiv <<std::endl;
    plotterMultiplePlots::debug=true;
    plotterMultiplePlots plotter;
    plotter.readStyleFromFileInCMSSW("src/TtZAnalysis/Tools/styles/multiplePlots.txt");
    plotter.usePad(&cv1);

    plotter.addPlot(&c1,false);
    plotter.addPlot(&c2,false);
    plotter.addPlot(&c3,false);
    plotter.addPlot(&c4,false);

    plotter.draw();

    cv1.Print("testStatStuff.pdf");

    return 0;
}
