#include "../interface/NTBTagSF.h"
#include "TFile.h"
#include "TTree.h"


namespace ztop{


float NTBTagSF::getNTEventWeight(const std::vector<ztop::NTJet *> & jets){
    if(!isMC_ ) return 1.;
    if(mode_==eventsf_mode){
        if(makesEff()) return 1;
        resetCounter();
        for(size_t i=0;i<jets.size();i++){
            ztop::NTJet *jet=jets.at(i);
            countJet(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour());
        }
        return getEventSF();
    }
    else if(mode_==shapereweighting_mode){
        float outweight=1;
        for(size_t i=0;i<jets.size();i++){
            ztop::NTJet *jet=jets.at(i);
            outweight *= getJetDiscrShapeWeight(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour(),jet->btag());
        }
        return outweight;
    }
    else
        return 1;
}
float NTBTagSF::getNTEventWeight( ztop::NTJet * jet){
    if(!isMC_ ) return 1.;
    if(mode_==eventsf_mode){
        if(makesEff()) return 1;
        resetCounter();
        countJet(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour());
        return getEventSF();
    }
    else if(mode_==shapereweighting_mode){
        return getJetDiscrShapeWeight(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour(),jet->btag());
    }
    else
        return 1;
}
/**
 * changes the discriminator values of the jets using random technique.
 * After this step do not expect the discriminator distrubtions in data MC to match!
 *  if()
            return;
 */

void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> *jets)const{
    if(!isMC_ || mode_!=randomtagging_mode || makesEff()) return;
    for(size_t i=0;i<jets->size();i++){
        ztop::NTJet * jet=jets->at(i);
        if(std::abs(jet->genPartonFlavour()) == 0) continue;
        bool shouldbetagged=false;
        shouldbetagged= jetIsTagged(jet->pt(),std::abs(jet->eta()),jet->genPartonFlavour(),jet->btag(),
                std::abs<int>(1.e6 * sin(jet->phi())));

        // throw std::logic_error(" NTBTagSF::changeNTJetTags wrong type (undefined");

        if(!shouldbetagged){
            if(jet->btag()>getWPDiscrValue())
                jet->setBtag(-0.1);
        }
        else if(jet->btag()<getWPDiscrValue()){
            jet->setBtag(1.1);
        }
    }
}
void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> &jets)const{
    NTBTagSF::changeNTJetTags(&jets);
}


void NTBTagSF::setWorkingPoint(const TString& wpstring){
    std::cout << "NTBTagSF::setWorkingPoint: " << wpstring << std::endl;
    if(wpstring=="csvl"){
        bTagBase::setWorkingPoint(workingPoints::csvl_wp);}
    else if(wpstring=="csvm"){
        bTagBase::setWorkingPoint(workingPoints::csvm_wp);}
    else if(wpstring=="csvt"){
        bTagBase::setWorkingPoint(workingPoints::csvt_wp);}
    else{
        throw std::logic_error(("NTBTagSF::setWorkingPoint: doesn't exist"));
    }
    wpval_=bTagBase::getWPDiscrValue();
}


void NTBTagSF::setWorkingPointValue(const float &val){

    if(mode_==shapereweighting_mode)
        wpval_=val;
    else
        throw std::logic_error("NTBTagSF::setWorkingPointValue: only possible in shapereweighting_mode");

}

const float & NTBTagSF::getWPDiscrValue()const{
    if(mode_==shapereweighting_mode)
        return wpval_;
    else
        return bTagBase::getWPDiscrValue();
}



/*
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
 */

NTBTagSF  NTBTagSF::operator + (NTBTagSF  second){
    std::map<std::string,std::vector<TH2D> >::iterator sampleit,sampleit2;//=histos_.find(samplename);


    //check further
    if(effhistos_.size()<1 || (effhistos_.begin()->first.length()<1 && effhistos_.begin()->second.at(0).GetEntries() <1)){ //no name

        return second;
    }



    NTBTagSF thiscopy=*this;



    std::vector<size_t > tobeskipped;
    size_t i=0;
    for(sampleit=thiscopy.histos_.begin();sampleit!=thiscopy.histos_.end();++sampleit){
        std::string samplename1=sampleit->first;
        for(sampleit2=second.histos_.begin();sampleit2!=second.histos_.end();++sampleit2){
            std::string samplename2=sampleit2->first;
            if(samplename1 == samplename2){
                std::cout << "NTBTagSF::operator +: adding efficiencies with same samplename not supported! \ncheck unique naming\nwill just use old one"
                        << " "<<samplename2 << std::endl;
                tobeskipped.push_back(i);
            }
        }
        i++;
    }
    if(tobeskipped.size()<1){
        //none of the samples are the same
        thiscopy.histos_.insert(second.histos_.begin(),second.histos_.end()); // samplename vec<hist>
        thiscopy.effhistos_.insert(second.effhistos_.begin(),second.effhistos_.end()); // samplename vec<hist>
        thiscopy.medianMap_.insert(second.medianMap_.begin(),second.medianMap_.end()); // median map
    }
    sampleit2=second.effhistos_.begin();
    std::map<std::string,std::vector<float> >::iterator  mmapit=second.medianMap_.begin();
    sampleit=second.histos_.begin();

    for(size_t i=0;i<second.histos_.size();i++){
        if(std::find(tobeskipped.begin(),tobeskipped.end(),i) == tobeskipped.end() ){ //unique
            thiscopy.histos_.insert(sampleit,++sampleit);
            thiscopy.effhistos_.insert(sampleit2,++sampleit2);
            thiscopy.medianMap_.insert(mmapit,++mmapit);
        }
        else{
            sampleit++;
            sampleit2++;
            mmapit++;
        }
    }

    return thiscopy;
}



/**
 * recreates file!
 */
void NTBTagSF::writeToTFile(TString filename){
    AutoLibraryLoader::enable();
    TH1::AddDirectory(false);
    TFile * f = new TFile(filename,"RECREATE");
    bool madenew=false;
    TTree * t=0;
    if(f->Get("NTBTagSFs")){
        t = (TTree*) f->Get("NTBTagSFs");
    }
    else{
        madenew=true;
        t = new TTree("NTBTagSFs","NTBTagSFs");
    }
    if(t->GetBranch("NTBTagSFs")){ //branch does  exist
        NTBTagSF * bt=this;
        t->SetBranchAddress("NTBTagSFs", &bt);
    }
    else{
        t->Branch("NTBTagSFs",this);
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




void NTBTagSF::readFromTFile(TString filename){
    AutoLibraryLoader::enable();
    TH1::AddDirectory(false);
    TFile * f = new TFile(filename);
    TTree * t = (TTree*) f->Get("NTBTagSFs");
    NTBTagSF * bt=0;
    int branchret=t->SetBranchAddress("NTBTagSFs", &bt) ;
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
    if(bt) delete bt;
    f->Close();
    delete f;

}

void NTBTagSF::listAllSampleNames()const{
    std::map<std::string,std::vector<TH2D> >::const_iterator sampleit;
    for(sampleit=histos_.begin();sampleit!=histos_.end();++sampleit){
        std::cout << sampleit->first << std::endl;
    }


}

bool NTBTagSF::isRealSyst()const{
    //some hardcoded stuff
    if(getSystematic()  == bTagBase::nominal)
        return false;

    if(mode_ != shapereweighting_mode){
        if(getSystematic() == bTagBase::jesup)
            return false;
        if(getSystematic()  == bTagBase::jesdown)
            return false;
        if(getSystematic()  == bTagBase::hfstat1up)
            return false;
        if(getSystematic()  == bTagBase::hfstat1down)
            return false;
        if(getSystematic()  == bTagBase::hfstat2up)
            return false;
        if(getSystematic()  == bTagBase::hfstat2down)
            return false;
        if(getSystematic()  == bTagBase::lfstat1up)
            return false;
        if(getSystematic()  == bTagBase::lfstat1down)
            return false;
        if(getSystematic()  == bTagBase::lfstat2up)
            return false;
        if(getSystematic()  == bTagBase::lfstat2down)
            return false;
        if(getSystematic()  == bTagBase::cerr1up)
            return false;
        if(getSystematic()  == bTagBase::cerr1down)
            return false;
        if(getSystematic()  == bTagBase::cerr2up)
            return false;
        if(getSystematic()  == bTagBase::cerr2down)
            return false;
    }
    return true;
}

}//namespace
