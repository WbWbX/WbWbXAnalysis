#ifndef containerStack_h
#define containerStack_h

#include "container.h"
#include "container2D.h"
#include "container1DUnfold.h"
#include <iostream>
#include "TString.h"
#include <vector>
#include "THStack.h"
#include "TLegend.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TGaxis.h"
#include <map>
#include "tObjectList.h"
#include <map>


namespace ztop{
class plotterControlPlot;

class containerStack: public tObjectList,public taggedObject{
    friend class plotterControlPlot;
public:


    enum plotmode{normal,ratio,sigbg};

    containerStack();
    containerStack(TString);
    ~containerStack();

    bool is2D()const{if(mode==dim2) return true; else return false;}
    bool is1D()const{if(mode==dim1) return true; else return false;}
    bool is1DUnfold()const{if(mode==unfolddim1) return true; else return false;}

    void push_back(ztop::container1D, TString, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
    void push_back(ztop::container2D, TString, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
    void push_back(ztop::container1DUnfold, TString, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
    void removeContribution(TString); //! removes contribution

    void setDataLegend(TString leg="data"){dataleg_=leg;}

    void setLegendOrder(const TString &leg, const size_t& no);

    void mergeSameLegends();       //! shouldn't be necessary
    ztop::container1D getContribution(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
    ztop::container1D getContributionsBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
    ztop::container1D getContributionsBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

    ztop::container2D getContribution2D(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
    ztop::container2D getContributions2DBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
    ztop::container2D getContributions2DBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

    ztop::container1DUnfold getContribution1DUnfold(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
    ztop::container1DUnfold getContributions1DUnfoldBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
    ztop::container1DUnfold getContributions1DUnfoldBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

    const TString & getName() const{return name_;}
    size_t size() const{return colors_.size();}
    TString & getLegend(unsigned int i){return legends_.at(i);}
    const TString & getLegend(unsigned int i)const{return legends_.at(i);}
    int & getColor (unsigned int i){return colors_.at(i);};
    const int & getColor (unsigned int i)const {return colors_.at(i);};
   // double & getNorm  (unsigned int i){return norms_.at(i);}
    const double & getNorm  (unsigned int i)const{return norms_.at(i);}

    container1D & getContainer(unsigned int i){return containers_.at(i);}
    const container1D & getContainer(unsigned int i)const{return containers_.at(i);}
    container1D & getContainer(TString);
    container1D  getContainer(TString) const;
    container1D getFullMCContainer()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

    container2D & getContainer2D(unsigned int i){return containers2D_.at(i);}
    const container2D & getContainer2D(unsigned int i)const{return containers2D_.at(i);}
    container2D & getContainer2D(TString);
    container2D  getContainer2D(TString)const;
    container2D getFullMCContainer2D()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

    container1DUnfold & getContainer1DUnfold(unsigned int i){return containers1DUnfold_.at(i);}
    const container1DUnfold & getContainer1DUnfold(unsigned int i)const{return containers1DUnfold_.at(i);}
    container1DUnfold & getContainer1DUnfold(TString);
    container1DUnfold  getContainer1DUnfold(TString)const;
    container1DUnfold getFullMCContainer1DUnfold()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

    void multiplyNorm(TString , double);
    void multiplyAllMCNorms(double);
    void addGlobalRelMCError(TString,double);   //! adds a global systematic variation to the systematics stored (e.g. lumi)
    void addMCErrorStack(const TString&,const containerStack &) ; //only for comp reasons
    void addErrorStack(const TString& , containerStack ); //! calls container1D::addErrorContainer for each same named member container
    void getRelSystematicsFrom(ztop::containerStack);
    void removeError(TString);
    void renameSyst(TString, TString); //! old, new

    void clear(){containers_.clear();legends_.clear();colors_.clear();norms_.clear();}

    void setName(TString name){name_=name;}

    THStack * makeTHStack(TString stackname = "");
    TLegend * makeTLegend(bool inverse=true);
    void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1); // the extra axis is in both... sorry for that!
    TGraphAsymmErrors * makeMCErrors();
    void drawRatioPlot(TString name="",double resizelabels=1);
    std::vector<container1D>  getPEffPlots(size_t idx)const;
    void drawPEffPlot(TString name="",double resizelabels=1);
    void drawSBGPlot(TString name="",double resizelabels=1,bool invert=false);

    //TCanvas * makeTCanvas(bool drawratioplot=true);
    TCanvas * makeTCanvas(plotmode plotMode=ratio);

    std::map<TString, float>  getAllContentsInBin(size_t bin,bool print=false)const;

    containerStack rebinXToBinning(const std::vector<float> &)const;
    containerStack rebinYToBinning(const std::vector<float> &)const;


    containerStack operator + ( containerStack );
    containerStack operator - ( containerStack );
    containerStack operator / ( containerStack );
    containerStack operator * ( containerStack );
    containerStack operator * (double);
    containerStack operator * (float);
    containerStack operator * (int);

    bool setsignal(const TString&);
    bool addSignal(const TString&);
    bool setsignals(const std::vector<TString> &);// (just return whether successful)
    std::vector<size_t> getSignalIdxs() const;
    std::vector<size_t> getSortedIdxs(bool inverse) const;

    container1D getSignalContainer()const;
    container1D getBackgroundContainer()const;


    container1DUnfold produceUnfoldingContainer(const std::vector<TString> &sign=std::vector<TString>()) const;// if sign empty use "setsignal"


    /*
       ad all signal -> cuf
       add all recocont_ w/o signaldef -> set BG of cuf
       (add all) with datadef -> setData in cuf
     */
    container1DUnfold produceXCheckUnfoldingContainer(const std::vector<TString> & sign=std::vector<TString>()) const; /* if sign empty use "setsignal"
       just adds all with signaldef (to be then unfolded and a comparison plot to be made gen vs unfolded gen) -> SWITCH OFF SYSTEMATICS HERE! (should be independent)

     */
    //define operators (propagate from containers) for easy handling
    // also define += etc. and use container += if better, needs to be implemented (later)

    container1DUnfold getSignalContainer1DUnfold(const std::vector<TString> &sign=std::vector<TString>()) const;

    /**
     * Function that just retunrs a formatted string listing all contributions
     * cout the output to print them
     */
    TString listContributions()const;

    static std::vector<ztop::containerStack*> cs_list;
    static bool cs_makelist;
    static void cs_clearlist(){cs_list.clear();}
    static bool debug;

    static bool batchmode;

    /**
     * If true all legends are required to be found in both stacks when adding errors
     */
    static bool addErrorsStrict;

    //debugging functions
    bool checknorms() const;

private:
    TString name_;
    std::vector<ztop::container1D> containers_;
    std::vector<ztop::container2D> containers2D_;
    std::vector<ztop::container1DUnfold> containers1DUnfold_;
    std::vector<TString> legends_;
    std::vector<int> colors_;
    std::vector<double> norms_;
    TString dataleg_;

    enum dimension{notset,dim1,dim2,unfolddim1};
    dimension mode;


    std::vector<int> legorder_;

    int checkLegOrder() const;


    std::vector<TString> signals_;


    int getContributionIdx(TString) const;
    bool checkDrawDimension() const;
    bool checkSignals(const std::vector<TString> &) const;



};



typedef containerStack container1DStack;

}
#endif
