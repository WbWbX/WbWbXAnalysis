/*
 * containerStyle.h
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#ifndef CANVASSTYLE_H_
#define CANVASSTYLE_H_
/**
 * for slow migration to histoplotter class using these styles
 * in general set style form template like "ratiodata, ratioMC, controlplotdata, controlplotmc..." and then adapt
 */
#include <vector>
#include "TString.h"


namespace ztop{
class containerStyle;
//helper classes
class containerAxisStyle{
	friend class containerStyle;

public:
	containerAxisStyle();
	~containerAxisStyle();

	bool applyAxisRange(); //if min>max false (default)

	float titleSize;
	float labelSize;
	float titleOffset;
	float labelOffset;
	float tickLength;
	int ndiv;

	float max,min; //if min > max, don't apply

};



class containerStyle{
public:
	enum templates{normalPlot,controlPlotData, controlPlotMC, ratioPlotData, ratioPlotMC, crosssectionPlot, systematicsPlot}; // make as many as you like...
	enum errorBarStyles{noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr}; //if syst errors are present, it refers to outer errors (for rect, perp etc)

	containerStyle();
	~containerStyle();

	void useTemplate(templates temp);


	containerAxisStyle * xAxisStyle(){return &xaxis_;}
	containerAxisStyle * yAxisStyle(){return &yaxis_;}



	float markerSize;
	int markerStyle;
	int markerColor;

	float lineSize;
	int lineStyle;
	int lineColor;

	int fillStyle;
	int fillColor;

	errorBarStyles errorStyle;

	float topMargin;
	float bottomMargin;
	float leftMargin;
	float rightMargin;

private:
	containerAxisStyle xaxis_;
	containerAxisStyle yaxis_;
};



/**
 * divide stuff, has vector of containerStyle in, some templates, all accessible
 */
/*
class canvasStyle{
public:
	canvasStyle();
	~canvasStyle();

	void newPad(const containerStyle &);


private:
	std::vector<containerStyle> padstyles_;
	std::vector<TString> padnames_;

};
*/
}//namespace

#endif /* CONTAINERSTYLE_H_ */
/*
 * h->GetYaxis()->SetTitleSize(0.06*labelmultiplier_);
	h->GetYaxis()->SetLabelSize(0.05*labelmultiplier_);
	h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset() / labelmultiplier_);
	h->GetYaxis()->SetTitle(yname_);
	h->GetYaxis()->SetNdivisions(510);
	h->GetXaxis()->SetTitleSize(0.06*labelmultiplier_);
	h->GetXaxis()->SetLabelSize(0.05*labelmultiplier_);
	h->GetXaxis()->SetTitle(xname_);
	h->LabelsDeflate("X");
	h->SetMarkerStyle(20);
	if(wasunderflow_) h->SetTitle((TString)h->GetTitle() + "_uf");
	if(wasoverflow_)  h->SetTitle((TString)h->GetTitle() + "_of");
 */
