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


class containerStackVector:public taggedObject{

public:
    containerStackVector();
    containerStackVector(TString);
    ~containerStackVector();

    void setSyst(TString sys){isSyst_=sys;}
    const TString& getSyst()const{return isSyst_;}

    void listStacks();
    std::vector<TString> getStackNames(bool);
    void setLegendOrder(TString , size_t); //do it afterwards

    // void add(ztop::container1D &, TString, int, double);
    void add(ztop::container1D &, TString, int, double,int legor=-1); //+legorder
    void add(ztop::container2D &, TString, int, double,int legor=-1); //+legorder
    void add(ztop::container1DUnfold &, TString, int, double,int legor=-1); //+legorder

    void addList( TString, int, double,int legor=-1); //+legorder
    void addList2D( TString, int, double,int legor=-1);
    void addList1DUnfold( TString, int, double,int legor=-1);

    void addSignal(const TString &);

    const std::vector<ztop::containerStack>& getVector()const{return stacks_;}
    std::vector<ztop::containerStack>& getVector(){return stacks_;}
    const ztop::containerStack & getStack(const TString&)const;
    ztop::containerStack & getStack(const TString&);
    const ztop::containerStack & getStack(unsigned int n)const{return stacks_.at(n);}
    ztop::containerStack & getStack(unsigned int n){return stacks_.at(n);}
    void removeContribution(TString);

    void addMCErrorStackVector(const TString&,const ztop::containerStackVector&);
    void addErrorStackVector(const TString&,const ztop::containerStackVector&);
    void addMCErrorStackVector(const ztop::containerStackVector&);
    void addGlobalRelMCError(TString,double);
    void getRelSystematicsFrom(const ztop::containerStackVector&);
    void addRelSystematicsFrom(const ztop::containerStackVector&,bool ignorestat=false,bool strict=false);
    void removeError(TString);
    void renameSyst(TString,TString); //! old new

    size_t getNSyst()const{if(stacks_.size()>0)return stacks_.at(0).getSystSize(); else return 0;}

    std::vector<size_t> removeSpikes(bool inclUFOF=false,int limittoindex=-1,float strength=100000,float sign=0.3,float threshold=8);

    void multiplyNorm(TString, double, TString); //! legendname, multiplier, stepID
    void multiplyNorms(TString, std::vector<double>, std::vector<TString>, bool showmessages=false);   //! multiplies norm of all MC with legendname  ,  with factor  ,  for step identifier string (step_string contains the id string)
    void multiplyAllMCNorms(double);

    static int treesplitlevel;

    void writeAllToTFile(TString, bool recreate=false, bool onlydata=false, TString treename="containerStackVectors");
    void printAll(TString,TString,TString);
    void writeAllToTFile(TFile * , TString treename="containerStackVectors");
    void loadFromTree(TTree * t,const TString& csvname="*");
    containerStackVector* getFromTree(TTree *t ,const TString& csvname="*");
    void listAllInTree(TTree *);

    void loadFromTFile(TFile * ,const TString& csvname="*", TString treename="containerStackVectors");

    void loadFromTFile(const TString& filename ,const TString& csvname="*", TString treename="containerStackVectors");

    size_t size(){return stacks_.size();}

    void clear(){stacks_.clear();name_="";}
    static std::vector<containerStackVector*> csv_list;
    static bool csv_makelist;
    static void csv_clearlist(){csv_list.clear();}
    static bool debug;

    static bool fastadd;

private:
    TString name_;
    TString isSyst_;
    std::vector<ztop::containerStack> stacks_;
    TString tempsig_;


};


typedef  containerStackVector container1DStackVector;

}


#endif
