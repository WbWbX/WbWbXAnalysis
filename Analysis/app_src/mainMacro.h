/*
 * mainMacro.h
 *
 *  Created on: Oct 14, 2014
 *      Author: kiesej
 */

#ifndef MAINMACRO_H_
#define MAINMACRO_H_


#include "TtZAnalysis/Tools/interface/optParser.h"
/**
 * invokes main and passes arguments to ztop::optParser parser
 */
#define invokeApplication() int application_main_function(ztop::optParser* parser); \
int main(int argc, char* argv[]){  ztop::optParser parser(argc,argv); return application_main_function(&parser);} \
int application_main_function(ztop::optParser* parser)


#endif /* MAINMACRO_H_ */
