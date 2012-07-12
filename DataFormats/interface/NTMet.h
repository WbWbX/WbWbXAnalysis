#ifndef NTMet_h
#define NTMet_h

namespace top{
  
  class NTMet{
 public:
  explicit NTMet(){};
  ~NTMet(){};
  //sets
  void setMet(double Met){met_=Met;}
  void setPhi(double Phi){phi_=Phi;}

  //gets
  double met(){return met_;}
  double phi(){return phi_;}


 protected:
  double met_;
  double phi_;

  };
}
#endif
