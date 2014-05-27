/*
 * testExtendedVariable.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */



#include "../interface/extendedVariable.h"
#include "../interface/graph.h"
#include <vector>

int main(){

    using namespace ztop;
    using namespace std;

    extendedVariable t;

    //get some dependencies

    graph dep1;
    dep1.addPoint(-1,0.5,0.01);
    dep1.addPoint( 0,1  ,0.005);
    dep1.addPoint( 1,1.9,0.01);


    graph dep2;
    dep2.addPoint(-1,0.8,0.01);
    dep2.addPoint( 0,1  ,0.005); //this neeeeed to stay the same
    dep2.addPoint( 1,1.3,0.01);


    t.addDependence(dep1,1,"bla");
    t.addDependence(dep2,1,"bla2");

    std::vector<float> varis;varis.resize(2);

    varis.at(0)=0;
    varis.at(1)=0;

    std::cout << t.getValue(varis) <<std::endl;

    varis.at(0)=-1;
    varis.at(1)=0;

    std::cout << t.getValue(varis) <<std::endl;

    varis.at(0)=1;
    varis.at(1)=0;

    std::cout << t.getValue(varis) <<std::endl;


    varis.at(0)=0;
    varis.at(1)=-1;

    std::cout << t.getValue(varis) <<std::endl;

    varis.at(0)=0;
    varis.at(1)=1;

    std::cout << t.getValue(varis) <<std::endl;

    varis.at(0)=-0.5;
    varis.at(1)=0;

    std::cout << t.getValue(varis) <<std::endl;


    return 0;
}
