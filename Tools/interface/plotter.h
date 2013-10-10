/*
 * plotter.h
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#ifndef PLOTTER_H_
#define PLOTTER_H_

#include "containerStyle.h"
#include "canvasStyle.h"
#include "TString.h"

class TH1D;
class TH2D;
class TCanvas;
class TGraphAsymmErrors;
class TObject;
class TVirtualPad;

namespace ztop{

class container1D;
class container2D;
class container1DUnfold;
class containerStack;

/**
 * provides interface to drawing TH1D, 2Ds etc. Here ROOT starts to become important. If any other drawing tool should be used, specify here
 */
class plotter{
public:

	//enum plotTemplates{standardPlot,controlPlot,controlPlusRatioPlot,controlSigBGPlot}; maybe use canvas styles here

	plotter();
	~plotter();
	plotter(const plotter&);
	const plotter & operator = (const plotter &);

	void setStyle(const canvasStyle &); //for only one pad plots
	void useStyleTemplate(canvasStyle::canvasTemplate temp){setStyle(canvasStyle(temp));}
	canvasStyle * getStyle(){return &canvasstyle_;}
	const canvasStyle * getStyle() const {return &canvasstyle_;}

	//drawAll (fills al

	void draw(const TH1D *, const size_t & pad=0,const size_t & cidx=0,bool same=false,const plotStyle *ps=0, const containerStyle *cs=0) ;
	/*void draw(const TH2D *, const size_t & pad=0,const size_t & cidx=0,bool same,const plotStyle *ps=0, const containerStyle *cs=0) ;
	void draw(const TGraphAsymmErrors *, const size_t & pad=0,const size_t & cidx=0,bool same,const plotStyle *ps=0, const containerStyle *cs=0) ;

	void draw(const container1D *, const size_t & pad=0,const size_t & cidx=0,bool same,const plotStyle *ps=0, const containerStyle *cs=0) ;
	void draw(const container2D *, const size_t & pad=0,const size_t & cidx=0,bool same,const plotStyle *ps=0, const containerStyle *cs=0) ;
	void draw(const containerStack *, const size_t & pad=0,const size_t & cidx=0,bool same,const plotStyle *ps=0) ;

*/
	//TCanvas * draw(const container1DUnfold *, size_t pad=0);

	void applyStyle(TH1D *,const containerStyle *) const;
	void applyStyle(TH1D *,const plotStyle *) const;
	void applyStyle(TH2D *,const containerStyle *) const;
	void applyStyle(TH2D *,const plotStyle *) const;
	void applyStyle(TGraphAsymmErrors *,const containerStyle *) const;

	//Root specific
	TString errToRootDrawOpt(const containerStyle *) const;
	int errToRootFillStyle(const containerStyle *) const;

	void clearPad(size_t pad);

	//...

	void reDrawCanvas();
	void drawTextBoxes();

private:

	template<class obj>
	obj * addObject(obj *o){
		TObject *tobj=o;
		allobj_.push_back(tobj);
		return o;
	}
	template<class obj>
	obj * addPadObject(obj *o,const size_t &pad){
		if(pad>=padobj_.size())
			padobj_.resize(pad+1,std::vector<TObject *>());
		TObject *tobj=o;
		padobj_.at(pad).push_back(tobj);
		allobj_.push_back(tobj);
		return o;
	}

	bool setupStyles(const size_t & pad,const size_t & cidx, const plotStyle *&ps,const containerStyle *&cs) const;
	bool padexists(const size_t &idx) const;
	void copyfrom(const plotter &);
	bool initCanvas();
	float getRootTextSize(const textBox& tb) const;
	TVirtualPad* cdRootPad(const size_t &padidx);

	canvasStyle canvasstyle_;
	std::vector<TObject *> allobj_;
	std::vector<std::vector<TObject *> > padobj_;
	TCanvas * canvas_;
	int optstat_;

};
}//namespace


#endif /* PLOTTER_H_ */
