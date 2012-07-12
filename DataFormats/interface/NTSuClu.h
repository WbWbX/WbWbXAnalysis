#ifndef NTSuClu_h
#define NTSuClu_h

namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;
  class NTSuClu{
  public:
    explicit NTSuClu(){LorentzVector temp(0,0,0,0); setP4(temp);};
    ~NTSuClu(){};

    void setP4(LorentzVector P4energ){P4_=P4energ;};
    LorentzVector p4(){return P4_;};
    double eta(){return P4_.Eta();}
    double pt(){return P4_.Pt();}
  private:
    LorentzVector P4_;
  };
}

#endif
