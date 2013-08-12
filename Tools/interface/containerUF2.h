#ifndef containerUF2_h
#define containerUF2_h

#include "container.h"
#include "container2D.h"
#include "TString.h"
#include "../TUnfold/TUnfold.h"



// add function to make a tex table of the contents (syst)

// container with some more plotting options and (very simple) unfolding

// make containerUFStack class inherits from normal stack?!? (check protected,private etc. such that all functions on member containers_ now apply for a vector of containerUF.. might be tricky / or not? in case of doubt containers_ ->private

//include Alan Caldwells stat error interpretation?? -> check with someone, how they feel about it.

namespace ztop{

  class container1DUF2 : public container1D{

  public:

    container1DUF2(){debug_=false;TUnfold bla(new TH2D(),TUnfold::kHistMapOutputHoriz,TUnfold::kRegModeNone,TUnfold::kEConstraintNone); }
    container1DUF2(const ztop::container1D &){}
    ~container1DUF2(){};


    container1DUF2 binByBinUnfold(){return *this;}
    container1DUF2 SVDUnfold(){return *this;}

   // void setBinName(int, TString );

  //  void coutBinContents();


    ztop::container1DUF2 breakDownRelSystematicsInBin(int){return *this;}


    //some plotting stuff - put in external class, just leave some debug stuff here for the time-being (inherited from container1D

  //  TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool noXErrors=false);
  //  TH1D * getTH1D(TString name="", bool dividebybinwidth=true);

    //define operators based on member ops like
   /*
    * operator + {
    * this container1D::+ bla
    * this->recosel + bla.recosel....
    * }
    */


    bool debug_;
  protected:
    std::vector<TString> binnames_;

   // container1D recosel_;//?
    container2D matrix_;

/*
 * need: 1D histogram (use *this container properties)
 *       2D histogram (use cont2d (to be implemented))
 */

  };

  ///////////////// /////////////////////////////////// //////////////////
  ///////////////// /////////////////////////////////// //////////////////
  ///////////////// /////////////////////////////////// //////////////////




}
/*
void test(){

}
*/


#endif
