/*
 * simplePlotterBase.h
 *
 *  Created on: Nov 8, 2013
 *      Author: kiesej
 */

#ifndef SIMPLEPLOTTERBASE_H_
#define SIMPLEPLOTTERBASE_H_

#include "TObject.h"
class TH1D;
class TGraphAsymmErrors;
class TCanvas;

namespace ztop{

class simplePlotterBase{
public:
	simplePlotterBase(); //plotter takes care of memory
	~simplePlotterBase();
	simplePlotterBase(const simplePlotterBase &):c_(0),drawsame_(false){} //does nothing
	simplePlotterBase & operator = (const simplePlotterBase&);

	void cleanMem();
	void setDrawCanvas(TCanvas *c){c_=c;drawsame_=true;}
	void setDrawSame(bool);
	bool getDrawSame(){return drawsame_;}
	TCanvas * getCanvas();

protected:
	template<class obj>
	obj * addObject(obj *o){
		TObject *tobj=o;
		allobj_.push_back(tobj);
		return o;
	}
	//somehelper
	void setColors(TH1D *,int);
	void setColors(TGraphAsymmErrors *,int);
	float getLabelMultiplier();

private:
	std::vector<TObject *> allobj_;
	TCanvas * c_;
	bool drawsame_;

};
}


#endif /* SIMPLEPLOTTERBASE_H_ */
