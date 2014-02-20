/*
 * histoBin.cc
 *
 *  Created on: Aug 26, 2013
 *      Author: kiesej
 */
#include "../interface/histoBin.h"
#include <iostream>
#include <omp.h>
namespace ztop{

/**
 * clears stat, entries, and content.
 * keeps name
 */
void histoBin::clear(){
    content_=0;
    entries_=0;
    stat2_=0;
}

void histoBin::multiply(const float&val){
    content_*=val;
    stat2_=stat2_*val*val;
    entries_*=val;
}

bool histoBin::operator != (const histoBin& rhs)const{
    if(entries_!=rhs.entries_) return true;
    if(content_!=rhs.content_) return true;
    if(stat2_!=rhs.stat2_) return true;
    if(name_!=rhs.name_) return true;
    return false;
}
bool histoBin::operator == (const histoBin& rhs)const{
    return !(*this!=rhs);
}

/////binning//////

bool histoBins::showwarnings=false;

histoBins::histoBins(size_t Size) : name_(""),layer_(-1){
    setSize(Size);
}
void histoBins::setSize(size_t Size){
    bins_.clear();
    histoBin hb;
    bins_.resize(Size,hb);
}
/**
 * returns true if bins were added
 */
bool histoBins::resize(const size_t & newsize){
    size_t oldsize=bins_.size();
    bins_.resize(newsize);
    return bins_.size()>oldsize;
}

/**
 * histoBins::add(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::add(const histoBins& rhs,bool statCorr){
    if(size()!=rhs.size())
        return -1;
    for(size_t i=0;i<size();i++){
        getBin(i).addToContent(rhs.getBin(i).getContent());
        float staterr=0;
        if(statCorr){
            staterr=getBin(i).getStat()+rhs.getBin(i).getStat();
            getBin(i).setStat(staterr);
        }
        else{
            getBin(i).addToStat(rhs.getBin(i).getStat());
        }
        getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
    }
    return 0;
}
/**
 * histoBins::subtract(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::subtract(const histoBins& rhs,bool statCorr){
    if(size()!=rhs.size())
        return -1;
    for(size_t i=0;i<size();i++){
        getBin(i).setContent(getBin(i).getContent()-rhs.getBin(i).getContent());
        float staterr=0;
        if(statCorr){
            staterr=getBin(i).getStat()-rhs.getBin(i).getStat();
            getBin(i).setStat(staterr);
        }
        else{
            getBin(i).addToStat(rhs.getBin(i).getStat());
        }
        getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
    }
    return 0;
}
/**
 * histoBins::divide(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::divide(const histoBins& rhs,bool statCorr){
    if(size()!=rhs.size())
        return -1;
    for(size_t i=0;i<size();i++){
        bool divbinstatcorr=statCorr;
        float div=0;
        if(rhs.getBin(i).getContent()==0){
            if(showwarnings) std::cout << "histoBins::divide: 0 content in bin " << i << std::endl;
            div=0;
        }
        else{
            div=getBin(i).getContent()/rhs.getBin(i).getContent();
        }
        float staterr2=0;

        if(divbinstatcorr){
            staterr2=fabs(div*(1-div)/rhs.getBin(i).getContent());
            if(staterr2!=staterr2) staterr2=0; //nan safe
        }
        else{
            staterr2=getBin(i).getStat2()/(rhs.getBin(i).getContent()*rhs.getBin(i).getContent()) + div*div*rhs.getBin(i).getStat2()/(rhs.getBin(i).getContent()*rhs.getBin(i).getContent());
            if(staterr2!=staterr2) staterr2=0; //nan safe
        }
        getBin(i).setStat2(staterr2);
        getBin(i).setContent(div);
        getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
    }
    return 0;
}
/**
 * histoBins::multiply(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::multiply(const histoBins& rhs,bool statCorr){
    if(size()!=rhs.size())
        return -1;
    for(size_t i=0;i<size();i++){
        const float & ca=getBin(i).getContent();
        const float & cb=rhs.getBin(i).getContent();
        const float & ea2=getBin(i).getStat2();
        const float & eb2=rhs.getBin(i).getStat2();

        float mult=ca*cb;
        float staterr2=0;
        if(statCorr){
            float ea=sqrt(ea2);
            float eb=sqrt(eb2);
            staterr2=((ca+ea)*(cb+eb) - mult)*((ca+ea)*(cb+eb) - mult);
        }
        else{

            staterr2=ca*ca*eb2+cb*cb*ea2;
        }
        getBin(i).setStat2(staterr2);
        getBin(i).setContent(mult);
        getBin(i).setEntries(getBin(i).getEntries() * rhs.getBin(i).getEntries());
    }
    return 0;
}
void histoBins::multiply(const float& val){
    for(size_t i=0;i<size();i++)
        bins_[i].multiply(val);
}

void histoBins::removeStat(){
    for(size_t i=0;i<size();i++)
        bins_[i].setStat2(0);
}

bool histoBins::operator !=(const histoBins& rhs) const{
    if(layer_ != rhs.layer_) return true;
    if(bins_!=rhs.bins_) return true;
    if(name_!=rhs.name_) return true;
    return false;
}
bool histoBins::operator ==(const histoBins& rhs) const{
    return !(*this!=rhs);
}

}//namespace

