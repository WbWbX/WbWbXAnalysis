/*
 * plotterMultiStack.h
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTISTACK_H_
#define TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTISTACK_H_

#include "plotterBase.h"
#include "plotterControlPlot.h"


namespace ztop{
class histoStack;

class plotterMultiStack : public plotterBase{
public:


	plotterMultiStack();
	~plotterMultiStack(){plotterMultiStack::cleanMem();}

	void readStyleFromFile(const std::string& );
	/*
	 * expects entries:
	 * [textBoxes - boxes]
	 *
	 * [plotterMultiStack]
	 *   controlPlotStyleFile=<file abs path within CMSSW_BASE>
	 *   ...
	 *   frac0= ...
	 * [end - plotterMultiStack]
	 *
	 * [legendStyle] (for legend printed in last plot)
	 *
	 */


	void addStack(const histoStack *s);
	void associateCorrelationMatrix(const corrMatrix& m){corrm_=&m;}

	void cleanMem();
	void printToPdf(const std::string& outname);


protected:
	void preparePad();
	void drawPlots();
	// void drawTextBoxes();
	void drawLegends();
	void refreshPad();

private:
	void addStyleFromFile(const std::string& );
	void clear(){}
	void clearPlots(){}

	std::vector<const histoStack*> stacks_;
	std::vector<plotterControlPlot*> plotter_;

	const corrMatrix* corrm_;

	std::string stylefile_;

	TLegend* globalleg_;

	std::vector<float> separators_;
	std::vector<size_t> yaxisconnects_;
	/*
	float bottommarg_,topmarg_,leftmarg_,rightmarg_;*/

	void readStylePriv(const std::string& infile,bool requireall);

};

}




#endif /* TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTISTACK_H_ */
