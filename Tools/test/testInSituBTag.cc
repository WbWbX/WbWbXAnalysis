/*
 * testInSituBTag.cc
 *
 *  Created on: Apr 17, 2014
 *      Author: kiesej
 */
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
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
 * Here, you will mostly deal with the histoStack and histo1D classes
 * these containers are essentially histograms! Just the naming is (historically) different
 * Remember, they already incorporate systematic uncertainties etc.
 * For reference check the include files below
 *
 */
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/histo1D.h"

/*
 * the input file is located in /nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root
 *  you can check control plots etc. by opening this file with root
 *
*/
int main(){

    using namespace ztop;

   // TString infilelocation="/nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root";
    TString infilelocation="/nfs/dust/cms/user/dolinska/inclusive/CMSSW_5_3_14/src/TtZAnalysis/Analysis/output/emu_8TeV_172.5_nominal_pseudodata230.root";
    TString btagmultiplot="selected b jet multi step 4"; //we use step 7 here, one ONE jet is required
    TString jetmultiplot="hard jet multi step 4";

    /*
     * prepare the input
     */

    histoStackVector csv_in;
    csv_in.loadFromTFile(infilelocation,"emu_8TeV_172.5_nominal_pseudodata230");//_syst");

    histoStack bjetmultistack=csv_in.getStack(btagmultiplot);
    histoStack jetmultistack =csv_in.getStack(jetmultiplot);


    std::cout << bjetmultistack.listContributions() <<std::endl; //just such that you know what you can get here

    histo1D background=bjetmultistack.getBackgroundContainer(); //this might come handy at some point
    histo1D signal=bjetmultistack.getSignalContainer(); //this might come handy at some point
    histo1D data=bjetmultistack.getContribution("data");
    histo1D dyll=bjetmultistack.getContribution("DY#rightarrowll");

    // -----> Epsilon_emu
    histoStack genstack = csv_in.getStack("generated events");
    histo1D signalgen = genstack.getSignalContainer();
    size_t genevtbin=signalgen.getBinNo(1);
    float genttbarevents=signalgen.getBinContent(genevtbin);

    float eps_emu=signal.integral(true)/genttbarevents; 
    //-------> end epsilon_emu


    //which systematics are available??
    for(size_t i=0;i<signal.getSystSize();i++){
        std::cout << signal.getSystErrorName(i) <<std::endl;
    }
    int somesystindex=0;
    //get a specific systematic:
    histo1D onlysystemtics= signal.getSystContainer(somesystindex);
    histo1D onlynominal=signal.getSystContainer(-1); // -1 is the index of nominal

    // ------> get a bin
    size_t onejetbin=signal.getBinNo(1);
    size_t twojetbin=signal.getBinNo(2);
    float N1_tt = signal.getBinContent(onejetbin);
    float N2_tt = signal.getBinContent(twojetbin);

    size_t onejetbinBg=background.getBinNo(1);
    size_t twojetbinBg=background.getBinNo(2);
    float nominal1jetcontentbackground = background.getBinContent(onejetbin);
    float nominal2jetcontentbackground = background.getBinContent(twojetbin);

    size_t onejetbinData=data.getBinNo(1);
    size_t twojetbinData=data.getBinNo(2);
    float nominal1jetcontentdata = data.getBinContent(onejetbinData);
    float nominal2jetcontentdata = data.getBinContent(twojetbinData);
    // -------> end get bin

    // if you want to look at ratios, you might consider using the built-in
    // operators that account for systematics
    // e.g. put all systematic variations, etc in one bin of a new container
    // and then just divide it by another one
    // please check the test programs here and the histo1D.h headers
    // there is already a lot of built-in functionality


    ///////////////// CROSS SECTION /////////////////////
    float L = 19.741; //fb^-1

    float N1_ = nominal1jetcontentdata - nominal1jetcontentbackground;
    float N2_ = nominal2jetcontentdata - nominal2jetcontentbackground;

    float Ntt_emu = signal.integral(true);

    float Cb = 4*Ntt_emu*N2_tt/(N1_tt+2*N2_tt)/(N1_tt+2*N2_tt);

    float Cross_Section;
    Cross_Section = (N1_ + 2*N2_)*(N1_ + 2*N2_)*Cb/4/L/eps_emu/N2_;
    /////////END CROSS SECTION///////////////////////////


    std::cout << "Cross section = " << Cross_Section/1000.0 << " pb" << std::endl;
    std::cout << std::endl;
    std::cout << "DONE" <<std::endl;
}
