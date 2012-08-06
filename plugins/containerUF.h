#ifndef containerUF_h
#define containerUF_h

#include "containerStack.h"
#include "TString.h"

// container with some more plotting options and unfolding

namespace top{

  class container1DUF : public container1D{

  public:

    container1DUF();
    container1DUF(const top::container1DStack &, const top::container1DStack &, TString signalname, TString dataname="data");
    container1DUF(const top::container1D &);
    ~container1D(){};

    void setGenStack(const top::container1DStack &gen){gen_=gen;}
    void setSelectedStack(const top::container1DStack &sel){sel_=sel;}
    void setSignal(TString signalname){signalname_=signalname}
    void setDataName(TString name){dataname_=name;}

    void simpleUnfold();

    top::container1D breakDownSystematicsInBin(int);


    //some plotting stuff - still missing



  private:
    top::container1DStack gen_;
    top::container1DStack sel_;
    TString dataname_;
    TString signalname_;

  };


  //functions split here if you like

  container1DUF::container1DUF(const top::container1DStack &gen, const top::container1DStack &sel, TString signalname, TString dataname){
    gen_=gen;
    sel_=sel;
    dataname_=dataname;
    signalname_=signalname;
    simpleUnfold();
  }

  container1DUF::container1DUF(const top::container1D & cont){ //convert
    showwarnings_=cont.showwarnings_;
    binwidth_=cont.binwidth_;
    bins_=cont.bins_;
    canfilldyn_=cont.canfilldyn_;
    content_=cont.content_;
    entries_=cont.entries_;
    staterrup_=cont.staterrup_;
    staterrdown_=cont.staterrdown_;
    syserrors_=cont.syserrors_;
    mergeufof_=cont.mergeufof_;
    wasunderflow_=cont.wasunderflow_;
    wasoverflow_=cont.wasoverflow_;
    divideBinomial_=cont.divideBinomial_;
    name_=cont.name_;
    xname_=cont.xname_;
    yname_=cont.yname_;
    labelmultiplier_=cont.labelmultiplier_;
  }

  void container1DUF::simpleUnfold(){

    // find signal in gen and sel
    top::container1D gencont=gen_.getContribution(signalname_);
    top::container1D selcont=sel_.getContribution(signalname_);
   
    // get efficiency and background
    std::vector<TString> excludefrombackground;
    excludefrombackground << signalname_ << dataname_;

    top::container1D efficiencyinv = gencont / selcont;
    top::container1D background = sel_.getContributionsBut(excludefrombackground);

    //get data
    top::container1D data=sel_.getContribution(dataname_);

    // and do the "unfolding"
    top::container1D output = (data - background) * efficiencyinv;

    *this = container1DUF(output);

  }
  top::container1D container1DUF::breakDownSystematicsInBin(int bin){
    top::container1D out;


    return out;
  }

}

#endif
