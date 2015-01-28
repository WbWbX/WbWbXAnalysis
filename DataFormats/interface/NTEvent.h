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
    void setRunNo(float RunNo){runNo_=RunNo;}
    void setLumiBlock(float LumiBl){lumiBl_=LumiBl;}
    void setEventNo(float EventNo){eventNo_=EventNo;}

    void setVertexMulti(float VertexMulti){vertexMulti_=VertexMulti;}
    void setFiredTriggers(std::vector<std::string> Triggers){triggers_=Triggers;}

    void setTruePU(float BXminus, float BXzero, float BXplus){truePU_[0]=BXminus; truePU_[1]=BXzero; truePU_[2]=BXplus;}

    void setIsoRho(std::vector<float> Rhos){rhos_=Rhos;}
    void setPDFWeights(std::vector<float> Weights){pdfweights_=Weights;}

    //gets
    const float& runNo()const{return runNo_;}
    const float& lumiBlock()const{return lumiBl_;}
    const float& eventNo()const{return eventNo_;}

    const float& vertexMulti()const{return vertexMulti_;}
   const std::vector<std::string>& firedTriggers()const{return triggers_;}

    const float& truePU(int BX=0)const{return truePU_[1+BX];}
    float trueAvgPU()const{return (truePU_[0]+truePU_[1]+truePU_[2])/3;}

    const float& isoRho(unsigned int identifier)const{return rhos_[identifier];}
    const float& PDFWeight(unsigned int id)const{return pdfweights_[id];}
      unsigned int PDFWeightsSize()const{return pdfweights_.size();}

  protected:
	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////

    float runNo_;
    float lumiBl_;
    float eventNo_;

    float vertexMulti_;
    std::vector<std::string> triggers_;

    std::vector<float> rhos_;
    std::vector<float> pdfweights_;

    float truePU_[3];

  };
}

#endif
