/*
 * mtExtractor.h
 *
 *  Created on: Feb 20, 2014
 *      Author: kiesej
 */

#ifndef MTEXTRACTOR_H_
#define MTEXTRACTOR_H_

#include <vector>

#include "TString.h"
#include "TtZAnalysis/Tools/interface/container.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/plotterBase.h"
#include "TtZAnalysis/Tools/interface/parameterExtractor.h"
#include "TtZAnalysis/Tools/interface/tObjectList.h"

#include "TF1.h"

namespace ztop{


class mtExtractor: public tObjectList{
public:
    mtExtractor():plotnamedata_(""),plotnamemc_(""),plottypemc_("cuf"),
    minbin_(-1),maxbin_(-1),excludebin_(-1),tmpglgraph_(0),tfitf_(0),iseighttev_(true),defmtop_(172.5),setup_(false),syspidx_(1) {reset();}
    ~mtExtractor(){cleanMem();}


    void setPlot(const TString& pl){plotnamedata_=pl;setup_=false;}
    void setMinBin(const int& pl){minbin_=pl;setup_=false;}
    void setMaxBin(const int& pl){excludebin_=pl;setup_=false;}
    void setExcludeBin(const int& pl){excludebin_=pl;setup_=false;}
    void setInputFilesData(const std::vector<TString>& pl){inputfiles_=pl;setup_=false;}

    /**
     * style file should incorporate styles for plotterCompare (MC/data dependence)
     * and styles for plotterSimple for chi2 plots
     */
    void setComparePlotterStyleFile(const std::string& f){compplotsstylefile_=f;}
    void setBinsPlotterStyleFile(const std::string& f){binsplotsstylefile_=f;}
    void setBinsChi2PlotterStyleFile(const std::string& f){binschi2plotsstylefile_=f;}

    void setFitMode(const TString& m){fitmode_=m;}

    void drawXsecDependence(TCanvas *c, bool fordata);
    void drawIndivBins(TCanvas *c);

    /**
     * syslayer from -1 (nominal) to systSize of data!
     * includesyst=true -> syslayer has no function and fit includes all systematics
     */
    void createBinLikelihoods(int syslayer,bool includesyst=false);
    void drawBinLikelihoods(TCanvas *c);
    /**
     * uses the created chi2 in bins
     */
    void createGlobalLikelihood();
    void fitGlobalLikelihood();
    void drawGlobalLikelihood(TCanvas *c,bool zoom=false);


    int getDataSystSize(){if(databingraphs_.size()>0) return (int)databingraphs_.at(0).getSystSize(); else return -3;}

    parameterExtractor * getExtractor(){return &paraExtr_;}

    void drawResultGraph(TCanvas *c);
    graph * getResultGraph(){return &allsyst_;}

    void setup(); //just  runs private functions

    void reset();//resets all calcs done, not inputfiles/plots etc.

    static bool debug;

    void cleanMem();

private:

    double getNewNorm(double deltam,bool eighttev=true)const;

    //init and read in
    void getMtValues();
    void readFiles(); //type specific stuff here
    void renormalize();
    void mergeSyst();
    void makeGraphs(); //at this step the systematics should be ordered in the same way (use first and copy!),
    // delete input conts

    //helper
    graph makeDepInBin(const std::vector<graph> &, size_t)const;

    //---a plotting step?---//
    // here for each mt a fully equipped distr is present
    void makeBinGraphs(); //still including all systematics use getBin()






    TString plotnamedata_;
    TString plotnamemc_; //ff
    TString plottypemc_; //ff
    int minbin_;
    int maxbin_;
    int excludebin_;
    std::vector<TString> inputfiles_;

    std::vector<container1D > datacont_;
    std::vector<container1D > mccont_;
    std::vector<graph > datagraphs_;
    std::vector<graph > mcgraphs_;
    std::vector<float> mtvals_;


    std::vector<graph > databingraphs_;
    std::vector<graph > mcbingraphs_;

    TString tmpSysName_;
    std::vector<graph > tmpbinchi2_;
    graph               tmpglchi2_;
    TGraphAsymmErrors * tmpglgraph_;
    TF1 * tfitf_;

    bool iseighttev_;
    float defmtop_;
    bool setup_;

    std::string compplotsstylefile_;
    std::string binsplotsstylefile_;
    std::string binschi2plotsstylefile_;
    std::vector<plotterBase *> pltrptrs_;

    parameterExtractor paraExtr_;

    graph allsyst_,allsystsl_,allsystsh_;
    std::string allsyststylefile_;
    float syspidx_;

    TString fitmode_;

};

}//ns



#endif /* MTEXTRACTOR_H_ */
