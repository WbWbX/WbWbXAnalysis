/*
 * compareNTuples.cc
 *
 *  Created on: Jun 16, 2014
 *      Author: kiesej
 */



#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include <vector>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TtZAnalysis/DataFormats/src/classes.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){

    using namespace ztop;
    using namespace std;

    parser->setAdditionalDesciption("just compares the content of two ntuples. \
             Processed events and events in ntuple\n \
            needs two input files");
    //  TString outputadd=parser->getOpt<TString>("o","","additional string to be added to output file names");
    // TString inputadd =parser->getOpt<TString>("i","","additional string to be added to input file names");
    std::vector<TString> files = parser->getRest<TString>();

    if(files.size()<2){
        parser->coutHelp();
    }


    //for future set comparison
    std::vector<TString> filesa,filesb;
    //  filesa.push_back(files.at(0));
    //  filesb.push_back(files.at(1));

    //search for double filenames in input (to know which to compare)
    //and only ush back the ones that are indeed double
    textFormatter tf;

    std::vector<TString> onlyfilenames;
    for(size_t i=0;i<files.size();i++){
        if(!fileExists(files.at(i))){
            std::cout << "file " << files.at(i) << " does not exists exit"  <<std::endl;
            exit(1);
        }
        TString filename=tf.getFilename(files.at(i).Data());
        onlyfilenames.push_back(filename);
    }

    for(size_t i=0;i<onlyfilenames.size()-1;i++){
        size_t secpos= std::find(onlyfilenames.begin()+i+1,onlyfilenames.end(),onlyfilenames.at(i)) - onlyfilenames.begin() ;
        if(secpos!=onlyfilenames.size()){ //duplicate found
            if(std::find(filesa.begin(),filesa.end(),files.at(i))==filesa.end()){ //but not yet in there
                filesa.push_back(files.at(i));
                filesb.push_back(files.at(secpos));

             //   std::cout << "\n" << files.at(i) <<"\t" <<files.at(secpos) <<std::endl;
            }
        }


    }


    for(size_t i=0;i<filesa.size();i++){

        std::cout << "comparing " << filesa.at(i) << " with " << filesb.at(i) << std::endl;

        TFile *fa = TFile::Open(filesa.at(i));
        TFile *fb = TFile::Open(filesb.at(i));

        TTree * pfta = (TTree*) fa->Get("PFTree/PFTree");

        TTree * pftb = (TTree*) fb->Get("PFTree/PFTree");

        size_t entriesa=pfta->GetEntries() ;
        size_t entriesb=pftb->GetEntries() ;
        float diffpercent = (((float)entriesb -(float)entriesa ) / (float)  entriesa + 1)*100;

        TString out= "Analysis tree entries\n" + toTString(entriesa)
                + "\t"  + toTString(entriesb) + "\t" + toTString(diffpercent) + "%";

        if(entriesa!=entriesb){
            out="\033[1;31m"+out;
            out+="\033[0m";
        }

        std::cout << out << std::endl;


        /////

        pfta = (TTree*) fa->Get("preCutPUInfo/preCutPUInfo");
        pftb = (TTree*) fb->Get("preCutPUInfo/preCutPUInfo");

        entriesa=pfta->GetEntries() ;
        entriesb=pftb->GetEntries() ;
        diffpercent = (((float)entriesb -(float)entriesa ) / (float)  entriesa + 1)*100;

        out= "preCutPUInfo tree entries\n" + toTString(entriesa)
                + "\t"  + toTString(entriesb) + "\t" + toTString(diffpercent) + "%";

        if(entriesa!=entriesb){
            out="\033[1;31m"+out;
            out+="\033[0m";
        }

        std::cout << out <<"\n" << std::endl;

        fa->Close();
        fb->Close();
    }
    return 0;
}

