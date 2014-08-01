/*
 * systMerger.cc
 *
 *  Created on: Feb 14, 2014
 *      Author: kiesej
 */



#include "../interface/systMerger.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "../interface/AnalysisUtils.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include <fstream>
#include "TFile.h"
#include <iostream>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include <omp.h>
#include "../interface/discriminatorFactory.h"

namespace ztop{


bool systMerger::debug=false;

void systMerger::setInputStrings(const std::vector<std::string>& instrings){
    //needs to deliver a directory and filenames without root (by convention the same as csv names
    using namespace ztop;
    std::string addstr="";
    if(infileadd_.Length()>0)
        addstr=("_"+infileadd_).Data();
    //get dir from first one and then check if changes. if it does, throw.
    //it could mean same csv names
    size_t ignorecount=0;
    for(size_t i=0;i<instrings.size();i++){
        const std::string& instring=instrings.at(i);
        if(infileadd_.Length()>0 && !hasEnding(instring,addstr)){
            ignorecount++;
            if(debug) std::cout << "systMerger::setInputStrings: ignored " << instring <<std::endl;
            continue; //wrong add
        }
        std::string st=instring;
        std::string dir="";
        if(instring.find("/") != std::string::npos){
            dir=st.substr( 0, st.find_last_of( '/' ) +1 );
            st=st.substr( st.find_last_of( '/' ) +1 );
            //now its only the filename that remains
        }
        if(dir.size()>0){
            if(ignorecount == i){ //first entry not ignored
                indir_=dir;
                if(debug) std::cout << "systMerger::setInputStrings: used dir " << indir_ <<std::endl;
            }
            else{
                if(indir_!=dir)
                    throw std::runtime_error(
                            "systMerger::setInputStrings: files with same ..<marer>.root need to be in the same directory");
            }
        } //dir set
        TString topushback=instring;
        topushback.ReplaceAll(".root","");
        if(infileadd_.Length()>0)
            topushback.ReplaceAll(infileadd_,"");
        if(debug) std::cout << "systMerger::setInputStrings: pushed back " << topushback <<std::endl;
        instrings_.push_back(topushback);
    }
}


std::vector<TString>  systMerger::start(){
    searchNominals();
    searchSystematics();
    std::vector<TString>  outfilenames=mergeAndSafe();
    mergeBTags();
    return outfilenames;
}



///sort inputs
void systMerger::searchNominals(){
    if(debug) std::cout << "systMerger::searchNominals" <<std::endl;
    TString check="_nominal";
    nominals_.clear();
    for(size_t i=0;i<instrings_.size();i++){
        if(instrings_.at(i).Contains(check) && (infileadd_.Length() < 1 || instrings_.at(i).EndsWith("_"+infileadd_))){
            if(debug) std::cout << "systMerger::searchNominals: identified " << instrings_.at(i) <<std::endl;
            nominals_.push_back(i);
        }
    }
}

void systMerger::searchSystematics(){
    if(debug) std::cout << "systMerger::searchSystematics" <<std::endl;
    //create syst vector
    if(nominals_.size() < 1 && instrings_.size() >0){
        throw std::runtime_error("systMerger::searchSystematics: no input"); //should not happen!
    }
    syst_.clear();
    //resize
    syst_.resize(nominals_.size(),std::vector<size_t>());
    if(debug) std::cout << "systMerger::searchSystematics: resized" <<std::endl;
    //for each nominal the corresp syst
    for(size_t nom=0;nom<nominals_.size();nom++){
        TString startstring=instrings_.at(nominals_.at(nom));
        if(debug) std::cout << "systMerger::searchSystematics: for nominal "<< startstring<<std::endl;

        startstring.ReplaceAll("_nominal","");
        for(size_t sys=0;sys<instrings_.size();sys++){
            if(sys==nominals_.at(nom)) continue; //is nominal
            if(instrings_.at(sys).BeginsWith(startstring) && (infileadd_.Length() < 1 ||  instrings_.at(sys).EndsWith("_"+infileadd_))){
                if(debug) std::cout << "systMerger::searchSystematics: identified " << instrings_.at(sys)
                                        << " for nominal " << instrings_.at(nominals_.at(nom)) << std::endl;

                syst_.at(nom).push_back(sys);
            }
        }
    }
}

containerStackVector * systMerger::getFromFileToMem( TString dir, const TString &name)const
{
    containerStackVector * vtemp=0;
    TFile * ftemp=0;
    TTree * ttemp =0;
#pragma omp critical
    {
        AutoLibraryLoader::enable();
        if(dir!="") dir+="/";

        ftemp=new TFile(dir+name+".root","read");
        ttemp = (TTree*)ftemp->Get("containerStackVectors");

        vtemp=new containerStackVector();
        vtemp->loadFromTree(ttemp,name);
        delete ttemp;
        delete ftemp;
    }

    return vtemp;
}


std::vector<TString>  systMerger::mergeAndSafe(){
    if(debug) std::cout << "systMerger::mergeAndSafe" <<std::endl;
    std::vector<TString>  outputfilenames;
    //get a nominal
    TString dir=indir_;
    TString outadd="";
    if(outfileadd_.Length()>0)
        outadd="_"+outfileadd_;

    //not parallel for all nominals, file IO is the bottleneck!


    std::vector<discriminatorFactory> alldisc;
    for(size_t nom=0;nom<nominals_.size();nom++){
        containerStackVector * nominal=getFromFileToMem(dir,instrings_.at(nominals_.at(nom)));
        TString reldir=dir;
        if(reldir!="")reldir+="/";
        std::vector<discriminatorFactory> disc;
        if(fileExists(reldir+instrings_.at(nominals_.at(nom))+"_discr.root")){
            disc=discriminatorFactory::readAllFromTFile(reldir+instrings_.at(nominals_.at(nom))+"_discr.root");//get nominal configuration
        }
        for(size_t sys=0;sys<syst_.at(nom).size();sys++){
            containerStackVector * sysvec=getFromFileToMem(dir,instrings_.at(syst_.at(nom).at(sys)));
            if(debug) std::cout << "systMerger::mergeAndSafe: adding "<< sysvec->getName() << " to " << nominal->getName()<< std::endl;
            nominal->addMCErrorStackVector(*sysvec);
            delete sysvec;
        }
        nominal->setName(nominal->getName()+outadd+"_syst");
        std::cout << "systMerger: writing "<< nominal->getName() << std::endl;
        outputfilenames << nominal->getName()+".root";
        for(size_t i=0;i<disc.size();i++){
            disc.at(i).extractLikelihoods(*nominal);

        }
        alldisc << disc;
        //remove old
        nominal->writeAllToTFile(nominal->getName()+".root",true);

        //clear discr factory mem


        //scope should have ended here - delete necc?

        delete nominal;
    }

    system(((TString)"rm -f "+"all"+outadd+ "_discr.root").Data());
    if(alldisc.size()>0)
        discriminatorFactory::writeAllToTFile("all" +outadd+  "_discr.root",alldisc);

    return outputfilenames;
}

void systMerger::mergeBTags()const{
    if(debug) std::cout << "systMerger::mergeBTags" <<std::endl;
    using namespace ztop;
    using namespace std;
    NTBTagSF btags;
    //check if file exists, if yes, assume btags have been recreated and merge

    if(instrings_.size()>0){
        TString check=instrings_.at(0);

        std::ifstream OutFileTest((instrings_.at(0)+"_btags.root").Data());
        if(OutFileTest) { //btags were created
            cout << "systMerger: merging btag SFs to "<<outfileadd_+"all_btags.root" <<endl;

            for(size_t i=0;i<instrings_.size();i++){
                TString filename=instrings_.at(i);
                if(infileadd_.Length()>0)
                    filename+="_"+infileadd_+"_btags.root";
                else
                    filename+="_btags.root";
                NTBTagSF temp;

                std::ifstream OutFileTest2(filename.Data());
                if(OutFileTest2){
                    temp.readFromTFile(filename);
                    btags=btags+temp;
                }
                else{
                    cout << "btag file " << filename << " not found" << endl;
                }
            }


            btags.writeToTFile(outfileadd_+"all_btags.root");
        }
        else{
            std::cout << "systMerger: did not find a valid b-tag file. Not merging b-tag info" << std::endl;
        }

    }
}





}


