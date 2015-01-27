/*
 * version.h
 *
 *  Created on: Jan 13, 2015
 *      Author: kiesej
 */

#ifndef VERSION_H_
#define VERSION_H_


#define CMSSW_LEQ_5
#if __GNUC__ && __GNUC__ > 3 && __GNUC_MINOR__ && __GNUC_MINOR__ > 7
#undef CMSSW_LEQ_5
#endif

#endif /* VERSION_H_ */
