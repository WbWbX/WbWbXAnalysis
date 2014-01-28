#ifndef NTIsolation_h
#define NTIsolation_h

namespace ztop{

  class NTIsolation{
  public:
    explicit NTIsolation(){ChargedHadronIso_=-999;NeutralHadronIso_=-999;PhotonIso_=-999;PuChargedHadronIso_=-999;}
    ~NTIsolation(){}
    //sets
    void setChargedHadronIso(float ChargedHadronIso){ChargedHadronIso_=ChargedHadronIso;}
    void setNeutralHadronIso(float NeutralHadronIso){NeutralHadronIso_=NeutralHadronIso;}
    void setPhotonIso(float PhotonIso){PhotonIso_=PhotonIso;}
    void setPuChargedHadronIso(float PuChargedHadronIso){PuChargedHadronIso_=PuChargedHadronIso;}


    //gets
    const float &chargedHadronIso()const{return ChargedHadronIso_;}
    const float &neutralHadronIso()const{return NeutralHadronIso_;}
    const float &photonIso()const{return PhotonIso_;}
    const float &puChargedHadronIso()const{return PuChargedHadronIso_;}

  protected:
    float ChargedHadronIso_;
    float NeutralHadronIso_;
    float PhotonIso_;
    float PuChargedHadronIso_;

  };
}
#endif
