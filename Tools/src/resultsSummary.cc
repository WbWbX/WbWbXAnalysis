/*
 * resultsSummary.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */


#include "../interface/resultsSummary.h"
#include <algorithm>
#include "../interface/formatter.h"

namespace ztop{


graph resultsSummary::makeGraph()const{


    return graph();
}
const TString& resultsSummary::getEntryName(size_t entr)const{

    return entries_.at(entr).name;

}

TString resultsSummary::getValueString(size_t idx, const float & prec)const{

    formatter fmt;
    TString out;
    if(entries_.at(idx).issys){
        float up=fmt.round(entries_.at(idx).totup,prec);
        float down=fmt.round(entries_.at(idx).totdown,prec);

        out="$";
        /*   if(up>0 && down<0)
            out+="\\pm";
        else if(up<0 && down>0)
            out+="\\mp";
        else if(down <0 && up<0)
            out+="-";
        else if(down >0 && up>0)
            out+="+"; */


        if(fabs(fabs(up)-fabs(down))>prec){
            TString upvar;
            if(up>0)
                upvar="+"+fmt.toTString(fabs(up));
            else
                upvar="-"+fmt.toTString(fabs(up));
            TString downvar;
            if(down>0)
                downvar="+"+fmt.toTString(fabs(down));
            else
                downvar="-"+fmt.toTString(fabs(down));


            out+="^{"+upvar+"}_{"+downvar+"}$";
        }
        else{
            out+="\\pm"+fmt.toTString(fabs(up))+"$";
        }
    }
    else{

        out=fmt.toTString( fmt.round(entries_.at(idx).entry,prec));

    }
    return out;

}

void resultsSummary::addEntry(const TString & name, const float & entry,
        const float& statup, const float& statdown,
        const float& totup, const float& totdown){

    summaryEntry newentry;
    newentry.name=name;
    newentry.entry=entry;
    newentry.statup=statup;
    newentry.statdown=statdown;
    newentry.totup=totup;
    newentry.totdown=totdown;
    entries_.push_back(newentry);

}

void resultsSummary::removeEntry(const TString& name){

    for(size_t i=0;i<entries_.size();i++){
        if(entries_.at(i).name == name){
            entries_.erase(entries_.begin()+i);
            return;
        }
    }
}

resultsSummary resultsSummary::createTotalError(const TString nomentry)const{

    resultsSummary out;

    summaryEntry nominal=getEntry(nomentry);
    std::vector<TString> vars;
    getVariations(vars);

    float nomp=nominal.entry;
    float errup2=0,errdown2=0;
    for(size_t i=0;i<vars.size();i++){


        const summaryEntry& upentry=getEntry(vars.at(i)+"_up");
        const summaryEntry& downentry=getEntry(vars.at(i)+"_down");

        float diffa= upentry.entry - nomp;
        float diffb= downentry.entry - nomp;


        if(sysrelout_){
            diffa/=nomp;diffa*=100;
            diffb/=nomp;diffb*=100;
        }

        out.addEntry(vars.at(i),0,0,0,diffa,diffb);


        float up = diffa;
        if(diffb>up) up= diffb;
        if(up< 0) up=0;

        float down=diffb;
        if(diffa<up) down=diffa;
        if(down>0) down=0;

        errup2+=up*up;
        errdown2+=down*down;
    }
    if(sysrelout_){
        errup2+=nominal.statup/nomp * nominal.statup/nomp * 10000;
        errdown2+=nominal.statdown/nomp * nominal.statdown/nomp * 10000;
        out.addEntry("stat",0,0,0,nominal.statup/nomp*100,nominal.statdown/nomp*100);
    }
    else{
        errup2+=nominal.statup * nominal.statup;
        errdown2+=nominal.statdown * nominal.statdown;
        out.addEntry("stat",0,0,0,nominal.statup,nominal.statdown);
    }



    out.addEntry("total",0,0,0,sqrt(errup2),-sqrt(errdown2));

    for(size_t i=0;i<out.nEntries();i++)
        out.entries_.at(i).issys=true;
    return out;

}

resultsSummary::summaryEntry& resultsSummary::getEntry(const TString & name){
    for(size_t i=0;i<entries_.size();i++){
        if(entries_.at(i).name == name){
            return entries_.at(i);
        }
    }
    throw std::out_of_range("resultsSummary::getEntry not found");
}


const resultsSummary::summaryEntry& resultsSummary::getEntry(const TString & name)const{
    for(size_t i=0;i<entries_.size();i++){
        if(entries_.at(i).name == name){
            return entries_.at(i);
        }
    }
    std::cout << "resultsSummary::getEntry " <<name << " not found" <<std::endl;
    throw std::out_of_range("resultsSummary::getEntry not found");
}

std::vector< std::pair<size_t,size_t> > resultsSummary::getVariations( std::vector<TString> &out)const{
    std::vector< std::pair<size_t,size_t> > pairs;
    out.clear();
    for(size_t i=0;i<entries_.size();i++){
        const TString& entry=entries_.at(i).name;
        TString newstr="";
        bool isvar=false;
        if(entry.EndsWith("_up")){
            newstr=TString(entry,entry.Last('_'));
            isvar=true;
        }
        if(entry.EndsWith("_down")){
            newstr=TString(entry,entry.Last('_'));
            isvar=true;
        }
        std::vector<TString>::const_iterator it=std::find(out.begin(),out.end(),newstr);
        if(isvar && it != out.end()){ //not new
            std::pair<size_t,size_t> p(i,it-out.begin());
            pairs.push_back(p);
        }
        else if(newstr.Length()>0){
            out.push_back(newstr);
        }
    }
    return pairs;
}
/*
std::vector< std::pair<size_t,size_t> > resultsSummary::getVariations()const{
    std::vector<TString> dummy;
    return getVariations(dummy);
}*/

}

