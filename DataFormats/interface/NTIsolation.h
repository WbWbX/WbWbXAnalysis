#ifndef NTIsolation_h
#define NTIsolation_h

namespace ztop{

  class NTIsolation{
  public:
    explicit NTIsolation(){ChargedHadronIso_=-999;NeutralHadronIso_=-999;PhotonIso_=-999;PuChargedHadronIso_=-999;}
    ~NTIsolation(){}
    //sets
    void setChargedHadronIso(double ChargedHadronIso){ChargedHadronIso_=ChargedHadronIso;}
    void setNeutralHadronIso(double NeutralHadronIso){NeutralHadronIso_=NeutralHadronIso;}
    void setPhotonIso(double PhotonIso){PhotonIso_=PhotonIso;}
    void setPuChargedHadronIso(double PuChargedHadronIso){PuChargedHadronIso_=PuChargedHadronIso;}


    //gets
    const double &chargedHadronIso()const{return ChargedHadronIso_;}
    const double &neutralHadronIso()const{return NeutralHadronIso_;}
    const double &photonIso()const{return PhotonIso_;}
    const double &puChargedHadronIso()const{return PuChargedHadronIso_;}

  protected:
    double ChargedHadronIso_;
    double NeutralHadronIso_;
    double PhotonIso_;
    double PuChargedHadronIso_;

  };
}
#endif
