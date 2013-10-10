/*
 * plotStyle.h
 *
 *  Created on: Oct 1, 2013
 *      Author: kiesej
 */

#ifndef ZTOPPLOTSTYLE_H_
#define ZTOPPLOTSTYLE_H_
#include "containerStyle.h"
#include <vector>
/**
 * containerStyle -> plotStyle -> canvasStyle
 */
namespace ztop{

/**
 * can handle overlaying containerStyles
 */
class plotStyle{
public:
	enum templates{singlePlot,stackPlot,ratioPlot,crosssectionsGenPlot,systematicsBreakdownPlot};

	plotStyle();
	plotStyle(templates temp);
	~plotStyle();

	size_t size() const{return cstyles_.size();}

	size_t addStyle(const containerStyle & cstyle){cstyles_.push_back(cstyle);return cstyles_.size()-1;}
	containerStyle * getStyle(size_t idx=0);
	const containerStyle * getStyle(size_t idx=0) const;

	void clear(){cstyles_.clear();}


	void useTemplate(templates temp);


	void multiplySymbols(float val);

	void setAxisDefaults();
	containerAxisStyle * xAxisStyle(){return &xaxis_;}
	containerAxisStyle * yAxisStyle(){return &yaxis_;}
	const containerAxisStyle * xAxisStyle()const{return &xaxis_;}
	const containerAxisStyle * yAxisStyle()const{return &yaxis_;}

	void setLegXRange(float xlow,float xhigh);
	void setLegYRange(float ylow,float yhigh);

	float bottomMargin;
	float topMargin;
	float leftMargin;
	float rightMargin;

	bool divideByBinWidth;
	bool horizontal;

	float legxlow,legxhigh,legylow,legyhigh;
private:
	std::vector<containerStyle> cstyles_;

	containerAxisStyle xaxis_;
	containerAxisStyle yaxis_;



};

}//namespace


#endif /* PLOTSTYLE_H_ */
