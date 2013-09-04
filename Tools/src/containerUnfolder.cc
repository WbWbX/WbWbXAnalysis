/*
 * containerUnfolder.cc
 *
 *  Created on: Sep 4, 2013
 *      Author: kiesej
 */

#include "../interface/containerUnfolder.h"


/*
class containerUnfolder{
public:
	containerUnfolder(): unfnominal_(0),usecontoptions_(true){}
	~containerUnfolder();

	void setOptions();
	container1D unfold(const container1DUnfold &);

	const std::vector<unfolder * > & getSystUnfolder() const {return unfsyst_;}
	unfolder * getNominalUnfolder() const {return unfnominal_;}

private:
	unfolder * unfnominal_;
	std::vector<unfolder * > unfsyst_;
	bool usecontoptions_;

}//namespace
 */

namespace ztop{


bool containerUnfolder::printinfo=false;
bool containerUnfolder::debug=false;



containerUnfolder::~containerUnfolder(){
	if(unfnominal_) delete unfnominal_;
	for(size_t i=0;i<unfsyst_.size();i++)
		if(unfsyst_.at(i)) delete unfsyst_.at(i);
}


/**
 * implement later, right now, just use default ones
 */
void containerUnfolder::setOptions(){

}
container1D containerUnfolder::unfold(const container1DUnfold & cuf){

	container1D out;

	TUnfold::ERegMode regmode=regmode_;
	bool LCurve=LCurve_;
	if(0){ //binbybin_){
		std::cout << "containerUnfolder::unfold: doing binByBin unfolding - not really implemented yet" << std::endl;
	}

	/*
	 *
	 * preparation nominal
	 */
	TH1::AddDirectory(false);
	if(debug)
		std::cout << "containerUnfolder::unfold: preparing response matrix" << std::endl;
	TH2* responsematrix=cuf.prepareRespMatrix();
	unfnominal_ = new unfolder(cuf.getName()+"_nominal");
	TH1* datahist=cuf.getDataContainer().getTH1D(cuf.getName()+"_datahist_nominal",false);
	if(debug)
		std::cout << "containerUnfolder::unfold: initialising unfolder" << std::endl;
	unfnominal_->init(responsematrix,datahist,TUnfold::kHistMapOutputHoriz,regmode);
	if(debug)
		std::cout << "containerUnfolder::unfold: scanning L-Curve" << std::endl;
	if(LCurve)
		unfnominal_->scanLCurve();
	else
		unfnominal_->scanTau();
	delete responsematrix;
	delete datahist;
	if(debug)
		std::cout << "containerUnfolder::unfold: getting output" << std::endl;
	if(unfnominal_->ready()){
		out.import(unfnominal_->getUnfolded());

		//refolded_.import(unfnominal_->getReUnfolded());
	}
	else{
		std::cout << "containerUnfolder::unfold: unfolding for nominal failed!! skipping rest" << std::endl;
		return -1;
	}
	if(debug)
		std::cout << "containerUnfolder::unfold: unfolding done for nominal" << std::endl;
	//done for nominal

	/*
	 * prepare loop for systematics
	 */
	bool dodatasyst=true;
	if(cuf.getDataContainer().getSystSize() != 0 && cuf.getDataContainer().getSystSize() != cuf.getSystSize()){
		std::cout << "WARNING! containerUnfolder::unfold: ambiguous association of data systematics and MC systematics. "
				<< "data will be unfolded without systematic variations." << std::endl;
		dodatasyst=false;
	}
	if(cuf.getDataContainer().getSystSize() <1){
		std::cout << "containerUnfolder::unfold: dataContainer has no info about systematics. If intended, ok, if not, check!" << std::endl;
		dodatasyst=false;
	}
	if(debug)
		std::cout << "containerUnfolder::unfold: init systematics" << std::endl;
	for(size_t sys=0;sys<cuf.getSystSize();sys++){
		TH2* SysResponsematrix=cuf.prepareRespMatrix(false,sys);
		if(!SysResponsematrix){
			std::cout << "containerUnfolder::unfold: Response matrix for " << cuf.getSystErrorName(sys) << " could not be created. stopping unfolding!" << std::endl;
			return -2;
		}
		SysResponsematrix->Draw("COLZ");
		TH1* SysDatahist=0;
		if(dodatasyst)
			SysDatahist=cuf.getDataContainer().getTH1DSyst(cuf.getName()+"_datahist_"+cuf.getSystErrorName(sys),sys,false,true);
		else
			SysDatahist=cuf.getDataContainer().getTH1D(cuf.getName()+"_datahist_nominal_fake"+cuf.getSystErrorName(sys),false);
		unfolder * uf=new unfolder(cuf.getName()+"_"+cuf.getSystErrorName(sys));
		int verb=uf->init(SysResponsematrix,SysDatahist);
		if(verb>10000){
			std::cout << "containerUnfolder::unfold: init of unfolder for "<< cuf.getName()<< ": "
					<< cuf.getSystErrorName(sys) << " not successful. Will not unfold distribution"<< std::endl;
		}
		unfsyst_.push_back(uf);
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
		std::cout << "containerUnfolder::unfold: scanning L-Curve of systematics" << std::endl;
	for(size_t sys=0;sys<cuf.getSystSize();sys++){
		if(debug)
			std::cout << "containerUnfolder::unfold: scanning L-Curve of "<< cuf.getName() << ": " << cuf.getSystErrorName(sys) << std::endl;
		unfsyst_.at(sys)->scanLCurve();
	}
	/*
	 * getting output and merging it into unfolded_ etc. should be ok in serial mode
	 */
	for(size_t sys=0;sys<cuf.getSystSize();sys++){
		TH1 * huf=unfsyst_.at(sys)->getUnfolded();
		if(!huf){
			std::cout << "containerUnfolder::unfold: unfolding result for " << cuf.getName() << ": " << cuf.getSystErrorName(sys) << " could not be retrieved. stopping unfolding!" << std::endl;
			return -3;
		}
		container1D syscont;
		syscont.import(huf);
		delete huf;
		out.addErrorContainer(cuf.getSystErrorName(sys),syscont,true); //ignoreMCStat=true
	}
	return out;
}

}//namespace
