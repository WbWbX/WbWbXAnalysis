/*
 * graphFitter.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */


#include "../interface/graphFitter.h"
#include "../interface/graph.h"



namespace ztop{

graphFitter::graphFitter():simpleFitter(){}


void graphFitter::readGraph(const graph* g){

    clearPoints();

    std::vector<point2D> nompoints;
    std::vector<point2D> errsup;
    std::vector<point2D> errsdown;
    //translate
    for(size_t i=0;i<g->getNPoints();i++){
        nompoints.push_back(point2D (g->getPointXContent(i),g->getPointYContent(i)));
        errsup.push_back(point2D (g->getPointXErrorUp(i,false),g->getPointYErrorUp(i,false)));
        errsdown.push_back(point2D (g->getPointXErrorDown(i,false),g->getPointYErrorDown(i,false)));
    }
    setPoints(nompoints);
    setErrorsUp(errsup);
    setErrorsDown(errsdown);

}


}

