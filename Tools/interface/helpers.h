/*
 * helpers.h
 *
 *  Created on: Feb 4, 2014
 *      Author: kiesej
 */

#ifndef HELPERS_H_
#define HELPERS_H_
/***
 * helpers to list containers, graphs etc globally
 */
#include <vector>
#include "TString.h"
class TFile;


namespace ztop{

  std::vector<TString> listAllContainers(TFile *f);

}
#endif /* HELPERS_H_ */
