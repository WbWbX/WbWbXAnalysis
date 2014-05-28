/*
 * testInSituBTag.cc
 *
 *  Created on: Apr 17, 2014
 *      Author: kiesej
 */
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TString.h"
#include <iostream>

/*
 * skeleton program to test in situ b-tag calibrations etc.
 *
 * if you have set up your environment (ini cmssw; cmsenv; anenv),
 * you can easily compile and run this program in one go with the command
 *
 * fastCompile.sh testInSituBTag.cc
 *
 * (run in the same directory testInSituBTag.cc is located in)
 *
 *
 * Here, you will mostly deal with the containerStack and container1D classes
 * these containers are essentially histograms! Just the naming is (historically) different
 * Remember, they already incorporate systematic uncertainties etc.
 * For reference check the include files below
 *
 */
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TtZAnalysis/Tools/interface/container.h"

/*
 * the input file is located in /nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root
 *  you can check control plots etc. by opening this file with root
 *
*/
int main(){

    using namespace ztop;

    TString infilelocation="/nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root";
    TString btagmultiplot="selected b jet multi step 7"; //we use step 7 here, one ONE jet is required
    TString jetmultiplot="hard jet multi step 7";

    /*
     * prepare the input
     */

    containerStackVector csv_in;
    csv_in.loadFromTFile(infilelocation,"emu_8TeV_172.5_nominal_syst");

    containerStack bjetmultistack=csv_in.getStack(btagmultiplot);
    containerStack jetmultistack =csv_in.getStack(jetmultiplot);


    std::cout << bjetmultistack.listContributions() <<std::endl; //just such that you know what you can get here

    container1D background=bjetmultistack.getBackgroundContainer(); //this might come handy at some point
    container1D signal=bjetmultistack.getSignalContainer(); //this might come handy at some point
    container1D data=bjetmultistack.getContribution("data");
    container1D dyll=bjetmultistack.getContribution("DY#rightarrowll");



    //which systematics are available??
    for(size_t i=0;i<signal.getSystSize();i++){
        std::cout << signal.getSystErrorName(i) <<std::endl;
    }
    int somesystindex=0;
    //get a specific systematic:
    container1D onlysystemtics= signal.getSystContainer(somesystindex);
    container1D onlynominal=signal.getSystContainer(-1); // -1 is the index of nominal

    //get a bin
    size_t onejetbin=signal.getBinNo(1);
    float nominal1jetcontentsignal = signal.getBinContent(onejetbin);
    float somesyst1jetcontentsignal = signal.getBinContent(onejetbin,3); //here: sys layer 3
    std::cout << "\n" << signal.getSystErrorName(3) <<std::endl; //this one we just selected


    // if you want to look at ratios, you might consider using the built-in
    // operators that account for systematics
    // e.g. put all systematic variations, etc in one bin of a new container
    // and then just divide it by another one
    // please check the test programs here and the container.h headers
    // there is already a lot of built-in functionality


    std::cout << "DONE" <<std::endl;
}
