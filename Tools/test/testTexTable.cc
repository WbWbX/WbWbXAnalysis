/*
 * testTexTable.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */


#include "../interface/texTabler.h"
#include <iostream>

int main(){
    using namespace ztop;

    texTabler tt(" c | r | l r");

    tt.setTopLine(texLine(2));

    tt << "bla" << 4 << "bli" << "gb";
    tt << texLine(3);
    tt << "bla2" << "blu2" << -5.6  << "hh";
    tt << "bla2" << "blu2" << 5.7  << "hh";
    tt << "bla2" << "blu2" << 5.8  << "hh";
    tt << "bla2" << "blu2" << 5.9  << "hh";
    tt << "bla2" << "blu2" << 5.10  << "hh";
    tt << "bla2" << "blu2" << 5.11  << "hh";
    tt << texLine(3);
    tt << "bla2" << "blu2" << 5.12  << "hh";

    std::cout << tt.getTable() <<std::endl;
    return 0;
}
