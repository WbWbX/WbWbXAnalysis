#include <vector>
#include "../interface/container.h"
#include "../interface/containerStack.h"
#include "../interface/containerStackVector.h"

namespace
{
  struct dictionary {

    std::pair<TString,std::vector<double> > jj;
    std::vector<std::pair<TString,std::vector<double> > > j;
    std::vector<long int> kl;

    std::vector<TString> kkj;

    top::container1D a;
    std::vector<top::container1D> b;
    top::container1DStack aa;
    std::vector<top::container1DStack> bb;
    top::container1DStackVector aaa;
    std::vector<top::container1DStackVector> bbb;


    
  };
}

