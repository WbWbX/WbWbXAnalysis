#include "TtZAnalysis/Tools/interface/bTagBase.h"

namespace top{

  bTagBase::bTagBase(){
    wp_=0.244;
    makeeffs_=true;
    histp_=0;
    effhistp_=0;
    syst_=0;
    is2011_=false;
    cbflatineta_=true;
  }
  void bTagBase::setSampleName(const std::string & samplename){
    //set pointers
    std::map<std::string,std::vector<TH2D> >::iterator sampleit=histos_.find(samplename);
    if(sampleit!=histos_.end())
      histp_ = & (sampleit->second);
    else
      histp_ = 0;
    std::map<std::string,std::vector<TH2D> >::iterator effit=effhistos_.find(samplename);
    if(effit!=effhistos_.end())
      effhistp_ = & (effit->second);
    else
      effhistp_ = 0;

    if(!makeeffs_ && effhistp_ && histp_){
      std::cout << "loaded b-tag efficiency histos for " << samplename << std::endl;
      return;
    }
    else if(!makeeffs_ &&!effhistp_){
      std::cout << " bTagBase::setSampleName: efficiency for " << samplename << " not derived, yet! exit." << std::endl;
      std::exit(EXIT_FAILURE);
    }

    // efficiencies not determined yet -> prepare efficiency histos
    // bool adddirtemp=TH1::AddDirectory;
    TH1::AddDirectory(kFALSE); //prevent some weird root behaviour
    std::cout << "preparing b-tag efficiency histos for " << samplename << std::endl;

    //////// put in algorithm to automatically rebin in case of low statistics !! then use a fine (equals btv binning) binning

    double effptbins[] = {30.,50.,70.,100.,160.,210.,670.};
    unsigned int npt=7;
    double effetabins[] ={0.0,0.5,1.0,2.5};
    unsigned int neta=4;

    //probably there will be less statistics for the light jets, so histos might need a coarser binning

    double l_effptbins[] = {30.,70.,120.,670.};
    unsigned int l_npt=4;
    double l_effetabins[] ={0.0,1.5,3.0};
    unsigned int l_neta=3;

    TH2D bjets          = TH2D((TString)("bjets2D_"      +samplename) , "unTagged Bjets", npt-1, effptbins, neta-1, effetabins);    bjets.Sumw2();
    TH2D bjetstagged    = TH2D((TString)("bjetsTagged2D_"+samplename) , "Tagged Bjets",   npt-1, effptbins, neta-1, effetabins);    bjetstagged.Sumw2(); 
    TH2D cjets          = TH2D((TString)("cjets2D_"      +samplename) , "unTagged Cjets", npt-1, effptbins, neta-1, effetabins);    cjets.Sumw2();
    TH2D cjetstagged    = TH2D((TString)("cjetsTagged2D_"+samplename) , "Tagged Cjets",   npt-1, effptbins, neta-1, effetabins);    cjetstagged.Sumw2(); 
    TH2D ljets          = TH2D((TString)("ljets2D_"      +samplename) , "unTagged Ljets", l_npt-1, l_effptbins, l_neta-1, l_effetabins);    ljets.Sumw2();
    TH2D ljetstagged    = TH2D((TString)("ljetsTagged2D_"+samplename) , "Tagged Ljets",   l_npt-1, l_effptbins, l_neta-1, l_effetabins);    ljetstagged.Sumw2();
      
    std::vector<TH2D> temp;
    temp << bjets << bjetstagged << cjets << cjetstagged << ljets << ljetstagged;
    histos_[samplename]=temp;
    histp_ = &(histos_.find(samplename)->second);
    
    TH2D beff          = TH2D((TString)("beff2D_"+samplename) ,     "Bjets eff", npt-1, effptbins, neta-1, effetabins);    beff.Sumw2();
    TH2D ceff          = TH2D((TString)("ceff2D_"+samplename) ,     "Cjets eff", npt-1, effptbins, neta-1, effetabins);    ceff.Sumw2();
    TH2D leff          = TH2D((TString)("leff2D_"+samplename) ,     "Ljets eff", l_npt-1, l_effptbins, l_neta-1, l_effetabins);    leff.Sumw2();
   
    temp.clear();
    temp << beff << ceff << leff;
    effhistos_[samplename]=temp;
    effhistp_=&(effhistos_.find(samplename)->second);
    
  }

 
  void bTagBase::fillEff(const top::PolarLorentzVector & p4, int genPartonFlavour, double bDiscrVal, double puweight){
    if(!makeeffs_)
      return;
    if(!histp_){ //protection
      std::cout << "bTagBase::fillEff: you have to set a samplename before filling efficiency histograms!" <<std::endl;
      std::exit(EXIT_FAILURE);
    }
    if(genPartonFlavour ==0)
      return;

    double abs_eta= fabs(p4.Eta());
    double pt=p4.Pt();
    if(pt>670) pt=669;


    if(fabs(fabs(genPartonFlavour) - 5) < 0.1 ){ // b jets
      if(cbflatineta_) abs_eta=1;
      histp_->at(0).Fill(pt,abs_eta,puweight);
      if(bDiscrVal > wp_) 
	histp_->at(1).Fill(pt,abs_eta,puweight);
    }
    else if(fabs(fabs(genPartonFlavour) - 4) < 0.1 ){ // c jets
      if(cbflatineta_) abs_eta=1;
      histp_->at(2).Fill(pt,abs_eta,puweight);
      if(bDiscrVal > wp_) 
	histp_->at(3).Fill(pt,abs_eta,puweight);
    }  
    else if(fabs(genPartonFlavour) > 0){ // light jets (including gluon jets)
      histp_->at(4).Fill(pt,abs_eta,puweight);
      if(bDiscrVal > wp_) 
	histp_->at(5).Fill(pt,abs_eta,puweight);
    }
    else{

    }
  }


  void bTagBase::makeEffs(){
    if(!makeeffs_)
      return;
    if(!histp_ || !effhistp_){
      std::cout << "bTagBase::makeEffs: you have to set a samplename!" <<std::endl;
      std::exit(EXIT_FAILURE);
    }

    for(unsigned int i=0;i<effhistp_->size();i++){
      for(int binx=1;binx<=effhistp_->at(i).GetNbinsX()+1;binx++){
	for(int biny=1;biny<=effhistp_->at(i).GetNbinsY()+1;biny++){
	  //
	  //uses histos at 2i and 2i+1
	  double cont=0;
	  double err=1;
	  if(histp_->at(2*i).GetBinContent(binx,biny) >0){
	    cont=histp_->at(2*i+1).GetBinContent(binx,biny) / histp_->at(2*i).GetBinContent(binx,biny);
	    std::cout << "makeEffs: content: " << cont;
	    err=sqrt(cont*(1-cont)/ histp_->at(2*i).GetBinContent(binx,biny));
	    std::cout << " error: " << err << "  " << binx << " " << biny << std::endl;
	  }
	  if(err > 0.015 && err!=1 && cont!=0){
	    std::cout << "bTagBase::makeEffs: warning. error in bin (" << binx << ", " << biny << ") for histogram " << effhistp_->at(i).GetName() << " is larger than 0.015" << std::endl;

	    //here there is space for automatic rebinning!! just bool binok, if not merge 2 bins - maybe in second step, here performance is not crucial

	  }
	  effhistp_->at(i).SetBinContent(binx,biny,cont);
	  effhistp_->at(i).SetBinError(binx,biny,err);
	}
      }
    }
  }


  double bTagBase::getEventWeight(const std::vector<top::PolarLorentzVector > &p4s , std::vector<int> genPartonFlavours){
    //skip protection for performance purposes?
    if(makeeffs_) return 1.;

    if(! effhistp_){
      std::cout << "bTagBase::getEventWeight: no sample set! EXIT!" <<std::endl;
      std::exit (EXIT_FAILURE);
    }
    if(p4s.size() != genPartonFlavours.size()){
      std::cout << "bTagBase::getEventWeight: vector<lorentzvector> and vector<genPartonFlavour> must be of same size! EXIT!" <<std::endl;
      std::exit (EXIT_FAILURE);
    }

    double sumStuffEff =0.9999999999;
    double sumStuffSfEff =0.9999999999;
    
    for(unsigned int i=0;i<p4s.size();i++){

      if(genPartonFlavours.at(i) == 0)
	continue;

      double pt=p4s.at(i).Pt();
      double abs_eta=fabs(p4s.at(i).Eta());

      double sf=1;
      double multi=1; //was needed for ICHEP prescription. Now obsolete
      unsigned int effh=100;
      
    
      if(fabs(fabs(genPartonFlavours.at(i)) - 5) < 0.1 ){ // b jets
	effh=0;

	if(cbflatineta_) abs_eta=1;
    
	if(abs(syst_)>1 || syst_==0)  //default
	  sf=BJetSF(pt,abs_eta);
	else if(fabs(syst_+1) < 0.01)            //bjets down
	  sf=BJetSF(pt,abs_eta,-1,multi);
	else if(fabs(syst_-1) < -0.01)             //bjets up
	  sf=BJetSF(pt,abs_eta,1,multi);
      }
      else if(fabs(fabs(genPartonFlavours.at(i)) - 4) < 0.1 ){ // c jets

	if(cbflatineta_) abs_eta=1;
    
	effh=1;
	if(abs(syst_>1) || syst_==0)  //default
	  sf=CJetSF(pt,abs_eta);
	else if(fabs(syst_+1) < 0.01)               //cjets down
	  sf=CJetSF(pt,abs_eta,-1,multi);
	else if(fabs(syst_-1) < -0.01)          //cjets up
	  sf=CJetSF(pt,abs_eta,1,multi);
      }
      else{ // (including gluon jets)
	effh=2;
	if(fabs(syst_) < 1.1) // default
	  sf=LJetSF(pt,abs_eta);
	else if(fabs(syst_+2)<0.01)            // ljets down
	  sf=LJetSF(pt,abs_eta,-1,multi);
	else if(fabs(syst_-2)<0.01)             // ljets up
	  sf=LJetSF(pt,abs_eta,1,multi);
      }
    
      //  if(effh>30) continue; //no defined jet
    

      int ptbin=0;
      int etabin=0; 
      int bla=0;
      double eff=0;
      effhistp_->at(effh).GetBinXYZ(effhistp_->at(effh).FindBin(pt, abs_eta), ptbin, etabin, bla);
      eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin );
      
      if(!(0<=eff && eff<=1 && sf>=0 && sf <=100))
	std::cout << "warning SF: " << sf << "   eff(" << pt << " GeV, " << abs_eta <<"): " <<eff<< "\t genflav: " << genPartonFlavours.at(i) << "\teffh " << effh << std::endl;
      
      sumStuffEff = sumStuffEff* ( 1-eff );
      sumStuffSfEff= sumStuffSfEff* ( 1-sf*eff );
    }
    
    if(!(( 1 -sumStuffSfEff ) / ( 1 -sumStuffEff ) > 0.3 || ( 1 -sumStuffSfEff ) / ( 1 -sumStuffEff ) < 1.5));
      //  std::cout << "unusual weight: " << ( 1 -sumStuffSfEff ) / ( 1 -sumStuffEff ) << std::endl;


    return ( 1 -sumStuffSfEff ) / ( 1 -sumStuffEff );
  }


  void bTagBase::setSystematic(const std::string & set){ 

    if(set.find("heavy")!=std::string::npos && set.find("up")!=std::string::npos){
      syst_=1;}
    else if(set.find("heavy")!=std::string::npos && set.find("down")!=std::string::npos){
      syst_=-1;}
    else if(set.find("light")!=std::string::npos && set.find("up")!=std::string::npos){
      syst_=2;}
    else if(set.find("light")!=std::string::npos && set.find("down")!=std::string::npos){
      syst_=-2;}
    else if(set.find("def")!=std::string::npos){
      syst_=0;}
    else{
      std::cout << "bTagBase::setSystematic: option " << set << " not available. Possible options: heavy up, heavy down, light up, light down, def";
      exit(EXIT_FAILURE);}
  }
  //////////////////////////////////////////// BTV INPUT ///////////////////////////////////////

  double bTagBase::BJetSF( double pt, double eta , int sys, double multiplier){
    //CSVL b-jet SF
    //From BTV-11-004 and https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFb-mujet_payload.txt

    double abs_eta=fabs(eta);
    abs_eta++; // not used right now; #TRAP#

    if ( pt < 30 ) pt = 30;
    if ( pt > 670 ) pt = 669;

    double  ptbins[]  = {30.,40.,50.,60.,70.,80.,100.,120.,160.,210.,260.,320.,400.,500.,670.}; 
    const unsigned int npt=15;
    //   double  etabins[] = {0.0,0.5,1.0,1.5,2.0,2.4,3.0};
    //   const unsigned int neta=7;

  
    double sf11= 1.02658*((1.+(0.0195388*pt))/(1.+(0.0209145*pt)));
    if(sys==0 && is2011_)
      return sf11;    

    double sf12 = 0.981149*((1. -0.000713295*pt)/(1. -0.000703264*pt));
    if(sys==0)
      return sf12;
   
    double sf=sf12;
    if(is2011_)
      sf=sf11;

    double SFb_error11[] = { 0.0188743, 0.0161816, 0.0139824, 0.0152644, 0.0161226, 0.0157396, 0.0161619, 0.0168747, 0.0257175, 0.026424, 0.0264928, 0.0315127, 0.030734, 0.0438259 };
    double SFb_error12[] = {0.0484285, 0.0126178, 0.0120027, 0.0141137, 0.0145441, 0.0131145, 0.0168479, 0.0160836, 0.0126209, 0.0136017, 0.019182, 0.0198805, 0.0386531, 0.0392831, 0.0481008, 0.0474291 }; //moriond13 values tt and mu+jets

    double abserr=0;
    if(is2011_){
      for(unsigned int i=1;i<npt;i++){
	if(ptbins[i] > pt){
	  abserr=SFb_error11[i-1];
	}
      }
    }
    else{
      for(unsigned int i=1;i<npt;i++){
	if(ptbins[i] > pt){
	  abserr=SFb_error12[i-1];
	}
      }
    }
    if(sys<0)
      return sf-(abserr*multiplier);
    if(sys>0)
      return sf+(abserr*multiplier);
    else 
      return sf;
  }



  double bTagBase::CJetSF ( double pt, double eta, int sys, double multiplier){
    return BJetSF(pt, eta,sys ,multiplier*2);
  }


  double bTagBase::LJetSF ( double pt, double eta, int sys, double multiplier){    
    //CSVL ligth jet mistag SF.
    //From BTV-11-004 and https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFlightFuncs.C

    double eta_abs = fabs(eta);
    double x =pt; //for easier copy paste
    if(x>=670) x=669; //approx but should be ok for ttbar/Z etc
    //for eta > 2.4, the value for 2.4 is taken (should also be ok)

    multiplier++; // to avoid warnings

    double sf=1;

    if(is2011_){
      if(sys==0){
	if ( eta_abs <= 0.5 ) {
	  sf = ((1.07536+(0.000175506*x))+(-8.63317e-07*(x*x)))+(3.27516e-10*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf = ((1.07846+(0.00032458*x))+(-1.30258e-06*(x*x)))+(8.50608e-10*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf = ((1.08294+(0.000474818*x))+(-1.43857e-06*(x*x)))+(1.13308e-09*(x*(x*x)));
	} else {
	  sf = ((1.0617+(0.000173654*x))+(-5.29009e-07*(x*x)))+(5.55931e-10*(x*(x*x)));
	}
      }
      else if(sys<0){
	if ( eta_abs <= 0.5 ) {
	  sf = ((0.994425+(-8.66392e-05*x))+(-3.03813e-08*(x*x)))+(-3.52151e-10*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf = ((0.998088+(6.94916e-05*x))+(-4.82731e-07*(x*x)))+(1.63506e-10*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf = ((1.00294+(0.000289844*x))+(-7.9845e-07*(x*x)))+(5.38525e-10*(x*(x*x)));
	} else {
	  sf = ((0.979816+(0.000138797*x))+(-3.14503e-07*(x*x)))+(2.38124e-10*(x*(x*x)));
	}
      }
      else if(sys>0){
	if ( eta_abs <= 0.5 ) {
	  sf = ((1.15628+(0.000437668*x))+(-1.69625e-06*(x*x)))+(1.00718e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf = ((1.15882+(0.000579711*x))+(-2.12243e-06*(x*x)))+(1.53771e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf = ((1.16292+(0.000659848*x))+(-2.07868e-06*(x*x)))+(1.72763e-09*(x*(x*x)));
	} else {
	  sf = ((1.14357+(0.00020854*x))+(-7.43519e-07*(x*x)))+(8.73742e-10*(x*(x*x)));
	}
      }
    }    

    else{ //2012 perscr https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFlightFuncs_Moriond2013.C  // Begin of definition of functions from SF_12ABCD ---------------
      if(sys==0){
	if ( eta_abs <= 0.5 ) {
	  sf =((1.04901+(0.00152181*x))+(-3.43568e-06*(x*x)))+(2.17219e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf =((0.991915+(0.00172552*x))+(-3.92652e-06*(x*x)))+(2.56816e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf =((0.962127+(0.00192796*x))+(-4.53385e-06*(x*x)))+(3.0605e-09*(x*(x*x)));
	} else {
	  sf =((1.06121+(0.000332747*x))+(-8.81201e-07*(x*x)))+(7.43896e-10*(x*(x*x)));
	}  
      }
      else if(sys<0){
	if ( eta_abs <= 0.5 ) {
	  sf =((0.973773+(0.00103049*x))+(-2.2277e-06*(x*x)))+(1.37208e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf =((0.921518+(0.00129098*x))+(-2.86488e-06*(x*x)))+(1.86022e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf =((0.895419+(0.00153387*x))+(-3.48409e-06*(x*x)))+(2.30899e-09*(x*(x*x)));
	} else {
	  sf =((0.983607+(0.000196747*x))+(-3.98327e-07*(x*x)))+(2.95764e-10*(x*(x*x)));
	}  
      }
      else if(sys>0){
	if ( eta_abs <= 0.5 ) {
	  sf =((1.12424+(0.00201136*x))+(-4.64021e-06*(x*x)))+(2.97219e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.0 ) {
	  sf =((1.06231+(0.00215815*x))+(-4.9844e-06*(x*x)))+(3.27623e-09*(x*(x*x)));
	} else if ( eta_abs <= 1.5 ) {
	  sf =((1.02883+(0.00231985*x))+(-5.57924e-06*(x*x)))+(3.81235e-09*(x*(x*x)));
	} else {
	  sf =((1.1388+(0.000468418*x))+(-1.36341e-06*(x*x)))+(1.19256e-09*(x*(x*x)));
	}  
      }
    }
    return sf;
  }



}
