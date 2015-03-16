/*
 * plotStyle.h
 *
 *  Created on: Oct 1, 2013
 *      Author: kiesej
 */

#ifndef ZTOPPLOTSTYLE_H_
#define ZTOPPLOTSTYLE_H_
#include "histoStyle.h"
#include <vector>
class TH1;
class TVirtualPad;
/**
 * histoStyle -> plotStyle -> canvasStyle
 */
namespace ztop{

/**
 * handles axis and axis labels etc
 * can create TH1F to
 */
class plotStyle{
public:

	plotStyle();
	plotStyle( axisStyle xaxis, axisStyle yaxis,
	        float bottommargin,float topmargin,float leftmargin,float rightmargin,bool dividebybinwidth,bool horizontalplot=false):
	           bottomMargin(bottommargin),topMargin(topmargin),leftMargin(leftmargin),rightMargin(rightmargin),
	           divideByBinWidth(dividebybinwidth),horizontal(horizontalplot),
	           xaxis_(xaxis),yaxis_(yaxis)  {}

	~plotStyle();


	void absorbYScaling(float val);
    void absorbXScaling(float val);

	void setAxisDefaults();
	axisStyle * xAxisStyle(){return &xaxis_;}
	axisStyle * yAxisStyle(){return &yaxis_;}
	const axisStyle * xAxisStyle()const{return &xaxis_;}
	const axisStyle * yAxisStyle()const{return &yaxis_;}

	void readFromFile(const std::string &,const std::string  stylename, bool requireall=true);


	void applyAxisStyle(TH1*)const;
    void applyPadStyle(TVirtualPad*)const;

    bool forceYAxisZero()const {return yaxis_.forcezero;}

	float bottomMargin;
	float topMargin;
	float leftMargin;
	float rightMargin;

	bool divideByBinWidth;
	bool horizontal;



private:

	axisStyle xaxis_;
	axisStyle yaxis_;



};

}//namespace


#endif /* PLOTSTYLE_H_ */
