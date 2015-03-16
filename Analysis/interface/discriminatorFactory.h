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
#include "TtZAnalysis/Tools/interface/histo1D.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"

class TFile;

namespace ztop{
class NTFullEvent;
class histo2D;

/**
 * put more info here
 *
 *
 * workflow (keep ordering) for producing likelihoods
 *
 * for each sample:
 *  - setStep()
 *  - setNBins()
 *  - addVariable() x nvars
 * (-setSystematics)
 *  - loop over events:
 *      fill(eventweight)
 *
 * If a variable is NOT between 0 and 1 it is NOT considered
 *
 * for one sample write configuration to File:
 *  - writeToTFile
 * for all samples write a histoStackVector (csv) to output file
 *  - csv.addList(...)
 *  if signal:
 *    - csv.addSignal(<signalname>)
 *  - csv.writeToTFile
 *
 *
 * to extract likelihoods:
 *  - get csv with all samples
 *  - extractLikelihoods(csv);
 *  - setUseLikelihoods(true);
 *
 * store in output file:
 *  - writeToTFile
 *
 * -------apply and use it-----
 * - readFromTFile()
 * - addVariable() (all you want to use - don't need to be all that are formerly written)
 * - setSystematics() (to account for diff syst for different likelihoods - they need to be written first!)
 * - loop over events:
 *     getCombinedLikelihood()
 *     (also fill that in a control plot)
 *
 */
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
    const std::string& getIdentifier()const{return id_;}

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

    void addVariable( float * const *,const TString& , const float& rangmin=0, const float& rangemax=1);



    /*
     * if a variable is not defined, a 1 will be filled (ignore in total likelihood)
     * if create=false then nothing is done
     */
    void fill(const float& );


    /**
     * this is the return function. Only returns something if setUseLikelihoods is true and LH are filled
     * (note:) put in some safety checks
     * the output will be normalized to the maximum achievable likelihood * 1.1 using the nominal variation
     */
    float getCombinedLikelihood()const; //a plot needs to be added to control plots. NOT done automatically



    /**
     * uses a CSV with all its plots tagged accordingly to
     * create a likelihood
     * use this function whn reading from file?
     */
    void extractLikelihoods(const histoStackVector &);
    static void extractAllLikelihoods(const histoStackVector &);
    /*
     * this only writes the configuration to the file.
     * If it is already written there, it does nothing.
     * You can write several different discriminatorFactory to the same file
     * But take care that they have a different id
     * Plots MUST be saved in an additional histoStackVector!
     */
    void writeToTFile(TFile *)const;
    void writeToTFile(const TString& filename)const;

    static void writeAllToTFile(TFile *);
    static void writeAllToTFile(const TString& filename);
    static void writeAllToTFile(const TString& filename,const std::vector<discriminatorFactory> &vec);

    /**
     * In a first step only works if a histoStack exists for each plot in a histoStackVector
     * just search for one. If there are more than one (usually not) that contain discr. info, give a warning
     *
     */
    void readFromTFile( TFile * f ,const std::string& id);
    void readFromTFile( const TString& filename ,const std::string& id);
    static std::vector<discriminatorFactory> readAllFromTFile( TFile * f);
    static std::vector<discriminatorFactory> readAllFromTFile(const TString& filename);

    void setSaveCorrelationPlots(bool doit){savecorrplots_=doit;}


    bool operator == (const discriminatorFactory&)const;


    static bool debug;


    //some listing stuff
    static std::vector<discriminatorFactory*> c_list;
    static bool c_makelist;


private:

    size_t nbins_;

    bool uselh_;

    std::string id_;

    size_t step_;

    //only for IO operation. Will be updated with new conts (e.g. after DY rescaling)
    std::vector<histo1D> likelihoods_;

    //only for filling/pointing purposes, these are like control plots
    std::vector<histo1D *> tobefilled_;

    std::vector<std::vector< histo2D *> > tobefilledcorr_;

    std::vector< float * const*> vars_;
    std::vector<float> offsets_;
    std::vector<float> ranges_;
    //general pointers/depend on mode. has same indices as vars_
    std::vector<histo1D *> histpointers_;

    //for using the right systematic variation
    int systidx_;
    float maxcomb_;
    bool savecorrplots_;

};

}

#endif /* DISCRIMINATORFACTORY_H_ */
