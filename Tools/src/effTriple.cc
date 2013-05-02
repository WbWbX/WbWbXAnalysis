#include "../interface/effTriple.h"

////////////////////CUT//////////////////


namespace ztop{

  std::vector<effTriple *> effTriple::effTriple_list;
  bool effTriple::makelist=false;


  void effTriple::addToList(){
    if(makelist)
      effTriple_list.push_back(this);
  }

  void effTriple::check1d(){
    if(!hists_.at(0).isTH1D()){
      std::cout << "effTriple::XX: 1D Histograms not initialized! exit!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  void effTriple::check2d(){
    if(!hists_.at(0).isTH2D()){
      std::cout << "effTriple::XX: 2D Histograms not initialized! exit!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  void effTriple::makeEff(){
    if(hists_.size() < 2)
      std::exit(EXIT_FAILURE); //should not happen because default constructor is disabled

    histWrapper eff=hists_.at(0) / hists_.at(1);
    eff.setName(name_+"_eff");
    eff.setFormatInfo(hists_.at(0).getFormatInfo());
    eff.setDivideBinomial(false);
    if(hists_.size() < 3)
      hists_.push_back(eff);
    else
      hists_.at(2) = eff;
    hists_.at(2).setName(name_+"_eff");
  }


  ///public functions

  effTriple::effTriple(){
    std::cout << "effTriple::effTriple(): default constructor not supported (yet), exit" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  effTriple::effTriple(std::vector<float> binsx,  TString name,TString xaxisname,TString yaxisname, TString formatinfo, bool mergeufof, size_t size){
    if(size>3){
      std::cout << "effTriple:size>3 not supported yet!" << std::endl;
      exit(EXIT_FAILURE);
    }
    name_=name;
    TH1::AddDirectory(kFALSE); 
    double binxarray[binsx.size()];
    for(size_t i=0;i<binsx.size();i++){
      binxarray[i] = binsx[i];
    }
    for(size_t i=0;i<size;i++){
      TString add;
      if(i==0) add="_num";
      else if(i==(size_t)1) add="_den";
      else if(i==(size_t)2) add="_eff";
      TH1D h=TH1D(name+add,
		  name+add,
		  binsx.size()-1,
		  binxarray);
      h.GetXaxis()->SetTitle(xaxisname);
      h.GetYaxis()->SetTitle(yaxisname);
      setStyle(h);
      // h.Sumw2();
      histWrapper temp(h,name+add);
      temp.setFormatInfo(formatinfo);
      hists_.push_back(temp);
    }
    bool justAPlaceholderIgnoreWarning=mergeufof; //to avoid warning !IMPLEMENT! later (or remove switch) 
    addToList();
  }

  effTriple::effTriple(std::vector<float> binsx,  std::vector<float> binsy, TString name,TString xaxisname,TString yaxisname,  TString formatinfo, bool mergeufof, size_t size){
    if(size>3){
      std::cout << "effTriple:size>3 not supported yet!" << std::endl;
      exit(EXIT_FAILURE);
    }
    name_=name;
    TH1::AddDirectory(kFALSE); 
    double binxarray[binsx.size()];
    for(size_t i=0;i<binsx.size();i++){
      binxarray[i] = binsx[i];
    }
   
    double binyarray[binsy.size()];
    for(size_t i=0;i<binsy.size();i++){
      binyarray[i] = binsy[i];
    }
    
    for(size_t i=0;i<size;i++){
      TString add;
      if(i==0) add="_num";
      else if(i==(size_t)1) add="_den";
      else if(i==(size_t)2) add="_eff";
      TH2D h=TH2D(name+add,
		  name+add,
		  binsx.size()-1,
		  binxarray,
		  binsy.size()-1,
		  binyarray);
      h.GetXaxis()->SetTitle(xaxisname);
      h.GetYaxis()->SetTitle(yaxisname);
      setStyle2d(h);
      // h.Sumw2();
      histWrapper temp(h,name+add);
      temp.setFormatInfo(formatinfo);
      hists_.push_back(temp);
    }
    bool justAPlaceholderIgnoreWarning=mergeufof; //to avoid warning !IMPLEMENT! later (or remove switch)
    addToList();
  }

  effTriple::~effTriple(){
    for(size_t i=0;i<effTriple_list.size();i++){
      if(effTriple_list[i] == this){
	effTriple_list.erase(effTriple_list.begin()+i);
	break;
      }
    }
  }

  void effTriple::fillNum(double value, double val2weight, double weight){
    hists_.at(0).fill(value, val2weight, weight);
  }

  void effTriple::fillDen(double value,double  val2weight, double weight){
    hists_.at(1).fill(value, val2weight, weight);
  }




} //namespace


