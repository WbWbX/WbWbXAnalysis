/*
 * canvasStyle.h
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#ifndef ZTOPCANVASSTYLE_H_
#define ZTOPCANVASSTYLE_H_

#include "plotStyle.h"
#include <vector>
namespace ztop{
class plotter;

class canvasStyle{
	friend class plotter;
public:

	enum canvasTemplate{normalPlotCanvas,normalStackCanvas,stackPlusRatioCanvas,crosssectionPlusRatioCanvas};

	canvasStyle();
	canvasStyle(canvasTemplate);
	canvasStyle(const std::vector<plotStyle> &,const std::vector<float> & divs);
	~canvasStyle();

	size_t size() const{return plotstyles_.size();}
	plotStyle * getPlotStyle(size_t idx);
	const plotStyle * getPlotStyle(size_t idx) const;

	void setNDivisions(size_t div);
	void setDivisions(const std::vector<float> &divs);
	void setDivision(size_t idx, float div);
	const float & getDivision(const size_t &idx) const;
	float getPadHeight(const size_t & idx) const;

	void setStyle(size_t idx,const plotStyle&);
	void setStyle(const plotStyle&);
	void clear();
	bool check();

	std::vector<textBox> * textBoxes(){return &textboxes_;}
	void newTextBox(float x,float y,const TString &text,float textsize=1);


	void useTemplate(canvasTemplate temp);

private:
	std::vector<textBox> textboxes_;
	std::vector<plotStyle> plotstyles_;
	std::vector<float> divs_;

};




}//namespace



#endif /* CANVASSTYLE_H_ */
