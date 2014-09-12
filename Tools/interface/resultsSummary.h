/*
 * resultsSummary.h
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */

#ifndef RESULTSSUMMARY_H_
#define RESULTSSUMMARY_H_

#include "../interface/graph.h"
#include "../interface/texTabler.h"
#include <utility>

namespace ztop{

/**
 * can be also used for summary parts
 */
class resultsSummary{
public:
    resultsSummary():sysrelout_(true){}
    ~resultsSummary(){}


    graph makeGraph()const;
   // texTabler getTexTable(const float & prec=2)const;

    const TString& getEntryName(size_t entr)const;
    TString getValueString(size_t entr, bool& isNeglegible, const float & prec=0.01)const;
    TString getStatString(size_t entr, bool& isNeglegible, const float & prec=0.01)const;

    void setSystBreakRelative(bool rel){sysrelout_=rel;}

    /**
     * give full numbers not % nor deviations to nominal
     * no unique naming check
     */
    void addEntry(const TString & name, const float & entry,
            const float& statup, const float& statdown,
            const float& totup=0, const float& totdown=0);

    /**
     * if many entries with same name only removes first occurrence
     */
    void removeEntry(const TString& name);
    /**
     * assumes all uncorrelated
     * calculates wrt to nominal entry and takes its stat
     * transforms all entries to (not %) deviations!
     * with totup/totdown entries
     */
    resultsSummary createTotalError(const TString nomentry="nominal")const;

    void mergeVariations(const TString &nomentry,const std::vector<TString>& names,
    		const TString & outname,bool linearly=false);

    void mergeVariationsFromFile(const TString &nomentry,const std::string& filename);
    void mergeVariationsFromFileInCMSSW(const TString &nomentry,const std::string& filename);


    size_t nEntries()const{return entries_.size();}

    static bool debug;

private:


    //small helper
    class summaryEntry{
    public:
        summaryEntry():entry(0),statup(0),statdown(0),totup(0),totdown(0),issys(false){}

        float entry,statup,statdown,totup,totdown;
        TString name;
        bool issys;
    };

    std::vector<summaryEntry> entries_;

    summaryEntry& getEntry(const TString & name);
    const summaryEntry& getEntry(const TString & name)const;

    std::vector< std::pair<size_t,size_t> > getVariations( std::vector<TString> & out)const;
    std::vector< std::pair<size_t,size_t> > getVariations()const;

    bool sysrelout_;

};


}



#endif /* RESULTSSUMMARY_H_ */
