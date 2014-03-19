/*
 * container1DUnfold.h
 *
 *  Created on: Aug 14, 2013
 *      Author: kiesej
 */

#ifndef CONTAINER1DUNFOLD_H_
#define CONTAINER1DUNFOLD_H_

#include "container2D.h"
#include <iostream>

/**
 * Warning:
 * due to internal reasons, statistics on systematic variation are NOW accounted for
 *
 */

/*
 * helper class to store unfolding output etc
 *
 */
class TTree;
class TFile;



namespace ztop{
class container1DUnfold: public container2D {
public:
    container1DUnfold();
    container1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);
    container1DUnfold( std::vector<float> genbins, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);

    ~container1DUnfold();



    void setBinning(const std::vector<float> & genbins,const std::vector<float> &recobins);
    //void subdivideRecoBins(int div);

    container1DUnfold rebinToBinning(const std::vector<float> &)const;

    void clear(){container2D::clear(); gencont_.clear(); recocont_.clear();}
    void reset(){container2D::reset(); gencont_.reset();recocont_.reset();}
    void removeError(TString name){container2D::removeError(name); gencont_.removeError(name); recocont_.removeError(name);unfolded_.removeError(name);}
    void renameSyst(TString old , TString New){container2D::renameSyst(old,New);gencont_.renameSyst(old,New);recocont_.renameSyst(old, New);}
    void removeAllSystematics();

    void fillGen(const float & val, const float & weight=1); //if isMC false only fill one specific container - which one?? 0<->underflow? or first one...?
    void fillReco(const float & yval, const float & weight=1);

    void setMC(bool ismc){isMC_=ismc; if(!isMC_) flushed_=true;}
    bool getMC(){return isMC_;}

    void setBinByBin(bool bbb);
    bool isBinByBin() const{return binbybin_;}

    void setLumi(float lumi){lumi_=lumi;}
    const float& getLumi(){return lumi_;}

    const container1D & getGenContainer() const {return gencont_;}
    const container1D & getRecoContainer() const {return recocont_;}

    container1D  getBinnedGenContainer() const{return gencont_.rebinToBinning(genbins_);}

    const std::vector<float> & getGenBins()const{return genbins_;}


    void setRecoContainer(const container1D &cont);
    void setBackground(const container1D &cont);
    void addToBackground(const container1D &cont){setBackground(getBackground()+cont);}
    container1D getBackground() const;

    /*
     * only container1DUnfold operator * (float) operates on the unfolded distribution.. let's see for the others
     */

    container1DUnfold operator + (const container1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator - (const container1DUnfold &);       //! adds errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator / (const container1DUnfold &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container1DUnfold operator * (const container1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator * (float);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container1DUnfold operator *= (float val){*this=*this*val;return *this;}            //! simple scalar multiplication. stat and syst errors are scaled accordingly!! room for performance improvement

    void setDivideBinomial(bool);

    void addErrorContainer(const TString & ,const container1DUnfold &,float weight);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addErrorContainer(const TString & ,const container1DUnfold &);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void getRelSystematicsFrom(const container1DUnfold &);
    void addGlobalRelError(TString,float);

    static std::vector<container1DUnfold *> c_list;
    static bool debug;
    static bool c_makelist;
    static void c_deletelist(){
        size_t count=0,listsize=c_list.size();
        for(unsigned int i=0;i<listsize;i++){
            if(c_list[count]) delete c_list[count]; //delete also removes entry from list
            else count++; //that should never happen if list not touched from outside
        }
    }


    /**
     * does nothing for data where flushed_ is always true
     */
    bool check();

    /**
     * TBI!!! FIXME
     * This function folds a (generated) input distribution
     * In contrast to the unfolding this takes place within this class.
     * The unfolding is done by an outside class, the containerUnfolder,
     * to keep the container1DUnfold class slim.
     * Checks on binning etc are performed.
     * The output will be binned in reco binning and will incorporate all systematic
     * uncertainties (TBI2:+stat of resp matrix).
     * It will be equivalent to a reconstructed distribution with background subtracted.
     * If <subtractbackground> is set to false, the reconstructed background with all its uncertainties
     * will be added
     */
    container1D fold(const container1D& input, bool subtractbackground=true) const;

    const container1D & getControlPlot() const{return recocont_;}

    const container1D & getUnfolded() const {return unfolded_;}
    const container1D & getRefolded() const {return refolded_;}

    void setUnfolded(const container1D & UF)  { unfolded_=UF;}
    void setRefolded(const container1D & RF)  { refolded_=RF;}

    container1D getPurity() const;
    container1D getStability(bool includeeff=true) const;

    /**
     * should be obsolete now since 2D matrix anyway constructed as N x N (check) ?!
     */
    bool checkCongruentBinBoundariesXY() const;
    histoBin getDiagonalBin(const size_t & xbin, int layer=-1) const;
    container1D getDiagonal()const;

    container2D getResponseMatrix()const;
    TH2D * prepareRespMatrix(bool nominal=true,unsigned int systNumber=0) const;

    // someformat getCovarianceMatrix(size_t syst) { // M_ij = d_i * d_j, d_i= nominal_i-sys_i, i=bin}

    static void flushAllListed();
    static void setAllListedMC(bool ISMC);
    static void setAllListedLumi(float lumi);
    static void checkAllListed();

    TString coutUnfoldedBinContent(size_t bin) const;

    void flush();


    ///IO
    void loadFromTree(TTree *, const TString & plotname);
    void loadFromTFile(TFile *, const TString & plotname);
    void loadFromTFile(const TString& filename, const TString & plotname);

    void writeToTree(TTree *);
    void writeToTFile(TFile *);
    void writeToTFile(const TString& filename);

private:

    void fill(const float & xval, const float & yval, const float & weight=1){
        container2D::fill(xval,yval,weight);
    }
    container2D rebinXToBinning(const std::vector<float>& vec)const{return container2D::rebinXToBinning(vec);}
    container2D rebinYToBinning(const std::vector<float>& vec)const{return container2D::rebinYToBinning(vec);}


    TString xaxis1Dname_,yaxis1Dname_;

    float tempgen_,tempreco_,tempgenweight_,tempweight_;
    bool recofill_,genfill_;

    bool isMC_;
    bool flushed_;
    bool binbybin_;

    container1D gencont_,recocont_,unfolded_,refolded_;
    std::vector<float> genbins_;

    float lumi_;

    bool congruentbins_;

    /*
   //destructor safe
   unfolder * unfold_;
   std::vector<unfolder *> unfolds_;
     */
    //some functions that should not be used although inherited
    void addErrorContainer(const TString & ,const container2D &,float){}
    void addErrorContainer(const TString &,const container2D &){}
    void getRelSystematicsFrom(const container2D &){}

    std::vector<float> subdivide(const std::vector<float> & bins, size_t div);

    bool checkCongruence(const std::vector<float>&, const std::vector<float>&)const;
};
inline void container1DUnfold::flush(){ //only for MC
    if(flushed_)
        return;

    if(genfill_)
        gencont_.fill(tempgen_,tempgenweight_);
    if(recofill_)
        recocont_.fill(tempreco_,tempweight_);

    if(genfill_ && !recofill_){ //put in Reco UF bins
        fill(tempgen_,ybins_[1]-100,tempgenweight_);}
    else if(recofill_ && !genfill_){ //put in gen underflow bins -> goes to background
        fill(xbins_[1]-100,tempreco_,tempweight_);}
    else if(genfill_ && recofill_){
        fill(tempgen_,tempreco_,tempweight_);
        fill(tempgen_,ybins_[1]-100,(tempgenweight_-tempweight_)); // w_gen * (1 - recoweight), tempweight_=fullweight=tempgenweight_*recoweight
    }

    recofill_=false;
    genfill_=false;
    flushed_=true;
}


inline void container1DUnfold::fillGen(const float & val, const float & weight){
    tempgen_=val;
    tempgenweight_=weight;
    genfill_=true;
    flushed_=false;
}

inline void container1DUnfold::fillReco(const float & val, const float & weight){ //fills and resets tempgen_
    tempreco_=val;
    tempweight_=weight;
    recofill_=true;
    flushed_=false;
}





} //namespace

#endif /* CONTAINER1DUNFOLD_H_ */
