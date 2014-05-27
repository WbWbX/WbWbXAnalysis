/*
 * ttbarXsecExtractor.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#include "../interface/ttbarXsecExtractor.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TtZAnalysis/Tools/interface/container.h"

#include "TtZAnalysis/Tools/interface/simpleFitter.h"

namespace ztop{

ttbarXsecExtractor::ttbarXsecExtractor():setup_(false),done_(false),lumi_(19741),
        lumierr_(0.025),ngenevents_(1),xsec_(1),xsecerrup_(1),xsecerrdown_(1),xsecidx_(0),eps_bidx_(0),xsecoffset_(250){
    fitter_.setRequireFitFunction(false);

}

void ttbarXsecExtractor::readInput(const containerStackVector & csv, const TString& plotname){
    done_=false;

    //get gen events (fast)
    containerStack gen=csv.getStack("generated events");//this one is normalized.. no chance of getting genevents back

    container1D gensignal=gen.getSignalContainer();
    float nsigsamples=(float) gen.getSignalIdxs().size();
    //  gensignal *=(1/(nsigsamples * 0.1049/0.1111202));
    //extr ttll and tttaull in there twice
    //re-readjust BR here
    if(gensignal.getNBins()<1)
        throw std::runtime_error("ttbarXsecExtractor::readInput: gen events container must have at least 1 bin");

    size_t binno=gensignal.getBinNo(1);
    //    float genstatscaled=gensignal.getBinStat(binno);
    float genscaled=gensignal.getBinContent(binno);


    ngenevents_=genscaled*(1/(nsigsamples * 1/(0.1049/0.1111202)));

    containerStack stack=csv.getStack(plotname);



    std::vector<size_t> nobgidxs=stack.getSignalIdxs();
    nobgidxs.push_back(stack.getDataIdx());

    for(size_t i=0;i<stack.size();i++){
        if(std::find(nobgidxs.begin(),nobgidxs.end(),i) != nobgidxs.end()) continue;

        containerStack copy=stack;
        copy.multiplyNorm(i,1.3);
        stack.addErrorStack(stack.getLegend(i)+"_var_up",copy);
        copy=stack;
        copy.multiplyNorm(i,0.7);
        stack.addErrorStack(stack.getLegend(i)+"_var_down",copy);

    }




    container1D signal=stack.getSignalContainer();
    container1D bkg=stack.getBackgroundContainer();
    container1D data=stack.getContribution("data");

    //add lumi uncertainty
    signal.addGlobalRelError("Lumi",lumierr_);
    bkg.addGlobalRelError("Lumi",lumierr_);
    data.addGlobalRelError("Lumi",0); //no error on data

    /*
    bkg.addGlobalRelError("background",0.3);
    signal.addGlobalRelError("background",0);
    data.addGlobalRelError("background",0);
     */

    //add xsec as parameter!
    //MUST!! be last one
    signal.addGlobalRelError("XSec",0);
    bkg.addGlobalRelError("XSec",0);
    data.addGlobalRelError("XSec",0);


    //now prepare all of those:
    /*
    extendedVariable Lumi_, eps_emu, eps_b, C_b, N_bkg1, N_bkg2;
        extendedVariable n_data1_,n_data2;
     */

    //clear all
    Lumi_.clear();
    eps_emu_.clear();
    eps_b_.clear();
    C_b_.clear();
    N_bkg1_.clear();
    N_bkg2_.clear();
    n_data1_.clear();
    n_data2_.clear();



    std::vector<TString> varnames=signal.getSystNameList();
    //start systematics
    graph tmpg(3);
    tmpg.setPointXContent(0,-1);
    tmpg.setPointXContent(1, 0);
    tmpg.setPointXContent(2, 1);






    ///stat errors here are just polishing th eplots in first order
    // for the moment they are just approximated

    std::vector<TString> paranames;

    simpleFitter::printlevel=-1;

    for(size_t i=0;i<varnames.size();i++){

        varpointers_.clear();

        std::vector<graph> tempgvec;

        const TString & thissys=varnames.at(i);
        sysnames_.push_back(thissys);

        //calc all variables wrt to THIS syst variation
        int sysidxdown=(int)signal.getSystErrorIndex(thissys+"_down");
        int sysidxup=(int)signal.getSystErrorIndex(thissys+"_up");


        ///////////lumi//////////
        if(thissys != "Lumi"){
            tmpg.setPointYContent(0,lumi_);
            tmpg.setPointYContent(1,lumi_);
            tmpg.setPointYContent(2,lumi_);
        }
        else{
            tmpg.setPointYContent(0,lumi_-lumi_*lumierr_);
            tmpg.setPointYContent(1,lumi_);
            tmpg.setPointYContent(2,lumi_+lumi_*lumierr_);

            //eps_emu and lumi have partially same influence...
            //lumi parameter of itself.... hmmm

        }
        tmpg.setPointYStat(0,1);
        tmpg.setPointYStat(1,1);
        tmpg.setPointYStat(2,1);
        Lumi_.addDependence(tmpg,1,thissys);
        tmpg.setName("lumi");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&Lumi_);


        float signalintegraldown=signal.integral(true,sysidxdown);
        float signalintegral=signal.integral(true);
        float signalintegralup=signal.integral(true,sysidxup);

        float signalintegralstat=0, signalintegralstatdown=0, signalintegralstatup=0;
        for(size_t bin=0;bin<signal.getBins().size();bin++){
            signalintegralstat+=signal.getBinStat(bin);
            signalintegralstatdown+=signal.getBinStat(bin,sysidxdown);
            signalintegralstatup+=signal.getBinStat(bin,sysidxup);
        }


        //////////eps_emu//////////
        float eps_emudown=signalintegraldown  /ngenevents_;
        float eps_emu=signalintegral /ngenevents_;
        float eps_emuup=signalintegralup /ngenevents_;

        if(thissys != "Lumi"){
            tmpg.setPointYContent(0,eps_emudown);
            tmpg.setPointYContent(1,eps_emu);
            tmpg.setPointYContent(2,eps_emuup);
        }
        else{
            tmpg.setPointYContent(0,eps_emu);
            tmpg.setPointYContent(1,eps_emu);
            tmpg.setPointYContent(2,eps_emu);
        }


        tmpg.setPointYStat(0,signalintegralstat/ngenevents_);
        tmpg.setPointYStat(1,signalintegralstat/ngenevents_);
        tmpg.setPointYStat(2,signalintegralstat/ngenevents_);

        eps_emu_.addDependence(tmpg,1,thissys);
        tmpg.setName("#epsilon_{e#mu}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&eps_emu_);

        //////////eps_b_ from MC as starting point//////////

        float eps_bdown=signal.getBinContent(signal.getBinNo(1),sysidxdown) / signalintegraldown;
        float eps_b=signal.getBinContent(signal.getBinNo(1)) / signalintegral;
        float eps_bup=signal.getBinContent(signal.getBinNo(1),sysidxup) / signalintegralup;

        tmpg.setPointYContent(0,eps_bdown);
        tmpg.setPointYContent(1,eps_b);
        tmpg.setPointYContent(2,eps_bup);
        tmpg.setPointYStat(0,sqrt(eps_bdown*signalintegraldown));
        tmpg.setPointYStat(1,sqrt(eps_b*signalintegral));
        tmpg.setPointYStat(2,sqrt(eps_bup*signalintegralup));

        eps_b_.addDependence(tmpg,1,thissys);
        tmpg.setName("#epsilon_{b}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&eps_b_);


        //////////C_b_ /////////
        float C_bdown= 4 * signalintegraldown * signal.getBinContent(signal.getBinNo(2),sysidxdown) /
                sq( signal.getBinContent(signal.getBinNo(1),sysidxdown) + 2* signal.getBinContent(signal.getBinNo(2),sysidxdown));
        float C_b=  4 * signalintegral * signal.getBinContent(signal.getBinNo(2))/
                sq( signal.getBinContent(signal.getBinNo(1)) + 2* signal.getBinContent(signal.getBinNo(2)));
        float C_bup= 4 * signalintegralup * signal.getBinContent(signal.getBinNo(2),sysidxup) /
                sq( signal.getBinContent(signal.getBinNo(1),sysidxup) + 2* signal.getBinContent(signal.getBinNo(2),sysidxup));
        //just approximate
        float C_bstat = 4* sqrt( sq(signalintegral * signal.getBinStat(signal.getBinNo(2))) + sq(sqrt(eps_emu*ngenevents_)/lumi_ *  signal.getBinContent(signal.getBinNo(2))))
        / sq( signal.getBinContent(signal.getBinNo(1)) + 2* signal.getBinContent(signal.getBinNo(2)));;

        tmpg.setPointYContent(0,C_bdown);
        tmpg.setPointYContent(1,C_b);
        tmpg.setPointYContent(2,C_bup);
        tmpg.setPointYStat(0,C_bstat);
        tmpg.setPointYStat(1,C_bstat);
        tmpg.setPointYStat(2,C_bstat);

        C_b_.addDependence(tmpg,1,thissys);
        tmpg.setName("C_{b}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&C_b_);


        ////////// N_bkg1_ //////////
        tmpg.setPointYContent(0,bkg.getBinContent(bkg.getBinNo(1),sysidxdown));
        tmpg.setPointYContent(1,bkg.getBinContent(bkg.getBinNo(1)));
        tmpg.setPointYContent(2,bkg.getBinContent(bkg.getBinNo(1),sysidxup));
        tmpg.setPointYStat(0,bkg.getBinStat(bkg.getBinNo(1),sysidxdown));
        tmpg.setPointYStat(1,bkg.getBinStat(bkg.getBinNo(1)));
        tmpg.setPointYStat(2,bkg.getBinStat(bkg.getBinNo(1),sysidxup));

        N_bkg1_.addDependence(tmpg,1,thissys);
        tmpg.setName("N_{bg1}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&N_bkg1_);

        ////////// N_bkg2_ //////////
        tmpg.setPointYContent(0,bkg.getBinContent(bkg.getBinNo(2),sysidxdown));
        tmpg.setPointYContent(1,bkg.getBinContent(bkg.getBinNo(2)));
        tmpg.setPointYContent(2,bkg.getBinContent(bkg.getBinNo(2),sysidxup));
        tmpg.setPointYStat(0,bkg.getBinStat(bkg.getBinNo(2),sysidxdown));
        tmpg.setPointYStat(1,bkg.getBinStat(bkg.getBinNo(2)));
        tmpg.setPointYStat(2,bkg.getBinStat(bkg.getBinNo(2),sysidxup));

        N_bkg2_.addDependence(tmpg,1,thissys);
        tmpg.setName("N_{bg2}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&N_bkg2_);

        ////////// n_data1_ //////////
        tmpg.setPointYContent(0,data.getBinContent(bkg.getBinNo(1),sysidxdown));
        tmpg.setPointYContent(1,data.getBinContent(bkg.getBinNo(1)));
        tmpg.setPointYContent(2,data.getBinContent(bkg.getBinNo(1),sysidxup));
        tmpg.setPointYStat(0,data.getBinStat(bkg.getBinNo(1),sysidxdown));
        tmpg.setPointYStat(1,data.getBinStat(bkg.getBinNo(1)));
        tmpg.setPointYStat(2,data.getBinStat(bkg.getBinNo(1),sysidxup));

        n_data1_.addDependence(tmpg,1,thissys);
        tmpg.setName("N^{data}_{1}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&n_data1_);


        ////////// n_data2_ //////////
        tmpg.setPointYContent(0,data.getBinContent(bkg.getBinNo(2),sysidxdown));
        tmpg.setPointYContent(1,data.getBinContent(bkg.getBinNo(2)));
        tmpg.setPointYContent(2,data.getBinContent(bkg.getBinNo(2),sysidxup));
        tmpg.setPointYStat(0,data.getBinStat(bkg.getBinNo(2),sysidxdown));
        tmpg.setPointYStat(1,data.getBinStat(bkg.getBinNo(2)));
        tmpg.setPointYStat(2,data.getBinStat(bkg.getBinNo(2),sysidxup));

        n_data2_.addDependence(tmpg,1,thissys);
        tmpg.setName("N^{data}_{2}");
        tempgvec.push_back(tmpg);
        varpointers_.push_back(&n_data2_);

        systdeps_.push_back(tempgvec);
        paranames.push_back(thissys);

    }


    //set starting parameters
    std::vector<double> startparas(systdeps_.size()); //includes xsec as
    xsecoffset_= ngenevents_/(Lumi_.getNominal());

    //add eps_b
    startparas.push_back(0.372802);//eps_b_.getNominal()); //from MC
    paranames.push_back("eps_b");

    std::vector<double> stepwidths;
    stepwidths.resize(startparas.size(),1e-20);



    fitter_.setMaxCalls(1e18);
    fitter_.setParameters(startparas,stepwidths);
    fitter_.setParameterNames(paranames);
    fitter_.setRequireFitFunction(false);
    xsecidx_=fitter_.findParameterIdx("XSec");
    eps_bidx_=fitter_.findParameterIdx("eps_b");
   // fitter_.addMinosParameter(xsecidx_);

    std::cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n*********************************************************************" <<std::endl;

    std::cout << "starting values for variables: \n"
            << "eps_b " << startparas.at(eps_bidx_) << "\n"
            << "xsec " << startparas.at(xsecidx_)+xsecoffset_ << "\n";
    std::cout << "variables start values" <<std::endl;
    std::cout << "\nLumi_.getValue(fitter_.getParameters()) " << Lumi_.getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
    std::cout << "eps_emu_.getValue(fitter_.getParameters()) " <<eps_emu_.getValue(&(fitter_.getParameters()->at(0))) << std::endl;
    std::cout << "eps_b_.getValue(fitter_.getParameters()) " <<eps_b_.getValue(&(fitter_.getParameters()->at(0))) << std::endl;
    std::cout << "fitter_.getParameters()->at(eps_bidx_) " << fitter_.getParameters()->at(eps_bidx_) << std::endl;
    std::cout << "fitter_.getParameters()[xsecidx_]+xsecoffset_ " <<fitter_.getParameters()->at(xsecidx_)+xsecoffset_ << std::endl;
    std::cout << "n_data1_.getValue(fitter_.getParameters()) " << n_data1_.getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
    std::cout << "n_data2_.getValue(fitter_.getParameters()) " << n_data2_.getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
    std::cout << "N_bkg1_.getValue(fitter_.getParameters()) " << N_bkg1_.getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
    std::cout << "N_bkg2_.getValue(fitter_.getParameters()) " << N_bkg2_.getValue(&(fitter_.getParameters()->at(0)))<< std::endl;




    std::cout << std::endl;

    setup_=true;
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// chi2 global WA definition ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ttbarXsecExtractor * currentextractor=0;


namespace chi2defs{
void chisqttbarxsec(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){

    if(!currentextractor)
        return;
    float chi2=currentextractor->getChi2(par);
    f=chi2;
    return;
}
}



void ttbarXsecExtractor::extract(){
    if(!setup_){
        throw std::logic_error("ttbarXsecExtractor::extract: first read input");
    }
    currentextractor=this;

    simpleFitter::printlevel=0;
    //fitter_.
    fitter_.fit(chi2defs::chisqttbarxsec);

    xsec_ = fitter_.getParameters()->at(xsecidx_)+xsecoffset_;
    xsecerrup_=fitter_.getParameterErrUp()->at(xsecidx_);
    xsecerrdown_=fitter_.getParameterErrDown()->at(xsecidx_);

    std::cout << "Fitted parameters / starting values" <<std::endl;
    std::cout << "\nLumi_.getValue(fitter_.getParameters()) " << Lumi_.getValue(&(fitter_.getParameters()->at(0)))<< "  " << Lumi_.getNominal() << std::endl;
    std::cout << "eps_emu_.getValue(fitter_.getParameters()) " <<eps_emu_.getValue(&(fitter_.getParameters()->at(0))) << "  " << eps_emu_.getNominal() << std::endl;
    std::cout << "eps_b_.getValue(fitter_.getParameters()) " <<eps_b_.getValue(&(fitter_.getParameters()->at(0))) << "  " << eps_b_.getNominal() << std::endl;
    std::cout << "fitter_.getParameters()->at(eps_bidx_) " << fitter_.getParameters()->at(eps_bidx_) << std::endl;
    std::cout << "fitter_.getParameters()[xsecidx_]+xsecoffset_ " <<fitter_.getParameters()->at(xsecidx_)+xsecoffset_ << std::endl;
    std::cout << "n_data1_.getValue(fitter_.getParameters()) " << n_data1_.getValue(&(fitter_.getParameters()->at(0)))<< "  " << n_data1_.getNominal() << std::endl;
    std::cout << "n_data2_.getValue(fitter_.getParameters()) " << n_data2_.getValue(&(fitter_.getParameters()->at(0)))<< "  " << n_data2_.getNominal() << std::endl;
    std::cout << "N_bkg1_.getValue(fitter_.getParameters()) " << N_bkg1_.getValue(&(fitter_.getParameters()->at(0)))<< "  " << N_bkg1_.getNominal() << std::endl;
    std::cout << "N_bkg2_.getValue(fitter_.getParameters()) " << N_bkg2_.getValue(&(fitter_.getParameters()->at(0)))<< "  " << N_bkg2_.getNominal() << std::endl;




    done_=true;
}





///trivial stuff

float ttbarXsecExtractor::getXsec()const{
    if(!done_){
        return 0;
    }
    return xsec_;
}
float ttbarXsecExtractor::getXsecErrUp()const{
    if(!done_){
        return 0;
    }
    return xsecerrup_;
}
float ttbarXsecExtractor::getXsecErrDown()const{
    if(!done_){
        return 0;
    }
    return xsecerrdown_;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////chi 2 implementation//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ttbarXsecExtractor::getChi2(double * variations)const{



    //  float eps_bvar=eps_b_.getValue(variations);
    float eps_bvar=variations[eps_bidx_];
    float C_bvar  =C_b_.getValue(variations);
    float lumitimesxsectimesepsemuvar=Lumi_.getValue(variations) * (variations[xsecidx_]+xsecoffset_) * eps_emu_.getValue(variations);

    float N_1 = lumitimesxsectimesepsemuvar * 2 * eps_bvar  * (1 - C_bvar*eps_bvar) + N_bkg1_.getValue(variations);

    float N_2 = lumitimesxsectimesepsemuvar * C_bvar *eps_bvar*eps_bvar  + N_bkg2_.getValue(variations);

    float nuisancesum=0;
    for(size_t i=0;i<fitter_.getParameters()->size();i++){

        if(i==xsecidx_ || i==eps_bidx_) continue; //don't put xsec/eps_b (last para ) here!!!

        //remember all are defined in terms of sigma
        // so (val - def) / sigma = val - def
        // and default is defined as 0 so

        float delta=variations[i];
        nuisancesum += delta*delta;
    }
    float ndata1var=n_data1_.getValue(variations);
    float ndata2var=n_data2_.getValue(variations);

    float delta1 = (ndata1var - N_1);
    float delta2 = (ndata2var - N_2);

    double chi2= delta1*delta1/ndata1var  + delta2* delta2/ndata2var + nuisancesum;

    bool verbose=false;
    if(verbose){
        std::cout << "\nLumi_.getValue(variations) " << Lumi_.getValue(variations)<< std::endl;
        std::cout << "eps_emu_.getValue(variations) " <<eps_emu_.getValue(variations) << std::endl;
        std::cout << "C_b_.getValue(variations) " <<C_b_.getValue(variations) << std::endl;
        std::cout << "eps_b_.getValue(variations) " <<eps_b_.getValue(variations) << std::endl;
        std::cout << "variations[xsecidx_]+xsecoffset_ " <<variations[xsecidx_]+xsecoffset_ << std::endl;
        std::cout << "n_data1_.getValue(variations) " << n_data1_.getValue(variations)<< std::endl;
        std::cout << "n_data2_.getValue(variations) " << n_data2_.getValue(variations)<< std::endl;
        std::cout << "N_1 " <<N_1 << std::endl;
        std::cout << "N_2 " <<N_2 << std::endl;
        std::cout << "N_bkg1_.getValue(variations) " << N_bkg1_.getValue(variations)<< std::endl;
        std::cout << "N_bkg2_.getValue(variations) " << N_bkg2_.getValue(variations)<< std::endl;
        std::cout << "chi2: " << chi2 <<std::endl;
    }
    //   throw std::runtime_error("");
    //std::cout << "chi2: " << chi2 <<std::endl;
    // throw std::runtime_error("");
    return chi2;

    // return fabs(nuisancesum-1);

}

}
