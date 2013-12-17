/*
 * scalefactors.cc
 *
 *  Created on: Aug 1, 2013
 *      Author: kiesej
 */
#include "../interface/scalefactors.h"
#include "TFile.h"
#include <cstdlib>
#include <stdexcept>

namespace ztop {
/**
 * sets input with a lot of error checks
 * return codes < 0 --> error
 * -1: wrong filename
 * -2: wrong histoname
 * -3: wrong histoclass
 *
 * no error returns 0;
 */
int scalefactors::setInput(TString filename, TString histname) {
    TFile * f = new TFile(filename, "READ");
    isglobal_ = false;
    if (f->IsZombie()) {
        std::cout << "scalefactors::setInput: file " << filename
                << " not found.!" << std::endl;
        throw std::runtime_error("scalefactors::setInput: file not found");
    }
    TH1 * ph = (TH1*) f->Get(histname);
    if (ph->IsZombie()) {
        std::cout << "scalefactors::setInput: histo " << histname
                << " not found in file " << filename << " exit!" << std::endl;
        throw std::runtime_error("scalefactors::setInput: histo not found");
    }
    TString classname = ph->ClassName();
    if (classname.Contains("TH2D")) {
        th2d_ = *(TH2D*) ph;
        h = &th2d_;
        isth2d_ = true;
        isglobal_ = false;
        return 0;
    } else if (classname.Contains("TH1D")) {
        th1d_ = *(TH1D*) ph;
        h = &th1d_;
        isth2d_ = false;
        isglobal_ = false;
        return 0;
    } else {
        std::cout
                << "scalefactors::setInput: class of input histo must be TH1D or TH2D"
                << std::endl;
        throw std::runtime_error("scalefactors::setInput: histo wrong format");
    }

}
/**
 * errors in percent!
 */
void scalefactors::setGlobal(double sf, double errup, double errdown) {
    glsf_ = sf;
    glsfup_ = glsf_ + errup / 100;
    glsfd_ = glsf_ - errdown / 100;
    isglobal_ = true;
}

/**
 * sets systematic variation.
 * returns -1 in case of no success and sets to nominal
 * inputstring should contain only one option of:
 * -nom
 * -down
 * -up
 * if more than one option is provided, the first found is set according to the
 * ordering given above
 */
int scalefactors::setSystematics(TString updownnom) {
    if (updownnom.Contains("nom")) {
        syst_ = 100;
        std::cout << "scalefactors::setSystematics: set to " << updownnom
                << std::endl;
        return 0;
    } else if (updownnom.Contains("down")) {
        syst_ = -1;
        std::cout << "scalefactors::setSystematics: set to " << updownnom
                << std::endl;
        return 0;
    } else if (updownnom.Contains("up")) {
        syst_ = 1;
        std::cout << "scalefactors::setSystematics: set to " << updownnom
                << std::endl;
        return 0;
    } else {
        syst_ = 0;
        std::cout
                << "scalefactors::setSystematics: input must contain up, down or nom"
                << std::endl;
        return -1;
    }
}
scalefactors::scalefactors(const ztop::scalefactors & rhs) {
    th1d_ = rhs.th1d_;
    th2d_ = rhs.th2d_;
    isth2d_ = rhs.isth2d_;
    syst_ = rhs.syst_;
    isMC_ = rhs.isMC_;
    rangecheck_ = rhs.rangecheck_;
    if (isth2d_)
        h = &th2d_;
    else
        h = &th1d_;
}

}

