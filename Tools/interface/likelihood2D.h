/*
 * likelihood2D.h
 *
 *  Created on: May 26, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_LIKELIHOOD2D_H_
#define TTZANALYSIS_TOOLS_INTERFACE_LIKELIHOOD2D_H_


#include "histo2D.h"
#include "graph.h"
#include "TString.h"

namespace ztop{

class likelihood2D: private histo2D{
public:
	likelihood2D():histo2D(){}

	float getGlobalMax(size_t& xbin, size_t& ybin,int syslayer=-1)const{
		return histo2D::getMax(xbin,ybin,syslayer);
	}

	void import(const histo2D&);
	histo2D exporthist()const;

	//lossless!
	likelihood2D& operator = (const histo2D&);

	//get around this local maximum
	float getXErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma=1)const;
	//get around this local maximum
	float getXErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma=1)const;
	//get around this local maximum
	float getYErrorUp(const size_t& xbin,const size_t& ybin,const float& sigma=1)const;
	//get around this local maximum
	float getYErrorDown(const size_t& xbin,const size_t& ybin,const float& sigma=1)const;

	//get around this bin
	graph getSigmaContour(const size_t& xbin,const size_t& ybin,const float sigma=1)const;
	graph getMaxPoint(const TString& pointname="");


	void getBinCenter(const size_t binx,const size_t biny, float& centerx,float& centery)const{
		histo2D::getBinCenter(binx,biny,centerx,centery);
	}

	/*
	likelihood2D & operator += (const likelihood2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	likelihood2D operator + (const likelihood2D &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	likelihood2D & operator -= (const likelihood2D &);       //! adds errors in squares; treats same named systematics as correlated!!
	likelihood2D operator - (const likelihood2D &)const;       //! adds errors in squares; treats same named systematics as correlated!!
	likelihood2D & operator /= (const likelihood2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	likelihood2D operator / (const likelihood2D &)const;       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	likelihood2D & operator *= (const likelihood2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	likelihood2D operator * (const likelihood2D &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	likelihood2D & operator *= (float); //fast hack!            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	likelihood2D operator * (float)const;            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	likelihood2D operator * (double)const;             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	likelihood2D operator * (int)const;               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

	bool operator == (const likelihood2D &rhs)const{
		return isEqual(rhs);
	}
	bool operator != (const likelihood2D &rhs)const{
		return !(*this==rhs);
	}
	 */
	void writeToFile(const std::string& filename)const{
		histo2D::writeToFile(filename);
	}
	void readFromFile(const std::string& filename){
		histo2D::readFromFile(filename);
	}

private:
	float sigmaToLH(const float& sigma)const;

	class contourfinder{
	public:
		contourfinder(const likelihood2D * lh,int xstart, int ystart,const float& thresh):
			lh_(lh),
			xstart_(0),ystart_(0),
			xpos_(xstart),ypos_(ystart),
			lastxpos_(xstart),lastypos_(ystart),
			xsearch_(0),ysearch_(1),
			maxx_(0),maxy_(0),
			threshold_(thresh),
			tmpcont_(0),
			valid_(false),done_(false),first_(true),
			visited_(0){}

		bool findNext();
		bool isValid()const{return valid_;}
		void getCoordinates(size_t & x, size_t& y)const;

		void move(const int& dx, const int & dy);

		bool done()const{return done_;}

	private:
		contourfinder(){}
		const likelihood2D* lh_;
		size_t xstart_,ystart_;
		size_t xpos_,ypos_;
		size_t lastxpos_,lastypos_;
		int xsearch_,ysearch_;
		size_t maxx_,maxy_;
		float threshold_,tmpcont_;
		bool valid_,done_,first_;
		size_t visited_;
		bool moveNext();
		void rotate(const bool& reverse=false);
	};


};


}


#endif /* TTZANALYSIS_TOOLS_INTERFACE_LIKELIHOOD2D_H_ */
