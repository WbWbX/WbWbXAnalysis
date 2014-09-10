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
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/plotterBase.h"
#include "TtZAnalysis/Tools/interface/parameterExtractor.h"
#include "TtZAnalysis/Tools/interface/tObjectList.h"
#include "TtZAnalysis/Tools/interface/texTabler.h"
#include "TtZAnalysis/Tools/interface/resultsSummary.h"

#include "TF1.h"
#include "TFile.h"
#include <stdexcept>

namespace ztop{


class mtExtractor: public tObjectList{
public:
    mtExtractor();
    ~mtExtractor(){cleanMem();}

    void setUseFolding(bool use){dofolding_=use;}

    void setPlot(const TString& pl){plotnamedata_=pl;setup_=false;}
    void setMinBin(const int& pl){minbin_=pl;setup_=false;}
    void setMaxBin(const int& pl){maxbin_=pl;setup_=false;}
    void setExcludeBin(const int& pl){excludebin_=pl;setup_=false;}
    /**
     * define the format of input files.
     * Filenames: filepreamble_topmass_fileendpattern (incl. e.g. ".root")
     * histonames: give list of histonames association.
     * Replace ANY blank with an underscore (m_lb_bla_step_8 = hist1234)
     * define suffix for syst up/down in systUpId/systDownId
     * define nominal id nominalId
     * define if its divided by binwidth inputIsDividedByBinWidth = true/false
     */
    void setConfigFile(const TString& pl);
    void setExternalPrefix(const TString& prefix){extfilepreamble_=prefix;}
    void setExternalGenInputPDF(const TString& pl){extfilepdf_=pl;setup_=false;}
    void setRescaleNLOPred(const bool& doit){rescalepreds_=doit;}
    void setInputFiles(const std::vector<TString>& pl);
    bool getIsExternalGen()const{return isexternalgen_;}

    void setFitUncertaintyModeData(parameterExtractor::fituncertaintyModes uncmode){paraExtr_.setFitUncertaintyModeA(uncmode);}
    void setFitUncertaintyModeMC(parameterExtractor::fituncertaintyModes uncmode){paraExtr_.setFitUncertaintyModeB(uncmode);}

    void setUseNormalized(bool usenormd){usenormalized_=usenormd;}

    /**
     * ONLY affects legend/color ordering. No effect on result
     */
    void setDefMTop(float inmt){defmtop_=inmt;}
    float getDefMTop()const{return defmtop_;}

    /**
     * style file should incorporate styles for plotterCompare (MC/data dependence)
     * and styles for plotterSimple for chi2 plots
     */
    /*   void setComparePlotterStyleFileMC(const std::string& f){compplotsstylefilemc_=f;}
    void setComparePlotterStyleFileData(const std::string& f){compplotsstylefiledata_=f;}
    void setBinsPlotterStyleFile(const std::string& f){binsplotsstylefile_=f;}
    void setBinsChi2PlotterStyleFile(const std::string& f){binschi2plotsstylefile_=f;}
    void setAllSystStyleFile(const std::string & in){allsyststylefile_=in;}

    void setBinsPlusFitPlotterStyleFile(const std::string& f){binsplusfitsstylefile_=f;}
     */
    void setFitMode(const TString& m){fitmode_=m;}

    void setIgnoreDataStat(bool ignore){ignoredatastat_=ignore;}
    void setIgnoreBGStat(bool ignore){ignorebgstat_=ignore;}

    void drawXsecDependence(TCanvas *c, bool fordata);
    void drawIndivBins(TCanvas *c,int syst=-2);

    std::vector<TString> getSystNameList()const{if(mccont_.size()<1) return std::vector<TString>(); return mccont_.at(0).getSystNameList();}
    //in extra loop!
    void drawSystVariation(TCanvas *c,const TString & sys);

    /**
     * syslayer from -1 (nominal) to systSize of data!
     * includesyst=true -> syslayer has no function and fit includes all systematics
     */
    void createBinLikelihoods(int syslayer,bool includesyst=false,bool datasyst=true);
    void drawBinLikelihoods(TCanvas *c);
    /*
    void overlayBinFittedFunctions(TCanvas *c);
     */
    /**
     * need to be called after each syst variation otherwise result graphs lost
     */
    void drawBinsPlusFits(TCanvas *c,int syst);
    /**
     * uses the created chi2 in bins
     */
    void createGlobalLikelihood();
    void fitGlobalLikelihood();
    /**
     * returns the minimum Y value
     */
    float drawGlobalLikelihood(TCanvas *c,bool zoom=false);


    int getDataSystSize(){if(databingraphs_.size()>0) return (int)databingraphs_.at(0).getSystSize(); else return -3;}
    int getMCSystSize(){if(mcbingraphs_.size()>0) return (int)mcbingraphs_.at(0).getSystSize(); else return -3;}
    const TString & getSystName(size_t idx)const {
        if(databingraphs_.size()>0) return databingraphs_.at(0).getSystErrorName(idx);
        throw std::logic_error("mtExtractor::getSystName: no graphs");}
    size_t getNDof()const{if(datagraphs_.size()>0) return datagraphs_.at(0).getNPoints()-1; else return 0;}

    parameterExtractor * getExtractor(){return &paraExtr_;}

    /**
     * can write back nominal out with stat uncert. (for calib mode)
     */
    void drawResultGraph(TCanvas *c, float * nom=0, float * errd=0, float * erru=0,float * syserrdp=0, float * syserrup=0);
    graph * getResultGraph(){return &allsyst_;}

    texTabler makeSystBreakdown(bool merge=true,bool removeneg=true,float prec=0.01,bool rel=false,bool includesystat=false)const;

    void drawSpreadWithInlay(TCanvas *c);

    void setup(); //just  runs private functions

    void reset();//resets all calcs done, not inputfiles/plots etc.

    static bool debug;

    void cleanMem();

    TString printConfig()const;

    std::vector<float> getMtValues();
    const TString & getDefMtopDataFile()
    {
        if(mtvals_.size()<1) getMtValues();
        return cufinputfiles_.at(defmtidx_);
    }

    void setBinGraphOutFile(TFile * f){mcgraphsoutfile_=f;}

    const std::vector<graph > & getDataBingraphs(){return databingraphs_;}
    const std::vector<graph > & getMCBingraphs(){return mcbingraphs_;}

    /**
     * This returns a graph without systematics
     */
    std::vector<graph > makeBGBinGraphs()const;
    /**
     * This returns a graph without systematics
     */
    std::vector<graph > makeSignalBinGraphs()const;

private:

    void setAxisLikelihoodVsMt(graph & g)const;
    void setAxisXsecVsMt(graph & g)const;
    void setAxisXsecRatio(graph & g)const;

    double getNewNorm(double deltam,bool eighttev=true)const;

    //init and read in

    void readFiles(); //type specific stuff here background uncertainties are added here!
    void addLumiUncert(std::vector<container1D> & mc,std::vector<container1D> & data)const;
    void renormalize();
    void mergeSyst(std::vector<container1D> & a, std::vector<container1D> & b)const;
    std::vector<graph > makeGraphs( std::vector<container1D > & in)const; //at this step the systematics should be ordered in the same way (use first and copy!),
    // delete input conts

    //helper
    graph makeDepInBin(const std::vector<graph> &, size_t)const;

    //---a plotting step?---//
    // here for each mt a fully equipped distr is present
    std::vector<graph >  makeBinGraphs(std::vector<graph > &)const; //still including all systematics use getBin()

    bool isEmptyForAnyMass(size_t bin)const;


    //same ordering as mtvals
    // std::vector< containerStackVector> allanalysisplots_;

    TString plotnamedata_;
    TString plotnamemc_; //ff
    TString plottypemc_; //ff
    int minbin_;
    int maxbin_;
    int excludebin_;
    std::vector<TString> cufinputfiles_;
    std::vector<TString> extgenfiles_;
    TString extfileformatfile_,extfilepdf_;
    TString extfilepreamble_;

    std::vector<containerStack> savedinputstacks_;
    std::vector<container1D > datacont_;
    std::vector<container1D > mccont_;
    std::vector<graph > datagraphs_;
    std::vector<graph > mcgraphs_;
    std::vector<float> mtvals_;


    std::vector<graph > databingraphs_;
    std::vector<graph > mcbingraphs_;

    TString tmpSysName_;
    std::vector<graph > tmpbinlhds_;
    std::vector<std::vector<TF1* > > tmpbinfitsdata_,tmpbinfitsmc_;
    graph               tmpgllhd_;
    TGraphAsymmErrors * tmpglgraph_;
    TF1 * tfitf_;

    bool iseighttev_;
    float defmtop_;
    bool setup_;

    std::string compplotsstylefilemc_,compplotsstylefiledata_;
    std::string binsplotsstylefile_;
    std::string binschi2plotsstylefile_;
    std::string binsplusfitsstylefile_;
    std::string chi2plotsstylefile_;
    std::string allsyststylefile_;
    std::string sysvariationsfile_;
    std::string spreadwithinlaystylefile_;
    std::vector<plotterBase *> pltrptrs_;
    std::string textboxesfile_,textboxesmarker_;


    graph globalnominal_;

    parameterExtractor paraExtr_;

    graph allsyst_,allsystsl_,allsystsh_;
    float syspidx_;

    TString fitmode_;
    bool dofolding_;
    bool isexternalgen_;

    std::vector<float> predrescalers_;
    bool rescalepreds_;

    bool usenormalized_;

    size_t defmtidx_;

    resultsSummary results_;

    TFile * mcgraphsoutfile_;

    std::vector<TString> systidentifiers_tobeaddedWithoutstat;

    bool ignoredatastat_,ignorebgstat_;

    template<class T>
    T tryToGet(TFile * f,const TString& name)const{
        T  * h= (T  *)f->Get(name);
        if(!h || h->IsZombie()){
            std::cout << "mtExtractor::tryToGet " << name << " - not found in "<< f->GetName() <<std::endl;
            throw std::runtime_error("mtExtractor::tryToGet: Histogram not found " );
        }
        T out=*h;
        delete h;
        return out;
    }

};

}//ns



#endif /* MTEXTRACTOR_H_ */
