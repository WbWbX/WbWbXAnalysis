#ifndef containerUF_h
#define containerUF_h

#include "containerStack.h"
#include "TString.h"

// add function to make a tex table of the contents (syst)

// container with some more plotting options and (very simple) unfolding

// make containerUFStack class inherits from normal stack?!? (check protected,private etc. such that all functions on member containers_ now apply for a vector of containerUF.. might be tricky / or not? in case of doubt containers_ ->private

//include Alan Caldwells stat error interpretation?? -> check with someone, how they feel about it.

namespace top{

  class container1DUF : public container1D{

  public:

    container1DUF(){};
    container1DUF(const top::container1DStack &, const top::container1DStack &, TString , double , TString dataname="data");
    container1DUF(const top::container1D &);
    ~container1DUF(){};

    bool nameContains(TString);
    bool nameContains(std::vector<TString>);

    void setGenStack(const top::container1DStack &gen){gen_=gen;}
    void setSelectedStack(const top::container1DStack &sel){sel_=sel;}
    void setSignal(TString signalname){signalname_=signalname;}
    void setDataName(TString name){dataname_=name;}

    void simpleUnfold();

    void setBinName(int, TString );

    void coutBinContents();


    top::container1DUF breakDownRelSystematicsInBin(int);


    //some plotting stuff - still missing

    TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool noXErrors=false);


  private:
    top::container1DStack gen_;
    top::container1DStack sel_;
    double lumi_;
    TString dataname_;
    TString signalname_;
    std::vector<TString> binnames_;

  };


  //functions split here if you like

  container1DUF::container1DUF(const top::container1DStack &gen, const top::container1DStack &sel, TString signalname, double lumi, TString dataname){
    gen_=gen;
    sel_=sel;
    dataname_=dataname;
    signalname_=signalname;
    lumi_=lumi;
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


  bool container1DUF::nameContains(TString somestring){
    if(name_.Contains(somestring))
      return true;
    else
      return false;
  }
  bool container1DUF::nameContains(std::vector<TString> somestrings){
    for(unsigned int i=0;i<somestrings.size();i++){
      if(!nameContains(somestrings.at(i) ) ){
	return false;
      }
    }
    return true;
  }

  void container1DUF::simpleUnfold(){

    // find signal in gen and sel
    bool listing=top::container1D::c_makelist; //protection agains unwanted listing of intermediate containers
    top::container1D::c_makelist=false;

    top::container1D gencont=gen_.getContribution(signalname_);
    top::container1D selcont=sel_.getContribution(signalname_);
   
    // get efficiency and background
    std::vector<TString> excludefrombackground;
    excludefrombackground << signalname_ << dataname_;

    for(unsigned int i=0;i<excludefrombackground.size();i++){
      cout << excludefrombackground.at(i) << " " ;
    }
    cout << endl;

    //   gencont.setDivideBinomial(false);
    // selcont.setDivideBinomial(false);
    // gencont.getTH1D()->Draw();


    // binomial error for efficiency

    cout << "gen in bin 1: " << gencont.getBinContent(1) << endl;
    cout << "genandsel in bin 1: " << selcont.getBinContent(1) << endl;

    top::container1D efficiency = selcont / gencont;

    cout << "eff in bin 1: "<< efficiency.getBinContent(1) << endl;

    top::container1D background = sel_.getContributionsBut(excludefrombackground);

    cout << "bg in bin 1: " << background.getBinContent(1) << endl;
    
    // efficiency.getTH1D()->Draw();

    //get data
    top::container1D data=sel_.getContribution(dataname_);

    cout << "data in bin 1: " << data.getBinContent(1) << endl;

    top::container1D select = (data - background);
    // select.getTH1D()->Draw();

    cout << "sel signal in bin 1: " << select.getBinContent(1) << endl;

    // and do the "unfolding"
    data.setDivideBinomial(false);
    background.setDivideBinomial(false);
    efficiency.setDivideBinomial(false);

    efficiency.transformStatToSyst("MC_stat"); //when using multiple of these don't forget to rename!!!

    top::container1D output = (data - background) / efficiency;
      output=output * (1/lumi_);

      cout << "xsec in bin 1: " << output.getBinContent(1) << endl;

      //  output.getTH1D()->Draw();

    *this = container1DUF(output);
    top::container1D::c_makelist=listing;

  }
  
  void container1DUF::setBinName(int bin, TString name){
    if(binnames_.size() ==0){
      for(unsigned int i=0;i<bins_.size();i++) binnames_ << "";
    }
    binnames_.at(bin) = name;
  }
  


  top::container1DUF container1DUF::breakDownRelSystematicsInBin(int bin){ //maybe add total syst

    std::vector<float> newbins;
    for(float i=0;i<=syserrors_.size();i++){ //+1 for total bin
      newbins << i;
    }

    container1DUF syscont(newbins);


    if(bin < 0 || (size_t)bin >= bins_.size()){
      std::cout << "top::container1DUF::breakDownSystematicsInBin: bin not in range!" << std::endl;
    }
    else{
      //    double totalup=0;
      //   double totaldown=0;
      for(unsigned int i=0;i<syserrors_.size();i++){
	int outputbin = syscont.getBinNo(i+0.5);
	syscont.setBinContent(outputbin, 1);
	syscont.setBinName(outputbin, syserrors_.at(i).first);

	if(syserrors_.at(i).second.at(bin) > 0){
	  syscont.setBinErrorUp(outputbin, syserrors_.at(i).second.at(bin)/fabs(content_.at(bin)));
	  //	  totalup+=pow(syserrors_.at(i).second.at(bin)/fabs(content_.at(bin)),2);
	}
	else{
	  syscont.setBinErrorDown(outputbin, syserrors_.at(i).second.at(bin)/fabs(content_.at(bin)));
	  //	  totaldown+=pow(syserrors_.at(i).second.at(bin)/fabs(content_.at(bin)),2);
	}
      }
    }
    return syscont;
  }

  void container1DUF::coutBinContents(){

    for(unsigned int i=0;i<bins_.size()-1;i++){
      //bin names and ranges
      if(i==0) std::cout << "UF\t";
      else if(binnames_.size()>0 && binnames_.at(i) !="") std::cout << binnames_.at(i) <<'\t';
      else std::cout << bins_.at(i) << "-" << bins_.at(i+1) <<'\t';

      //bin content
      std::cout << content_.at(i) << "\t( +" << staterrup_.at(i) << "\t " << staterrdown_.at(i) << " (stat.))";
      if(syserrors_.size()>0){
	std::cout << "\t+" << getBinErrorUp(i) << "\t -" << getBinErrorDown(i) << " \t(total)";
      }
      std::cout << std::endl;
    }
    if(bins_.size()>0){ //and the missing overflow bin
      std::cout << "OF\t"
		<< content_.at(bins_.size()-1) << "\t( +" << staterrup_.at(bins_.size()-1) 
		<< "\t " << staterrdown_.at(bins_.size()-1) << " (stat.))";
      if(syserrors_.size()>0){
	std::cout << "\t+" << getBinErrorUp(bins_.size()-1) << "\t -" << getBinErrorDown(bins_.size()-1) << " \t(total)";
	std::cout << std::endl;
      }
    }
  }

  //..setBinLabel(bin,label)


  TGraphAsymmErrors * container1DUF::getTGraph(TString name, bool dividebybinwidth,bool noXErrors){
    TGraphAsymmErrors * g=container1D::getTGraph(name, dividebybinwidth, noXErrors);

    if(binnames_.size()>0){

      float binsarr[(size_t)bins_.size()-1];
      for(unsigned int i=1;i<bins_.size();i++){ binsarr[i-1]=bins_.at(i); std::cout << bins_.at(i) << "  " << binsarr[i-1] << std::endl; }
      std::cout << bins_.size()-2 << std::endl;
      g->GetXaxis()->Set((int)bins_.size()-2, (Float_t*)binsarr);

      for(unsigned int i=1;i<bins_.size()-1;i++){
	//	int graphbin=g->GetXaxis()->FindBin(i);
		g->GetXaxis()->SetBinLabel(i,binnames_.at(i));
      }
      g->GetXaxis()->SetRangeUser(bins_.at(1),bins_.at(bins_.size()-1));
    }
    return g;
  }

}

#endif
