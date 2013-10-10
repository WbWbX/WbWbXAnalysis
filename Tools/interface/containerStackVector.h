#ifndef containerStackVector_h
#define containerStackVector_h

#include "containerStack.h"
//#include <vector>
#include "TStyle.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"

// class to handle containerstacks and to add systematics to the MC contributions etc.
// writes plots with and without ratio to TFiles


namespace ztop{


  class containerStackVector{

  public:
    containerStackVector();
    containerStackVector(TString);
    ~containerStackVector();
    void setName(TString Name){name_=Name;}
    TString getName(){return name_;}
    void setSyst(TString sys){isSyst_=sys;}
    TString getSyst(){return isSyst_;}

    void listStacks();
    void setLegendOrder(TString , size_t); //do it afterwards

    // void add(ztop::container1D &, TString, int, double);
    void add(ztop::container1D &, TString, int, double,int legor=-1); //+legorder
    void add(ztop::container2D &, TString, int, double,int legor=-1); //+legorder
    void add(ztop::container1DUnfold &, TString, int, double,int legor=-1); //+legorder

    void addList( TString, int, double,int legor=-1); //+legorder
    void addList2D( TString, int, double,int legor=-1);
    void addList1DUnfold( TString, int, double,int legor=-1);

    void addSignal(const TString &);

    std::vector<ztop::containerStack> getVector(){return stacks_;}
    ztop::containerStack getStack(TString);
    ztop::containerStack getStack(unsigned int n){return stacks_[n];}
    void removeContribution(TString);

    void addMCErrorStackVector(TString,ztop::containerStackVector);
    void addMCErrorStackVector(ztop::containerStackVector);
    void addGlobalRelMCError(TString,double);
    void addRelSystematicsFrom(ztop::containerStackVector);
    void removeError(TString);
    void renameSyst(TString,TString); //! old new

    void multiplyNorm(TString, double, TString); //! legendname, multiplier, stepID
    void multiplyNorms(TString, std::vector<double>, std::vector<TString>, bool showmessages=false);   //! multiplies norm of all MC with legendname  ,  with factor  ,  for step identifier string (step_string contains the id string)
    void multiplyAllMCNorms(double);

    void writeAllToTFile(TString, bool recreate=false, TString treename="stored_objects");
    void writeAllToTFile(TFile * , TString treename="stored_objects");
    void loadFromTree(TTree *, TString);
    containerStackVector* getFromTree(TTree *, TString);
    void listAllInTree(TTree *);

    void clear(){stacks_.clear();name_="";}
    static std::vector<containerStackVector*> csv_list;
    static bool csv_makelist;
    static void csv_clearlist(){csv_list.clear();}
    static bool debug;

  private:
    TString name_;
    TString isSyst_;
    std::vector<ztop::containerStack> stacks_;
    TString tempsig_;


  };


  typedef  containerStackVector container1DStackVector;
}


#endif
