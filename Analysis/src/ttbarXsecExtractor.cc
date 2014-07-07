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
#include "Math/Functor.h"

namespace ztop{

ttbarXsecExtractor::ttbarXsecExtractor():setup_(false),done_(false),lumi_(19741),
        lumierr_(0.025),ngenevents_(1),xsec_(1),xsecerrup_(1),xsecerrdown_(1),xsecidx_(0),eps_bidx_(0),xsecoffset_(250),
        minchi2_(1e9),pseudodata_(false),mainminimzstr_("Minuit2"),mainminmzalgo_("Migrad"),binssize_(0),usepoisson_(true){
    fitter_.setRequireFitFunction(false);

}

void ttbarXsecExtractor::readInput(const containerStackVector & csv, const TString& plotname, const TString& plotnameprofile){
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


    ngenevents_=genscaled*(1/(nsigsamples* (0.1049/0.1111202)));

    if(pseudodata_)
        ngenevents_*=0.9;

    //translate to 0, 1, 2 bjet categories
    std::vector<TString> plotnames;
    for(size_t i=0;i<3;i++){
        TString number=toTString(i);
        TString newname=plotname;
        newname.ReplaceAll("<N>",number);
        plotnames.push_back(newname);
    }
    TString lastplotname=plotname;
    lastplotname.ReplaceAll("<N>","3+");
    plotnames.push_back(lastplotname);

    //get the corresponding stacks and containers
    float bkgvar=0.3;
    binssize_=0;
    std::vector<container1D> signals,backgrounds,datas;
    for(size_t i=0;i<plotnames.size();i++){

        containerStack stack=csv.getStack(plotnames.at(i));

        std::vector<size_t> nobgidxs=stack.getSignalIdxs();
        nobgidxs.push_back(stack.getDataIdx());



        for(size_t i=0;i<stack.size();i++){
            if(std::find(nobgidxs.begin(),nobgidxs.end(),i) != nobgidxs.end()) continue;
            float variation=bkgvar;
            if(stack.getLegend(i) == "singleTop")
                variation=0.23;
            containerStack copy=stack;
            copy.multiplyNorm(i,1+variation);
            stack.addErrorStack(stack.getLegend(i)+"_var_up",copy);
            copy=stack;
            copy.multiplyNorm(i,1-variation);
            stack.addErrorStack(stack.getLegend(i)+"_var_down",copy);

        }

        container1D signal=stack.getSignalContainer();
        container1D bkg=stack.getBackgroundContainer();
        container1D data=stack.getContribution("data");

        //add lumi uncertainty
        signal.addGlobalRelError("Lumi",lumierr_);
        bkg.addGlobalRelError("Lumi",lumierr_);
        data.addGlobalRelError("Lumi",0); //no error on data



        //add xsec as parameter!
        //MUST!! be last one
        signal.addGlobalRelError("XSec",0);
        bkg.addGlobalRelError("XSec",0);
        data.addGlobalRelError("XSec",0);

        //tesing reasons
        /*
        std::vector<float> newbins;
        newbins.push_back(signal.getBins().at(1));
         newbins.push_back(signal.getBins().at(signal.getBins().size()-1));

        signal.rebinToBinning(newbins);
        bkg.rebinToBinning(newbins);
        data.rebinToBinning(newbins);
         */

        if(binssize_>0 && signal.getBins().size()!=binssize_){
            throw std::runtime_error("not all plots read in have same binning!!");
        }
        else{
            binssize_=signal.getBins().size();
        }

        signals.push_back(signal);
        backgrounds.push_back(bkg);
        datas.push_back(data);
    }


    std::vector<TString> varnames=signals.at(0).getSystNameList();

    //check for empty bins (all systematics!)
    emptybins_.clear();
    for(size_t vari=0;vari<varnames.size();vari++){

        const TString & thissys=varnames.at(vari);
        int sysidxdown=(int)signals.at(0).getSystErrorIndex(thissys+"_down");
        int sysidxup=(int)signals.at(0).getSystErrorIndex(thissys+"_up");

        for(size_t bin=0;bin<binssize_;bin++){
            bool emptybin=false;
            for(size_t njets=0;njets<3;njets++){ //only check firt 3!!! 0,1,2

                if(signals.at(njets).getBinContent(bin,sysidxdown)<1
                        || signals.at(njets).getBinContent(bin)<1
                        || signals.at(njets).getBinContent(bin,sysidxup)<1)
                    emptybin=true;
                if(datas.at(njets).getBinContent(bin,sysidxdown)<1
                        || datas.at(njets).getBinContent(bin)<1
                        || datas.at(njets).getBinContent(bin,sysidxup)<1)
                    emptybin=true;
            }
            if(emptybin)
                emptybins_.push_back(bin);
        }
    }
    //  extendedVariable::debug=true;


    simpleFitter::printlevel=0;

    varpointers_.clear();
    alldepsgraphs_.clear();

    Lumi_.resize(binssize_);
    eps_emu_.resize(binssize_);
    eps_b_.resize(binssize_);
    C_b_.resize(binssize_);
    N_bkg0_.resize(binssize_);
    N_bkg1_.resize(binssize_);
    N_bkg2_.resize(binssize_);
    n_data0_.resize(binssize_);
    n_data1_.resize(binssize_);
    n_data2_.resize(binssize_);

    for(size_t bin=0;bin<binssize_;bin++){

        sysnames_.clear();
        TString binnumber=toTString(bin);

        Lumi_.at(bin).setName("Lumi_"+binnumber);
        eps_emu_.at(bin).setName("eps_emu_"+binnumber);
        eps_b_.at(bin).setName("eps_b_"+binnumber);
        C_b_.at(bin).setName("C_b_"+binnumber);
        N_bkg0_.at(bin).setName("N_bkg0_"+binnumber);
        N_bkg1_.at(bin).setName("N_bkg1_"+binnumber);
        N_bkg2_.at(bin).setName("N_bkg2_"+binnumber);
        n_data0_.at(bin).setName("n_data0_"+binnumber);
        n_data1_.at(bin).setName("n_data1_"+binnumber);
        n_data2_.at(bin).setName("n_data2_"+binnumber);

        bool emptybin=false;
        if(std::find(emptybins_.begin(),emptybins_.end(),bin) != emptybins_.end()){
            std::cout << "empty bin " << bin <<std::endl;
            emptybin=true;
        }

        for(size_t vari=0;vari<varnames.size();vari++){

            const TString & thissys=varnames.at(vari);
            //  std::cout << thissys << " " << bin<<std::endl;


            sysnames_.push_back(thissys);


            //calc all variables wrt to THIS syst variation
            int sysidxdown=(int)signals.at(0).getSystErrorIndex(thissys+"_down");
            int sysidxup=(int)signals.at(0).getSystErrorIndex(thissys+"_up");


            ///////////lumi//////////
            if(thissys != "Lumi"){
                alldepsgraphs_.push_back(Lumi_.at(bin).addDependence(lumi_,lumi_,lumi_,thissys));
            }
            else{
                alldepsgraphs_.push_back(Lumi_.at(bin).addDependence(lumi_-lumi_*lumierr_,lumi_,lumi_+lumi_*lumierr_,thissys));
            }

            varpointers_.push_back(&Lumi_.at(bin));
            combused_.push_back(!emptybin);

            //integrals include all njet bins
            float signalintegraldown=0;
            float signalintegral=0;
            float signalintegralup=0;


            for(size_t njets=0;njets<plotnames.size();njets++){ //also take into account 3+ jets
                signalintegraldown+=signals.at(njets).getBinContent(bin,sysidxdown);
                signalintegral+=signals.at(njets).getBinContent(bin);
                signalintegralup+=signals.at(njets).getBinContent(bin,sysidxup);


            }

            //////////eps_emu//////////
            float eps_emudown=signalintegraldown  /ngenevents_;
            float eps_emu=signalintegral /ngenevents_;
            float eps_emuup=signalintegralup /ngenevents_;


            if(thissys != "Lumi"){
                alldepsgraphs_.push_back(eps_emu_.at(bin).addDependence(eps_emudown,eps_emu,eps_emuup,thissys));
            }
            else{
                alldepsgraphs_.push_back(eps_emu_.at(bin).addDependence(eps_emu,eps_emu,eps_emu,thissys));
            }
            varpointers_.push_back(&eps_emu_.at(bin));
            combused_.push_back(!emptybin);

            //////////eps_b_ from MC as starting point//////////
            //needs C_b as input
            float C_bdown= 4 * signalintegraldown * signals.at(2).getBinContent(bin,sysidxdown) /
                    sq( signals.at(1).getBinContent(bin,sysidxdown) + 2* signals.at(2).getBinContent(bin,sysidxdown));
            float C_b=  4 * signalintegral * signals.at(2).getBinContent(bin)/
                    sq( signals.at(1).getBinContent(bin) + 2* signals.at(2).getBinContent(bin));
            float C_bup= 4 * signalintegralup * signals.at(2).getBinContent(bin,sysidxup) /
                    sq( signals.at(1).getBinContent(bin,sysidxup) + 2* signals.at(2).getBinContent(bin,sysidxup));

            //nan / inf protection

            float eps_bdown = 1
                    /((1+ signals.at(1).getBinContent(bin,sysidxdown)/(2*signals.at(2).getBinContent(bin,sysidxdown)))  *C_bdown);
            float eps_b =1
                    /((1+ signals.at(1).getBinContent(bin)/(2*signals.at(2).getBinContent(bin)))  *C_b);
            float eps_bup =1
                    /((1+ signals.at(1).getBinContent(bin,sysidxup)/(2*signals.at(2).getBinContent(bin,sysidxup)))  *C_bup);

            if(emptybin){
                C_b=1;C_bdown=1;C_bup=1;
                eps_bdown=0;eps_b=0;eps_bup=0;
            }


            alldepsgraphs_.push_back(eps_b_.at(bin).addDependence(eps_bdown,eps_b,eps_bup,thissys));
            varpointers_.push_back(&eps_b_.at(bin));
            combused_.push_back(!emptybin);


            //////////C_b_ /////////


            alldepsgraphs_.push_back(C_b_.at(bin).addDependence(C_bdown,C_b,C_bup,thissys));
            varpointers_.push_back(&C_b_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(N_bkg0_.at(bin).addDependence(backgrounds.at(0).getBinContent(bin,sysidxdown),backgrounds.at(0).getBinContent(bin),backgrounds.at(0).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&N_bkg0_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(N_bkg1_.at(bin).addDependence(backgrounds.at(1).getBinContent(bin,sysidxdown),backgrounds.at(1).getBinContent(bin),backgrounds.at(1).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&N_bkg1_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(N_bkg2_.at(bin).addDependence(backgrounds.at(2).getBinContent(bin,sysidxdown),backgrounds.at(2).getBinContent(bin),backgrounds.at(2).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&N_bkg2_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(n_data0_.at(bin).addDependence(datas.at(0).getBinContent(bin,sysidxdown),datas.at(0).getBinContent(bin),datas.at(0).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&n_data0_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(n_data1_.at(bin).addDependence(datas.at(1).getBinContent(bin,sysidxdown),datas.at(1).getBinContent(bin),datas.at(1).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&n_data1_.at(bin));
            combused_.push_back(!emptybin);

            alldepsgraphs_.push_back(n_data2_.at(bin).addDependence(datas.at(2).getBinContent(bin,sysidxdown),datas.at(2).getBinContent(bin),datas.at(2).getBinContent(bin,sysidxup),thissys));
            varpointers_.push_back(&n_data2_.at(bin));
            combused_.push_back(!emptybin);


        }


    }
    if(Lumi_.size()<1){
        throw std::runtime_error("No info for variables found");
    }


    std::vector<double> startparas(varnames.size(),0); //includes xsec as
    xsecoffset_= ngenevents_/(Lumi_.at(0).getNominal());

    std::vector<double> stepwidths;
    stepwidths.resize(startparas.size(),1e-7);

    std::vector<TString> paranames=sysnames_;
    paranames.push_back("XSec");

    fitter_.setParameterNames(paranames);
    fitter_.setRequireFitFunction(false);
    xsecidx_=fitter_.findParameterIdx("XSec");
    stepwidths.at(xsecidx_) = 0.001;
    fitter_.setParameters(startparas,stepwidths);


    outstream_ <<"\n\n\n\n\n\n\n\n\n\n\n\n\n*********************************************************************" <<std::endl;

    outstream_ << "starting values for variables: \n"
            << "xsec " << startparas.at(xsecidx_)+xsecoffset_ << "\n";

    outstream_ << "variables start values" <<std::endl;
    for(size_t bin=0;bin<binssize_;bin++){

        if(std::find(emptybins_.begin(),emptybins_.end(),bin) != emptybins_.end()) continue;
        outstream_ << "\nbin "<<bin<<std::endl;
        outstream_ << "Lumi_.getValue(fitter_.getParameters()) " << Lumi_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "eps_emu_.getValue(fitter_.getParameters()) " <<eps_emu_.at(bin).getValue(&(fitter_.getParameters()->at(0))) << std::endl;
        outstream_ << "eps_b_.getValue(fitter_.getParameters()) " <<eps_b_.at(bin).getValue(&(fitter_.getParameters()->at(0))) << std::endl;
        outstream_ << "n_data0_.getValue(fitter_.getParameters()) " << n_data0_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "n_data1_.getValue(fitter_.getParameters()) " << n_data1_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "n_data2_.getValue(fitter_.getParameters()) " << n_data2_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "N_bkg0_.getValue(fitter_.getParameters()) " << N_bkg0_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "N_bkg1_.getValue(fitter_.getParameters()) " << N_bkg1_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;
        outstream_ << "N_bkg2_.getValue(fitter_.getParameters()) " << N_bkg2_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< std::endl;

    }

    outstream_ << "fitter_.getParameters()[xsecidx_]+xsecoffset_ " <<fitter_.getParameters()->at(xsecidx_)+xsecoffset_ << std::endl;


    outstream_ << std::endl;

    /*
     * profile input
     */
    if(plotnameprofile==""){
        setup_=true;
        return;
    }
/*
    ////process profiled input
    containerStack profile=csv.getStack(plotnameprofile);
    if(profile.size()<1)
        throw std::runtime_error("input profile stack not found");
    size_t nbins=profile.getContainer(0).getBins().size();

container1D data=profile.getContainer( profile.getDataIdx());

    for(size_t bin=0;bin<nbins;bin++){
        for(size_t sys=0;sys<profile.getContainer(0).getSystSize();sys++){


        }
    }

*/
    setup_=true;
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// chi2 global WA definition ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ttbarXsecExtractor::extract(){
    if(!setup_){
        throw std::logic_error("ttbarXsecExtractor::extract: first read input");
    }



    fitter_.setMinimizer(simpleFitter::mm_minuit2);
    fitter_.setMaxCalls(4e6);
    //fitter_.setTolerance(0.5);//0.1);
    //first do a simple scan


    ROOT::Math::Functor f(this,&ttbarXsecExtractor::getChi2,fitter_.getParameters()->size());

    fitter_.setMinFunction(&f);

    simpleFitter::printlevel=0;


    fitter_.setMinimizer(mainminimzstr_);
    fitter_.setAlgorithm(mainminmzalgo_);
    //get better starting values
    fitter_.setTolerance(0.01);
    // fitter_.addMinosParameter(xsecidx_);
    fitter_.fit();



    xsec_ = fitter_.getParameters()->at(xsecidx_)+xsecoffset_;
    xsecerrup_=fitter_.getParameterErrUp()->at(xsecidx_);
    xsecerrdown_=fitter_.getParameterErrDown()->at(xsecidx_);

    float avgerror=(fabs(xsecerrup_)+fabs(xsecerrdown_))/2;


    outstream_ << "Fitted parameters / starting values" <<std::endl;
    for(size_t bin=0;bin<binssize_;bin++){
        if(std::find(emptybins_.begin(),emptybins_.end(),bin) != emptybins_.end()) continue;
        outstream_ << "\nbin "<<bin<<std::endl;
        outstream_ << "Lumi_.at(bin).getValue(fitter_.getParameters()) " << Lumi_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << Lumi_.at(bin).getNominal() << std::endl;
        outstream_ << "eps_emu_.at(bin).getValue(fitter_.getParameters()) " <<eps_emu_.at(bin).getValue(&(fitter_.getParameters()->at(0))) << "  " << eps_emu_.at(bin).getNominal() << std::endl;
        outstream_ << "eps_b_.at(bin).getValue(fitter_.getParameters()) " <<eps_b_.at(bin).getValue(&(fitter_.getParameters()->at(0))) << "  " << eps_b_.at(bin).getNominal() << std::endl;
        outstream_ << "n_data0_.at(bin).getValue(fitter_.getParameters()) " << n_data0_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << n_data0_.at(bin).getNominal() << std::endl;
        outstream_ << "n_data1_.at(bin).getValue(fitter_.getParameters()) " << n_data1_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << n_data1_.at(bin).getNominal() << std::endl;
        outstream_ << "n_data2_.at(bin).getValue(fitter_.getParameters()) " << n_data2_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << n_data2_.at(bin).getNominal() << std::endl;
        outstream_ << "N_bkg0_.at(bin).getValue(fitter_.getParameters()) " << N_bkg0_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << N_bkg0_.at(bin).getNominal() << std::endl;
        outstream_ << "N_bkg1_.at(bin).getValue(fitter_.getParameters()) " << N_bkg1_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << N_bkg1_.at(bin).getNominal() << std::endl;
        outstream_ << "N_bkg2_.at(bin).getValue(fitter_.getParameters()) " << N_bkg2_.at(bin).getValue(&(fitter_.getParameters()->at(0)))<< "  " << N_bkg2_.at(bin).getNominal() << std::endl;
    }

    if(!fitter_.wasSuccess()){
        outstream_ << "\n\n********************************\nFit was not successful! result should not be trusted.\n********************************" <<std::endl;
        throw std::runtime_error("Fit was not successful!");
    }

    //put parameters to output

    for(size_t i=0;i<fitter_.getParameters()->size();i++){
        float breakdown=0;
        breakdown=sqrt(sq(avgerror)-sq(avgerror * sqrt(1-sq(fitter_.getCorrelationCoefficient(xsecidx_,i)))));
        breakdown/=xsec_;
        breakdown*=100;
        outstream_ << fitter_.getParameters()->at(i) << "\t"<< fitter_.getParameterErr(i) << "\t"
                <<fitter_.getParameterNames()->at(i)<< "\t"<< breakdown<<"%" <<std::endl;

    }
    outstream_ << "\nfitter_.getParameters()[xsecidx_]+xsecoffset_ " <<fitter_.getParameters()->at(xsecidx_)+xsecoffset_ << std::endl;
    outstream_ << "best chi2 / ndof: " << minchi2_ << "/" << fitter_.getParameters()->size()-1 << "=" << minchi2_/(fitter_.getParameters()->size()-1) <<  std::endl;


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

bool ttbarXsecExtractor::isEmptyBin(size_t bin)const{

    return std::find(emptybins_.begin(),emptybins_.end(),bin) != emptybins_.end();

}
void ttbarXsecExtractor::coutOutStream()const{
    std::cout << outstream_.rdbuf();
}
TString ttbarXsecExtractor::dumpOutStream()const{
    return (TString)outstream_.str();
}
void ttbarXsecExtractor::clearOutStream(){
    outstream_.clear();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////chi 2 implementation//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//work with doubles here to have larger precision (2nd derivatives in hesse)
double ttbarXsecExtractor::getChi2(const double * variations){

    //   std::vector<double> variations(variationsp,fitter_.getParameters()->size()-1);


    double datasum=0;

    bool usepoisson=usepoisson_;

    for(size_t i=0;i<binssize_;i++){


        if(std::find(emptybins_.begin(),emptybins_.end(),i) != emptybins_.end()) //don't consider empty bins
            continue;


        //////space for additional constraints

        double ndata0var=n_data0_.at(i).getValue(variations);
        double ndata1var=n_data1_.at(i).getValue(variations);
        double ndata2var=n_data2_.at(i).getValue(variations);

        double eps_bvar=eps_b_.at(i).getValue(variations);
        //  double eps_bvar=variations[eps_bidx_];
        double C_bvar  =C_b_.at(i).getValue(variations);
        double lumitimesxsectimesepsemuvar=Lumi_.at(i).getValue(variations) * (variations[xsecidx_]+xsecoffset_) * eps_emu_.at(i).getValue(variations);

        double prob1jet=2 * eps_bvar  * (1 - C_bvar*eps_bvar);

        double N_1 = lumitimesxsectimesepsemuvar * prob1jet + N_bkg1_.at(i).getValue(variations);

        double prob2jet=C_bvar *eps_bvar*eps_bvar;

        double N_2 = lumitimesxsectimesepsemuvar * prob2jet  + N_bkg2_.at(i).getValue(variations);

        double N_0 = lumitimesxsectimesepsemuvar * (1- prob1jet) * (1-prob2jet) + N_bkg0_.at(i).getValue(variations);

        double delta0 = (ndata0var - N_0);
        double delta1 = (ndata1var - N_1);
        double delta2 = (ndata2var - N_2);


        //poisson implementation!
        if(usepoisson){
            datasum+= -2 * logPoisson(ndata0var,N_0);
            datasum+= -2 * logPoisson(ndata1var,N_1);
            datasum+= -2 * logPoisson(ndata2var,N_2);
        }
        else{//gaussian approx
            datasum+=delta0*delta0/ndata0var;
            datasum+=delta1*delta1/ndata1var;
            datasum+=delta2* delta2/ndata2var;
        }

    }
    ///////nuisance part

    double nuisancesum=0;
    bool coutall=false;
    for(size_t i=0;i<fitter_.getParameters()->size();i++){

        if(i==xsecidx_) continue; // || i==eps_bidx_) continue; //don't put xsec/eps_b (last para ) here!!!

        //remember all are defined in terms of sigma
        // so (val - def) / sigma = val - def
        // and default is defined as 0 so
        if(variations[i]!=variations[i] || variations[i] >40 || variations[i] < -40){
            coutall=true;
        }
        double delta=variations[i];
        nuisancesum += delta*delta;
    }


    ////final chi2 and several sanity checks (sometimes strage minuit behaviour)

    double chi2= datasum + nuisancesum;
    if(chi2 < 0)
        throw std::logic_error("ttbarXsecExtractor::getChi2: chi2 value became <0 !! serious error");


    bool verbose=false;
    if(chi2 != chi2){
        verbose=true; //this is a nan case
        std::cout << "warning NAN produced in chi2... setting high value" <<std::endl;
        chi2=minchi2_*1e3;
    }

    if(coutall){
        std::cout <<std::endl;
        for(size_t i=0;i<fitter_.getParameters()->size();i++){
            std::cout << variations[i] << std::endl;
        }
        std::cout <<std::endl;
        verbose=true;
    }

    if(verbose){
        for(size_t i=0;i<binssize_;i++){
            std::cout << "\nLumi_.getValue(variations) " << Lumi_.at(i).getValue(variations)<< std::endl;
            std::cout << "eps_emu_.getValue(variations) " <<eps_emu_.at(i).getValue(variations) << std::endl;
            std::cout << "C_b_.getValue(variations) " <<C_b_.at(i).getValue(variations) << std::endl;
            std::cout << "eps_b_.getValue(variations) " <<eps_b_.at(i).getValue(variations) << std::endl;
            std::cout << "n_data1_.getValue(variations) " << n_data1_.at(i).getValue(variations)<< std::endl;
            std::cout << "n_data2_.getValue(variations) " << n_data2_.at(i).getValue(variations)<< std::endl;
            std::cout << "N_bkg1_.getValue(variations) " << N_bkg1_.at(i).getValue(variations)<< std::endl;
            std::cout << "N_bkg2_.getValue(variations) " << N_bkg2_.at(i).getValue(variations)<< std::endl;
        }
        std::cout << "variations[xsecidx_]+xsecoffset_ " <<variations[xsecidx_]+xsecoffset_ << std::endl;
        std::cout << "chi2: " << chi2 <<std::endl;

    }


    if(minchi2_>chi2)
        minchi2_=chi2;

    return chi2;


}

}
