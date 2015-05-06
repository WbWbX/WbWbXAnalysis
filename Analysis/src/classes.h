#include "TtZAnalysis/Analysis/interface/scalefactors.h"
#include "TtZAnalysis/Analysis/interface/reweightfunctions.h"
#include "TtZAnalysis/Analysis/interface/discriminatorFactory.h"
#include <string>


//#include "TtZAnalysis/Tools/interface/histo1D.h"

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

