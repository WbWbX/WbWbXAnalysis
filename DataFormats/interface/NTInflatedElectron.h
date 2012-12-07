#ifndef NTInflatedElectron_h
#define NTInflatedElectron_h

#include "NTElectron.h"
#include <map>

namespace top{



  class NTInflatedElectron : public NTElectron {

  public:
    explicit NTInflatedElectron(){q_=0;};
    ~NTInflatedElectron(){};

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
