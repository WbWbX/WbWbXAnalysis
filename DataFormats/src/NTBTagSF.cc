#include "../interface/NTBTagSF.h"
#include "TFile.h"
#include "TTree.h"


namespace ztop{



void  NTBTagSF::setSystematic(const TString &sys=""){
    std::cout << "NTBTagSF::setSystematic: " <<sys<<std::endl;
    if(sys=="heavy up")
        bTagBase::setSystematic(bTagBase::heavyup);
    else if(sys=="heavy down")
        bTagBase::setSystematic(bTagBase::heavydown);
    else if(sys=="light up")
        bTagBase::setSystematic(bTagBase::lightup);
    else if(sys=="light down")
        bTagBase::setSystematic(bTagBase::lightdown);
    else if(sys=="nominal"||sys=="no"||sys=="")
        bTagBase::setSystematic(bTagBase::nominal);
    else
        throw std::logic_error(("NTBTagSF::setSystematic doesn't exist"));

}


NTBTagSF  NTBTagSF::operator + (NTBTagSF  second){
    std::map<std::string,std::vector<TH2D> >::iterator sampleit,sampleit2;//=histos_.find(samplename);
    for(sampleit=histos_.begin();sampleit!=histos_.end();++sampleit){
        std::string samplename1=sampleit->first;
        for(sampleit2=second.histos_.begin();sampleit2!=second.histos_.end();++sampleit2){
            std::string samplename2=sampleit2->first;
            if(samplename1 == samplename2){
                std::cout << "NTBTagSF::operator +: adding efficiencies with same samplename not supported! check unique naming" << std::endl;
                return second;
            }
        }
    }
    //none of the samples are the same
    second.histos_.insert(histos_.begin(),histos_.end()); // samplename vec<hist>
    second.effhistos_.insert(effhistos_.begin(),effhistos_.end()); // samplename vec<hist>
    return second;
}




void NTBTagSF::writeToTFile(TString filename, std::string treename){
    AutoLibraryLoader::enable();
    TFile * f = new TFile(filename,"RECREATE");
    bool madenew=false;
    TTree * t=0;
    if(f->Get((treename).data())){
        t = (TTree*) f->Get(treename.data());
    }
    else{
        madenew=true;
        t = new TTree(treename.data(),treename.data());
    }
    if(t->GetBranch("allNTBTagSF")){ //branch does  exist
        NTBTagSF * bt=this;
        t->SetBranchAddress("allNTBTagSF", &bt);
    }
    else{
        t->Branch("allNTBTagSF",this);
        std::cout << "NTBTagSF::writeToTFile: added branch" << std::endl;
    }
    setMakeEff(false);
    cleanptr();
    t->Fill();
    t->Write("",TObject::kOverwrite);
    if(madenew)
        delete t;
    f->Close();
    delete f;
}




void NTBTagSF::readFromTFile(TString filename, std::string treename){
    AutoLibraryLoader::enable();
    TFile * f = new TFile(filename);
    TTree * t = (TTree*) f->Get(treename.data());
    NTBTagSF * bt=0;
    int branchret=t->SetBranchAddress("allNTBTagSF", &bt) ;
    if(branchret <0){
        std::cout << "NTBTagSF::readFromTFile: reading branch not successful (return " << branchret << ")" <<std::endl;
        throw std::runtime_error("NTBTagSF::readFromTFile: branch not found");
    }
    int entries=t->GetEntries();
    if(entries>1)
        std::cout << "NTBTagSF::readFromTFile: warning more than one NTBTagSF Class found. chose first entry" << std::endl;
    for(float n=0;n<t->GetEntries();n++){
        t->GetEntry(n);
        if(bt){
            *this=*bt;
            setMakeEff(false);
            break;
        }
    }
    f->Close();
    delete f;
}

}//namespace
