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

	bool applyAxisRange() const; //if min>max false (default)

	float titleSize;
	float labelSize;
	float titleOffset;
	float labelOffset;
	float tickLength;
	int ndiv;

	float max,min; //if min > max, don't apply

};
class containerStyle;
class textBox{
	friend class containerStyle;
public:
	textBox();
	textBox(float,float,const TString &,float textsize=1);
	~textBox();
	void setText(const TString &);
	void setTextSize(float);
	void setCoords(float,float);
	const TString & getText() const;
	const float & getTextSize() const;
	const float & getX() const;
	const float &  getY() const;

private:
	float x_,y_;
	TString text_;
	float textsize_;
};

class containerStyle{
public:
	enum templates{normalPlot,controlPlotData, controlPlotMC, ratioPlotData, ratioPlotMC, crosssectionPlot, systematicsPlot}; // make as many as you like...
	enum errorBarStyles{noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr}; //if syst errors are present, it refers to outer errors (for rect, perp etc)


	containerStyle();
	containerStyle(templates );
	~containerStyle();

	void useTemplate(templates temp);


	void multiplySymbols(float val);

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


};



}//namespace

#endif /* CONTAINERSTYLE_H_ */
