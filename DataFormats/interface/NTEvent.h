#ifndef NTEvent_h
#define NTEvent_h
#include "TString.h"
#include <vector>

namespace ztop{
  class NTEvent{
  public:
    explicit NTEvent(){}
    ~NTEvent(){}
    //sets
    void setRunNo(double RunNo){runNo_=RunNo;}
    void setLumiBlock(double LumiBl){lumiBl_=LumiBl;}
    void setEventNo(double EventNo){eventNo_=EventNo;}

    void setVertexMulti(double VertexMulti){vertexMulti_=VertexMulti;}
    void setFiredTriggers(std::vector<std::string> Triggers){triggers_=Triggers;}

    void setTruePU(float BXminus, float BXzero, float BXplus){truePU_[0]=BXminus; truePU_[1]=BXzero; truePU_[2]=BXplus;}

    void setIsoRho(std::vector<double> Rhos){rhos_=Rhos;}
    void setPDFWeights(std::vector<double> Weights){pdfweights_=Weights;}

    //gets
    double runNo(){return runNo_;}
    double lumiBlock(){return lumiBl_;}
    double eventNo(){return eventNo_;}

    double vertexMulti(){return vertexMulti_;}
   const std::vector<std::string>& firedTriggers()const{return triggers_;}

    const float& truePU(int BX=0)const{return truePU_[1+BX];}
    double trueAvgPU()const{return (truePU_[0]+truePU_[1]+truePU_[2])/3;}

    double isoRho(unsigned int identifier){return rhos_[identifier];}
    const double& PDFWeight(unsigned int id)const{return pdfweights_[id];}
    unsigned int PDFWeightsSize(){return pdfweights_.size();}

  protected:
    double runNo_;
    double lumiBl_;
    double eventNo_;

    double vertexMulti_;
    std::vector<std::string> triggers_;

    std::vector<double> rhos_;
    std::vector<double> pdfweights_;

    float truePU_[3];

  };
}

#endif
