/*
 * discriminatorFactory.h
 *
 *  Created on: Mar 26, 2014
 *      Author: kiesej
 */

#ifndef DISCRIMINATORFACTORY_H_
#define DISCRIMINATORFACTORY_H_


#include <string>
#include "TString.h"
#include "TtZAnalysis/Tools/interface/container.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"

class TFile;

namespace ztop{
class NTFullEvent;

class discriminatorFactory{
public:
    discriminatorFactory();
    discriminatorFactory(const std::string & id);
    ~discriminatorFactory();

    /**
     * call this before adding all variables etc.
     * does NOT delete content, only clears all pointers
     * to event variables etc
     */
    void clear();

    void setIdentifier(const std::string & id){id_=id;}

    void setUseLikelihoods(bool doit){uselh_=doit;}

    /**
     * This is important to get step dependent corrections right that are applied after the selection
     * E.g. data driven background estimations etc
     */
    void setStep(size_t step){step_=step;}

    void setNBins(size_t nbins){nbins_=nbins;}

    /**
     * systematics can be named according to the standard scheme
     * nominal must be named "nominal"!
     */
    void setSystematics(const TString& sysname);


    /*
     * Associate variables to be used for the likelihood
     * initializes plots etc. if setUseLikelihoods is false and creation
     * mode is switched on
     */

    void addVariable( float * const *,const TString& );



    /*
     * if a variable is not defined, a 1 will be filled (ignore in total likelihood)
     * if create=false then nothing is done
     */
    void fill(const float& );


    /**
     * this is the return function. Only returns something if setUseLikelihoods is true and LH are filled
     * (note:) put in some safety checks
     */
    float getCombinedLikelihood()const; //a plot needs to be added to control plots. NOT done automatically



    /**
     * uses a CSV with all its plots tagged accordingly to
     * create a likelihood
     * use this function whn reading from file?
     */
    void extractLikelihoods(const containerStackVector &);
    /*
     * this only writes the configuration to the file.
     * If it is already written there, it does nothing.
     * You can write several different discriminatorFactory to the same file
     * But take care that they have a different id
     * Plots MUST be saved in an additional containerStackVector!
     */
    void writeToTFile(TFile *)const;
    void writeToTFile(const TString& filename)const;

    /**
     * In a first step only works if a containerStack exists for each plot in a containerStackVector
     * just search for one. If there are more than one (usually not) that contain discr. info, give a warning
     *
     */
    void readFromTFile( TFile * f ,const std::string& id);
    void readFromTFile( const TString& filename ,const std::string& id);


    bool operator == (const discriminatorFactory&)const;


    static bool debug;


private:

    size_t nbins_;

    bool uselh_;

    std::string id_;

    size_t step_;

    //only for IO operation. Will be updated with new conts (e.g. after DY rescaling)
    std::vector<container1D> likelihoods_;

    //only for filling/pointing purposes, these are like control plots
    std::vector<container1D *> tobefilled_;


    std::vector< float * const*> vars_;
    //general pointers/depend on mode. has same indices as vars_
    std::vector<container1D *> histpointers_;

    //for using the right systematic variation
    int systidx_;

};

}

#endif /* DISCRIMINATORFACTORY_H_ */
