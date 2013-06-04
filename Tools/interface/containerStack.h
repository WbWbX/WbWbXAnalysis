#ifndef containerStack_h
#define containerStack_h

#include "container.h"
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

namespace ztop{

  class container1DStack{
    
  public:
    container1DStack();
    container1DStack(TString);
    ~container1DStack();

    void push_back(ztop::container1D, TString, int, double); //! adds container with, name, colour, norm to stack
    void removeContribution(TString); //! removes contribution
    
    void setDataLegend(TString leg="data"){dataleg_=leg;}

    void setLegendOrder(TString leg, size_t no){legorder_[leg]=no;}

    void mergeSameLegends();       //! shouldn't be necessary
    ztop::container1D getContribution(TString);   //! does not ignore data; makes copy, doesn't change member containers!
    ztop::container1D getContributionsBut(TString);  //!does not ignore data; makes copy, doesn't change member containers!
    ztop::container1D getContributionsBut(std::vector<TString>);  //!does not ignore data; makes copy, doesn't change member containers!

    TString getName(){return name_;}
    unsigned int size(){return colors_.size();}
    TString & getLegend(unsigned int i){return legends_[i];}
    int & getColor (unsigned int i){return colors_[i];};
    double & getNorm  (unsigned int i){return norms_[i];}
    container1D & getContainer(unsigned int i){return containers_[i];}
    container1D & getContainer(TString);
    container1D getFullMCContainer();           //! gets the sum of all MC containers (normalized with their stored norm) including error handling
    void multiplyNorm(TString , double);
    void multiplyAllMCNorms(double);
    void addGlobalRelMCError(TString,double);   //! adds a global systematic variation to the systematics stored (e.g. lumi)
    void addMCErrorStack(TString,container1DStack,bool ignoreMCStat=true);  //! calls container1D::addErrorContainer for each same named member container
    void addRelSystematicsFrom(ztop::container1DStack);
    void removeError(TString);
    void renameSyst(TString, TString); //! old, new

    void clear(){containers_.clear();legends_.clear();colors_.clear();norms_.clear();}
    
    void setName(TString name){name_=name;}
    
    THStack * makeTHStack(TString stackname = "");
    TLegend * makeTLegend(bool inverse=true);
    void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1); // the extra axis is in both... sorry for that!
    TGraphAsymmErrors * makeMCErrors();
    void drawRatioPlot(TString name="",double resizelabels=1);

    TCanvas * makeTCanvas(bool drawratioplot=true);


    //define operators (propagate from containers) for easy handling

    container1DStack operator + ( container1DStack );
    container1DStack operator - ( container1DStack );
    container1DStack operator / ( container1DStack );
    container1DStack operator * ( container1DStack );
    container1DStack operator * (double);
    container1DStack operator * (float);
    container1DStack operator * (int);
    
    static std::vector<ztop::container1DStack*> cs_list;
    static bool cs_makelist;
    static void cs_clearlist(){cs_list.clear();}

    static bool batchmode;

  private:
    TString name_;
    std::vector<ztop::container1D> containers_;
    std::vector<TString> legends_;
    std::vector<int> colors_;
    std::vector<double> norms_;
    TString dataleg_;

    std::map<TString,size_t> legorder_;
    
    int checkLegOrder();

    std::vector<size_t> sortEntries(bool inverse);

};
  

}
#endif
