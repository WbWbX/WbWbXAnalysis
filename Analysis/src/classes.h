#include "../interface/scalefactors.h"
#include "../interface/reweightfunctions.h"
#include "../interface/discriminatorFactory.h"
#include <string>

//#include "TtZAnalysis/Tools/interface/histo1D.h"

namespace
{
struct dictionary {

    ztop::scalefactors a;
    ztop::reweightfunctions b;

//

    std::vector< float *const*> d;
    std::string f;
    ztop::discriminatorFactory c;
    //

};
}

