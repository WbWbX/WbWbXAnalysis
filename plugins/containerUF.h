#ifndef containerUF_h
#define containerUF_h


namespace top{

  class container1DUF : public container1D{

  public:

    container1DUF();
    container1DUF(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);
    ~container1D();

    container1DUF simpleunfold();



  };


  //functions



}

#endif
