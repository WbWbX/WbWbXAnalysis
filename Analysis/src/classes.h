#include "WbWbXAnalysis/Analysis/interface/scalefactors.h"
#include "WbWbXAnalysis/Analysis/interface/reweightfunctions.h"
#include "WbWbXAnalysis/Analysis/interface/discriminatorFactory.h"
#include <string>


//#include "WbWbXAnalysis/Tools/interface/histo1D.h"

namespace
{
struct dictionary_ana {

    ztop::scalefactors a;
    ztop::reweightfunctions b;

//

    std::vector< float *const*> d;
    std::string f;
    ztop::discriminatorFactory c;
    //

};
}

