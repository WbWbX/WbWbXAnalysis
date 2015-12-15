/*
 * likelihood2D.cc
 *
 *  Created on: May 26, 2015
 *      Author: kiesej
 */


#include "../interface/likelihood2D.h"


/*void import(const histo2D&);

	//get around this bin
	float getXErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma=0.68)const;
	//get around this bin
	float getXErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma=0.68)const;
	//get around this bin
	float getYErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma=0.68)const;
	//get around this bin
	float getYErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma=0.68)const;

	//get around this bin
	graph getsigmaContour(const size_t& xbin,const size_t& ybin)const;
 *
 */

namespace ztop{



void likelihood2D::import(const histo2D& h){
	copyFrom(h);
}
histo2D likelihood2D::exporthist()const{
	histo2D h;
	h.conts_=conts_; //! for each y axis bin one container
	h.xbins_=xbins_;
	h.ybins_=ybins_;
	h.divideBinomial_=divideBinomial_;
	h.mergeufof_=mergeufof_;

	h.xaxisname_=xaxisname_;
	h.yaxisname_=yaxisname_;
	h.zaxisname_=zaxisname_;
	return h;
}

//lossless!
likelihood2D& likelihood2D::operator = (const histo2D& h){
	copyFrom(h);
	return *this;
}


//get around this bin
float likelihood2D::getXErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma)const{
	float target=sigmaToLH(sigma) * getBinContent(xbin,ybin); //in case its not 1 at maximum
	for(size_t bin=xbin;bin<getNBinsX();bin++){
		float content=getBinContent(bin,ybin);
		if(content < target){
			float cx=0,cy=0,def=0;
			getBinCenter(bin,ybin,cx,cy);
			getBinCenter(xbin,ybin,def,cy);
			return cx-def;
		}
	}
	return 0;
}
//get around this bin
float likelihood2D::getXErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma)const{
	float target=sigmaToLH(sigma) * getBinContent(xbin,ybin); //in case its not 1 at maximum
	for(size_t bin=xbin;bin<getNBinsX();bin--){
		float content=getBinContent(bin,ybin);
		if(content < target){
			float cx=0,cy=0,def=0;
			getBinCenter(bin,ybin,cx,cy);
			getBinCenter(xbin,ybin,def,cy);
			return cx-def;
		}
	}
	return 0;
}
//get around this bin
float likelihood2D::getYErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma)const{
	float target=sigmaToLH(sigma) * getBinContent(xbin,ybin); //in case its not 1 at maximum
	for(size_t bin=ybin;bin<getNBinsY();bin++){
		float content=getBinContent(xbin,bin);
		if(content < target){
			float cx=0,cy=0,def=0;
			getBinCenter(xbin,bin,cx,cy);
			getBinCenter(xbin,ybin,cx,def);
			return cy-def;
		}
	}
	return 0;
}
//get around this bin
float likelihood2D::getYErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma)const{
	float target=sigmaToLH(sigma) * getBinContent(xbin,ybin); //in case its not 1 at maximum
	for(size_t bin=ybin;bin<getNBinsY();bin--){
		float content=getBinContent(xbin,bin);
		if(content < target){
			float cx=0,cy=0,def=0;
			getBinCenter(xbin,bin,cx,cy);
			getBinCenter(xbin,ybin,cx,def);
			return cy-def;
		}
	}
	return 0;
}

//get around this bin
graph likelihood2D::getSigmaContour(const size_t& xmax,const size_t& ymax,const float sigma)const{
	graph out;
	out.setXAxisName(getXAxisName());
	out.setYAxisName(getYAxisName());

	float target=sigmaToLH(sigma) * getBinContent(xmax,ymax);
	//ZTOP_COUTVAR(target);
	contourfinder finder(this,xmax,ymax,target);
	while(!finder.done()){
		while(!finder.findNext());
		size_t x=0,y=0;
		finder.getCoordinates(x,y);
		float cx=0,cy=0;
		getBinCenter(x,y,cx,cy);
		out.addPoint(cx,cy);
	}

	return out;

}


graph likelihood2D::getMaxPoint(const TString& pointname){
	size_t x,y;
	getGlobalMax(x,y);
	float cx=0,cy=0;
	getBinCenter(x,y,cx,cy);
	graph out(1);
	out.setPointContents(0,true,cx,cy);
	return out;
}

graph likelihood2D::getMaxLine(size_t npoints)const{

	std::vector<float > xcoords;
	std::vector<float > ycoords;
	//gets max from each row and then select npoints, without UF OF
	for(size_t y=1;y<conts_.size()-1;y++){
		float cx,cy;
		size_t xbin;
		conts_.at(y).getYMax(xbin,false);
		getBinCenter(xbin,y,cx,cy);
		xcoords.push_back(cx);
		ycoords.push_back(cy);
	}
	graph out;
	size_t skipmult=1;
	if(npoints*2<=xcoords.size())
		skipmult=(double)xcoords.size()/(double)npoints;
	size_t j=0;
	for(size_t i=0;i<xcoords.size();i++){
		if(i%skipmult) continue;
		j=i;
		out.addPoint(xcoords.at(i),ycoords.at(i));
	}
	if(j!=xcoords.size()-1)
		out.addPoint(xcoords.at(xcoords.size()-1),ycoords.at(xcoords.size()-1));


	return out;
}
/*

likelihood2D & likelihood2D::operator += (const likelihood2D &){
	//! adds stat errors in squares; treats same named systematics as correlated!!

}
likelihood2D likelihood2D::operator + (const likelihood2D &)const{
	//! adds stat errors in squares; treats same named systematics as correlated!!

}
likelihood2D & likelihood2D::operator -= (const likelihood2D &){
	//! adds errors in squares; treats same named systematics as correlated!!
}
likelihood2D likelihood2D::operator - (const likelihood2D &)const{
	//! adds errors in squares; treats same named systematics as correlated!!
}
likelihood2D & likelihood2D::operator /= (const likelihood2D &){
	//! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
}
likelihood2D likelihood2D::operator / (const likelihood2D &)const{
	//! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
}
likelihood2D & likelihood2D::operator *= (const likelihood2D &){
	//! adds stat errors in squares; treats same named systematics as correlated!!
}
likelihood2D likelihood2D::operator * (const likelihood2D &)const{
	//! adds stat errors in squares; treats same named systematics as correlated!!
}
likelihood2D & likelihood2D::operator *= (float){
	//fast hack!            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
}
likelihood2D likelihood2D::operator * (float)const{
	//! simple scalar multiplication. stat and syst errors are scaled accordingly!!
}
likelihood2D likelihood2D::operator * (double)const{
	//! simple scalar multiplication. stat and syst errors are scaled accordingly!!
}
likelihood2D likelihood2D::operator * (int)const{
	//! simple scalar multiplication. stat and syst errors are scaled accordingly!!
}
 */

float likelihood2D::sigmaToLH(const float& sigma)const{

	return exp( - sigma*sigma/2 );

}



bool likelihood2D::contourfinder::findNext(){
	//check,rotate,check,rotate,...
	//std::cout << "findnext "<< visited_ << ": "<< xpos_ <<","<< ypos_;
	valid_=false;
	if(first_){
		maxy_=lh_->getNBinsY();
		maxx_=lh_->getNBinsX();
	}
	for(size_t rot=0;rot<8;rot++){
		const float& content=lh_->getBinContent(xpos_+xsearch_,ypos_+ysearch_);
		rotate();
		const float& nextcontent=lh_->getBinContent(xpos_+xsearch_,ypos_+ysearch_);
		bool validpoint=false;
		if(espilon_)
			validpoint=fabs(content-threshold_)<espilon_;
		else
			validpoint=content < threshold_ && nextcontent>=threshold_;

		if(validpoint){
			rotate(true); //get back to previous
			valid_=true;
			//std::cout << " hit: " << xpos_+xsearch_ <<","<< ypos_+ysearch_<< std::endl;
			if(!moveNext())
				done_=true;//directly go to next
			if(first_){
				xstart_=xpos_;
				ystart_=ypos_;
				first_=false;
			}
			return true;
		}
	}
	//no valid edge found, allow checking overflow?
	size_t mvx=0,mvy=0;
	if(ypos_<maxy_-1 && lastypos_ != ypos_+1){
		mvy=1;}
	else if(xpos_<maxx_-1&& lastxpos_ != xpos_+1){
		mvx=1;}
	else if(ypos_>3 && lastypos_ != ypos_-1){
		mvy=-1;}
	else if(xpos_>3 && lastxpos_ != xpos_-1){
		mvx=-1;}
	else
		throw std::out_of_range("likelihood2D::contourfinder::findNext(): no space to move");
	if(visited_ > maxx_ * maxy_){
		done_=true;
		return true; //abort, checked all cells
	}
	lastxpos_=xpos_;
	lastypos_=ypos_;
	move(mvx,mvy);
	return false;
	//findNext();
}


void likelihood2D::contourfinder::rotate(const bool& reverse){
	if(!reverse){
		if     (xsearch_ ==  0 && ysearch_ ==-1)
			xsearch_--;
		else if(xsearch_ == -1 && ysearch_ ==-1)
			ysearch_++;
		else if(xsearch_ == -1 && ysearch_ ==0)
			ysearch_++;
		else if(xsearch_ == -1 && ysearch_ ==1)
			xsearch_++;
		else if(xsearch_ ==  0 && ysearch_ ==1)
			xsearch_++;
		else if(xsearch_ ==  1 && ysearch_ ==1)
			ysearch_--;
		else if(xsearch_ ==  1 && ysearch_ ==0)
			ysearch_--;
		else if(xsearch_ ==  1 && ysearch_ ==-1)
			xsearch_--;
	}
	else{
		if     (xsearch_ ==  0 && ysearch_ ==-1)
			xsearch_++;
		else if(xsearch_ == 1 && ysearch_ ==-1)
			ysearch_++;
		else if(xsearch_ == 1 && ysearch_ ==0)
			ysearch_++;
		else if(xsearch_ == 1 && ysearch_ ==1)
			xsearch_--;
		else if(xsearch_ ==  0 && ysearch_ ==1)
			xsearch_--;
		else if(xsearch_ ==  -1 && ysearch_ ==1)
			ysearch_--;
		else if(xsearch_ ==  -1 && ysearch_ ==0)
			ysearch_--;
		else if(xsearch_ ==  -1 && ysearch_ ==-1)
			xsearch_++;
	}
}
void likelihood2D::contourfinder::getCoordinates(size_t & x, size_t& y)const{
	x=xpos_;
	y=ypos_;
}
bool likelihood2D::contourfinder::moveNext(){
	bool ret=true;
	if(!first_ && xpos_+xsearch_==lastxpos_ && ypos_+ysearch_==lastypos_){
		ret= false;
	}
	if(xpos_+xsearch_<maxx_-1 && xpos_+xsearch_ > 2){
		xpos_+=xsearch_;
	}
	if(ypos_+ysearch_<maxy_-1 && ypos_+ysearch_ > 2){
		ypos_+=ysearch_;
	}
	if(xpos_==xstart_ && ypos_==ystart_)
		ret = false;
	lastxpos_=xpos_;
	lastypos_=ypos_;
	visited_++;
	return ret;
}
void likelihood2D::contourfinder::move(const int& dx, const int & dy){
	if(!dx && !dy)
		throw std::logic_error("likelihood2D::contourfinder::move: no movement");
	xsearch_= dx < 0 ? 1 : -1;
	if(!dx) xsearch_=0;
	ysearch_= dy < 0 ? 1 : -1;
	if(!dy) xsearch_=0;

	xpos_+=dx;
	ypos_+=dy;
	visited_++;
	rotate(); //rotate once to check next cell

}



}
