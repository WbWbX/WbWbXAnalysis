#include <TH1D.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TF1.h>
#include <TMatrixDSym.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TAttText.h>
#include <TString.h>
#include <iostream>
#include <sstream>
#include <TCanvas.h>
#include <TFile.h>
#include <TLegend.h>

using namespace std;

TString ToString(float num){
 ostringstream start;
 start << num;
 TString start1 = start.str();
 return start1;
}

struct A7Func { 
  // Reco derivatives histograms
  TH1D* DeltaA7_;  
  TH1D* A7Null_;
  // The final result is simple = A7Null A7 * DeltaA7. 
  // For the minimisationin root we need however TF1 to fit it to data and not histograms
  // This class converts the two hsitograms A7Null DeltaA7 into a TF1 with a parameter which is A7
public:
  
  A7Func( TH1D* DeltaA7, TH1D* A7Null ) : DeltaA7_(DeltaA7), A7Null_(A7Null)
  {
  };


  double A7FuncCore (double *xvar, double *A7par ) const { 
    
    double x =xvar[0];
    int xBin = A7Null_->GetXaxis()->FindBin(x);
    if(DeltaA7_->GetXaxis()->FindBin(x)!=xBin)
      {
	cout << "A7Func warning, A7Null DeltaA7 must have same binning and boundaries!" <<endl;
      }
    
    double result =  A7par[0]*A7Null_->GetBinContent(xBin)+ A7par[1]*DeltaA7_->GetBinContent(xBin);
    return result;
  };
};

class A7Fit{

  TH1D* DATA_;
  TH1D* DeltaA7_;  
  TH1D* A7Null_;
 
  TF1* Func_ ;
  TFitResultPtr Result_;
  float A7_;
  float A7err_;

  // The final result is simple = A7Null A7 * DeltaA7. 
  // For the minimisationin root we need however TF1 to fit it to data and not histograms
  // This class converts the two hsitograms A7Null DeltaA7 into a TF1 with a parameter which is A7
public:
  
  A7Fit( TH1D* DATA, TH1D* DeltaA7, TH1D* A7Null ) :DATA_(DATA), DeltaA7_(DeltaA7), A7Null_(A7Null)
  {
    A7Func * My_A7Func = new A7Func(DeltaA7_,A7Null_);
    Func_ = new TF1("myFunc_",My_A7Func,&A7Func::A7FuncCore,0,100,2,"A7Func","A7FuncCore");
  }

  void Fit()
  {
  Result_ = DATA_->Fit(Func_,"LS");// "L"= likyhood, "S" stores fit result
  //A7_ = 0.1 *  Func_->GetParameter(1)/Func_->GetParameter(0);
  //TMatrixDSym CoreMatrix =   Result_->GetCovarianceMatrix(); 

  //A7err_ = 0.1* sqrt(  1/pow(Func_->GetParameter(0),2)*Func_->GetParError(1)*Func_->GetParError(1)  +   Func_->GetParameter(1)*Func_->GetParameter(1)/pow(Func_->GetParameter(0),4)*Func_->GetParError(0)*Func_->GetParError(0) - Func_->GetParameter(1)/pow(Func_->GetParameter(0),3)*CoreMatrix(0,1) );

 };


  float ReturnPar0(){
      return Func_->GetParameter(0);
  };

  float ReturnPar1(){
      return Func_->GetParameter(1);
  };


  void PrintFitResult(int b)
  {
    cout<< "The fit results are in term of fitted parameters are: "<<endl;
    cout << "norm " << Func_->GetParameter(0)<< "+_" <<  Func_->GetParError(0) << " A7 \"s-section\" " << Func_->GetParameter(1)<< "+_" <<  Func_->GetParError(1)<<endl;
    //cout << " In term of A7 this is: "<<endl;
    //cout << " A7 = "<< A7_ << "+-" << A7err_<<endl;    
    

//};//end of for loop
  }; // end of PrintFitResult

};// end of class A7Fit

