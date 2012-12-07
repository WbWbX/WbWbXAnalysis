#ifndef NTInflatedMuon_h
#define NTInflatedMuon_h

#include "NTMuon.h"
#include <map>

namespace top{



  class NTInflatedMuon : public NTMuon {

  public:
    explicit NTInflatedMuon(){q_=0;};
    ~NTInflatedMuon(){};

    void setMember(std::string Membername, double value=-999999999){
      members_[Membername]=value;
    }

    double getMember(std::string membername){
      return members_.find(membername)->second;
    }

  protected:
    std::map<std::string, double> members_;
  };

}

#endif
