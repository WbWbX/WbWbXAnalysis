/*
 * histoUnfolder.cc
 *
 *  Created on: Sep 4, 2013
 *      Author: kiesej
 */

#include "../interface/histoUnfolder.h"
#include <omp.h>
#include <stdexcept>

/*
class histoUnfolder{
public:
	histoUnfolder(): unfnominal_(0),usecontoptions_(true){}
	~histoUnfolder();

	void setOptions();
	histo1D unfold(const histo1DUnfold &);

	const std::vector<unfolder * > & getSystUnfolder() const {return unfsyst_;}
	unfolder * getNominalUnfolder() const {return unfnominal_;}

private:
	unfolder * unfnominal_;
	std::vector<unfolder * > unfsyst_;
	bool usecontoptions_;

}//namespace
 */

namespace ztop{


bool histoUnfolder::printinfo=false;
bool histoUnfolder::debug=false;



histoUnfolder::~histoUnfolder(){
	clear();
	//delete option pointers?
}

void histoUnfolder::clear(){//don't delete options
	if(unfnominal_){
		delete unfnominal_;
		unfnominal_=0;
	}
	for(size_t i=0;i<unfsyst_.size();i++)
		delete unfsyst_.at(i);
	unfsyst_.clear();
}
/**
 * implement later, right now, just use default ones
 */
void histoUnfolder::setOptions(){

}

histo1D histoUnfolder::binbybinunfold(histo1DUnfold & cuf){

	//efficiency:
	bool tempmakelist=histo1D::c_makelist;
	histo1D::c_makelist=false;

	if(debug)
		std::cout << "containerUnfolder::binbybinunfold "<< cuf.getName() << std::endl;
	//get full gen&reco
	histo1D genreco=cuf.projectToY(false);
	histo1D gen=cuf.getBinnedGenContainer();
	genreco=genreco.rebinToBinning(gen); //make same bins

	//operations
	bool temp=histoContent::divideStatCorrelated; //all same named syst,nom, etc are assumed to be stat corr
	histoContent::divideStatCorrelated=true;
	if(debug)
		std::cout << "containerUnfolder::binbybinunfold: creating efficiencies: (bin1): ";
	histo1D effinv=gen/genreco;
	histoContent::divideStatCorrelated=temp;
	if(debug)
		std::cout << 1/effinv.getBinContent(1) << std::endl;

	temp=histoContent::subtractStatCorrelated;
	histoContent::subtractStatCorrelated=false;
	if(debug)
		std::cout << "containerUnfolder::binbybinunfold: getting N_sel - N_bg (bin1) ";
	histo1D sel=cuf.getRecoContainer();
	histo1D bg=cuf.getBackground();
	histo1D smbg=sel - bg;
	histoContent::subtractStatCorrelated=temp;
	if(debug)
		std::cout << smbg.getBinContent(1) << std::endl;

	temp=histoContent::multiplyStatCorrelated;
	histoContent::multiplyStatCorrelated=false;
	if(debug)
		std::cout << "containerUnfolder::binbybinunfold: rebinning N_sel-N_bg to efficiencies" << std::endl;
	histo1D smbgrb=smbg.rebinToBinning(effinv);
	if(debug)
		std::cout << "containerUnfolder::binbybinunfold: compute xsec (w/o lumi)" << std::endl;
	histo1D xsec=smbgrb * effinv;
	histoContent::multiplyStatCorrelated=temp;

	if(debug)
		std::cout << "containerUnfolder::binbybinunfold: correct for lumi: " <<cuf.getLumi()<< std::endl;
	double lumiinv=1/cuf.getLumi();
	xsec = xsec * lumiinv;
	cuf.setUnfolded(xsec);
	cuf.setRefolded(cuf.getRecoContainer());
	histo1D::c_makelist=tempmakelist;
	return xsec;

}
histo1D histoUnfolder::unfold(/*const*/ histo1DUnfold & cuf){

	bool tempmakelist=histo1D::c_makelist;
	histo1D::c_makelist=false;
	histo1D out,refolded;

	if(cuf.getBackground().integral() > cuf.getRecoContainer().integral() * 0.55){
		std::cout << "containerUnfolder::unfold: " << cuf.getName() <<  " has more background than signal ("
				<<cuf.getBackground().integral()/cuf.getRecoContainer().integral()
				<< "%), skipping" <<std::endl;
		return histo1D();
	}
	if(cuf.getRecoContainer().integral() == 0){
		std::cout << "containerUnfolder::unfold: " << cuf.getName() << " empty, skipping" <<std::endl;
		return histo1D();
	}

	if(cuf.isBinByBin()){
		histo1D out=binbybinunfold(cuf);
		return out;
	}
	if(cuf.getBinsX().size()<2){
		std::cout << "containerUnfolder::unfold: " << cuf.getName() << " has no bins, skipping" <<std::endl;
		return histo1D();

	}

	if(cuf.getBinsX().size() >= cuf.getBinsY().size()){
		std::vector<float> newbins;
		for(size_t i=1;i<cuf.getBinsX().size();i++){
			if(i%2)
				newbins.push_back(cuf.getBinsX().at(i));
		}
		if(newbins.at(newbins.size()-1) != cuf.getBinsX().at(cuf.getBinsX().size()-1))
			newbins.push_back(cuf.getBinsX().at(cuf.getBinsX().size()-1));
		cuf.setGenBinning(newbins);
		std::cout << "containerUnfolder::unfold: rebinned input!"<<std::endl;
	}




	clear();
	TUnfold::ERegMode regmode=regmode_;

	/*
	 *
	 * preparation nominal
	 */
	bool outisBWdiv=false; //for debugging purposes
	TH1::AddDirectory(false);
	if(debug)
		std::cout << "containerUnfolder::unfold: preparing response matrix" << std::endl;
	TH2* responsematrix=cuf.prepareRespMatrix();
	unfnominal_ = new unfolder(cuf.getName()+"_nominal");
	unfnominal_->setVerbose(printinfo);
	TH1* datahist=cuf.getRecoContainer().getTH1D(cuf.getName()+"_datahist_nominal",false,true);
	if(debug)
		std::cout << "containerUnfolder::unfold: initialising unfolder" << std::endl;
	unfnominal_->init(responsematrix,datahist,TUnfold::kHistMapOutputHoriz,regmode);
	if(debug)
		std::cout << "containerUnfolder::unfold: scanning L-Curve/Tau" << std::endl;
	if(scanmode_==LCurve)
		unfnominal_->scanLCurve();
	else
		unfnominal_->scanTau();
	delete responsematrix;
	delete datahist;
	if(debug)
		std::cout << "containerUnfolder::unfold: getting output" << std::endl;
	if(unfnominal_->ready()){
		out.import(unfnominal_->getUnfolded(),outisBWdiv);
		refolded.import(unfnominal_->getRefolded(),outisBWdiv);
	}
	else{
		std::cout << "containerUnfolder::unfold: unfolding for nominal failed!! skipping rest" << std::endl;
		return histo1D();
	}
	if(debug)
		std::cout << "containerUnfolder::unfold: unfolding done for nominal" << std::endl;
	//done for nominal

	/*
	 * prepare loop for systematics
	 */
	bool dodatasyst=true;
	if(cuf.getRecoContainer().getSystSize() != 0 && cuf.getRecoContainer().getSystSize() != cuf.getSystSize()){
		std::cout << "WARNING! containerUnfolder::unfold: ambiguous association of data systematics and MC systematics. "
				<< "data will be unfolded without systematic variations." << std::endl;
		dodatasyst=false;
	}
	if(cuf.getRecoContainer().getSystSize() <1){
		std::cout << "containerUnfolder::unfold: dataContainer has no info about systematics. If intended, ok, if not, check!" << std::endl;
		dodatasyst=false;
	}
	if(debug)
		std::cout << "containerUnfolder::unfold: init systematics" << std::endl;
	unfsyst_.resize(cuf.getSystSize(),0);

	for(size_t sys=0;sys<cuf.getSystSize();sys++){
		if(std::find(ignoresyst_.begin(),ignoresyst_.end(),cuf.getSystErrorName(sys)) != ignoresyst_.end())
			continue;

		TH2* SysResponsematrix=cuf.prepareRespMatrix(false,sys);
		if(!SysResponsematrix){
			std::cout << "containerUnfolder::unfold: Response matrix for " << cuf.getSystErrorName(sys)
			        		<< " could not be created. stopping unfolding!" << std::endl;
			throw std::runtime_error("");
		}
		//SysResponsematrix->Draw("COLZ");
		TH1* SysDatahist=0;
		if(dodatasyst)
			SysDatahist=cuf.getRecoContainer().getTH1DSyst(cuf.getName()+"_datahist_"+cuf.getSystErrorName(sys),sys,false,true);
		else
			SysDatahist=cuf.getRecoContainer().getTH1D(cuf.getName()+"_datahist_nominal_fake"+cuf.getSystErrorName(sys),false,true);
		unfolder * uf=0;

		try{
			uf=new unfolder(cuf.getName()+"_"+cuf.getSystErrorName(sys));
		}
		catch(...){

			{
				std::cout << "containerUnfolder::unfold: bad memory alloc by new unfolder , try to switch of parallalization" << std::endl;

			}
			continue;
		}
		uf->setVerbose(printinfo);
		int verb=uf->init(SysResponsematrix,SysDatahist);
		if(verb>10000){

			{
				std::cout << "containerUnfolder::unfold: init of unfolder for "<< cuf.getName()<< ": "
						<< cuf.getSystErrorName(sys) << " not successful. Will not unfold distribution"<< std::endl;
			}
			continue;
		}
		unfsyst_.at(sys)=uf;
		delete SysResponsematrix;
		delete SysDatahist;
	}
	/*
	 * there might be an addional loop needed if MC independent data systematics are introduced not given in the list of syst.
	 * if filling is setup properly, this will not happen
	 */


	/*
	 * scanning L-Curve loop for systematics.
	 * parallelise this one!!
	 */
	if(debug)
		std::cout << "containerUnfolder::unfold: scanning L-Curve/Tau of systematics" << std::endl;
	//unfortunately not really thread safe..
//#pragma omp parallel for
	for(size_t sys=0;sys<unfsyst_.size();sys++){
		if(debug||printinfo)
			std::cout << "containerUnfolder::unfold: scanning L-Curve/Tau of "<< cuf.getName() << ": " << cuf.getSystErrorName(sys) << std::endl;
		if(scanmode_==LCurve)
			unfsyst_.at(sys)->scanLCurve();
		else
			unfsyst_.at(sys)->scanTau();
	}
	/*
	 * getting output and merging it into unfolded_ etc. should be ok in serial mode
	 */
	for(size_t sys=0;sys<unfsyst_.size();sys++){
		TH1 * huf=unfsyst_.at(sys)->getUnfolded();
		TH1 * hrf=unfsyst_.at(sys)->getRefolded();
		if(!huf){
			std::cout << "containerUnfolder::unfold: unfolding result for " << cuf.getName() << ": " << cuf.getSystErrorName(sys) << " could not be retrieved. stopping unfolding!" << std::endl;
			return histo1D();
		}
		histo1D syscont,sysref;
		syscont.import(huf,outisBWdiv);
		sysref.import(hrf,outisBWdiv);
		delete huf;
		delete hrf;
		out.addErrorContainer(cuf.getSystErrorName(sys),syscont,true); //ignoreMCStat=true, shouldnt do anything
		refolded.addErrorContainer(cuf.getSystErrorName(sys),sysref,true); //ignoreMCStat=true
	}
	//const histo1D setter=out;
	double lumiinv=1/cuf.getLumi();
	out=out*lumiinv;
	TString xaxisname=out.getXAxisName();
	xaxisname.ReplaceAll("_reco","");
	xaxisname.ReplaceAll(" - gen","");
	xaxisname.ReplaceAll("-gen","");
	out.setXAxisName(xaxisname);

	xaxisname.ReplaceAll("[","[pb/");
	xaxisname="d#sigma/d"+xaxisname;
	out.setYAxisName(xaxisname);
	cuf.setUnfolded(out);
	cuf.setRefolded(refolded);
	histo1D::c_makelist=tempmakelist;
	return out;
}

}//namespace
