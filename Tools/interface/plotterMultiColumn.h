/*
 * plotterMultiColumn.h
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_plotterMultiColumn_H_
#define TTZANALYSIS_TOOLS_INTERFACE_plotterMultiColumn_H_

#include "plotterBase.h"
#include "plotterControlPlot.h"


namespace ztop{
class histoStack;

class plotterMultiColumn : public plotterBase{
public:


	plotterMultiColumn();
	virtual ~plotterMultiColumn(){plotterMultiColumn::cleanMem();}

	/*
	 * expects entries:
	 * [textBoxes - boxes]
	 *
	 * [plotterMultiColumn]
	 *   controlPlotStyleFile=<file abs path within CMSSW_BASE>
	 *   ...
	 *   frac0= ...
	 * [end - plotterMultiColumn]
	 *
	 * [legendStyle] (for legend printed in last plot)
	 *
	 */


	void addStyleForAllPlots(const std::string &);


	void associateCorrelationMatrix(const corrMatrix& m){corrm_=&m;}

	void cleanMem();
	void printToPdf(const std::string& outname);
	void printToPng(const std::string& outname);

    bool hasRatio()const{return true;}
    float getYSpaceMulti(bool logar,bool divbw)const{return yspacemulti_;}

protected:


	void preparePad();
	void drawPlots();
	// void drawTextBoxes();
	void drawLegends();
	void refreshPad();
	void adjustCanvas(TCanvas *p);

	std::vector<plotterBase*> plotter_;

	void readStylePrivMarker(const std::string& infile,bool requireall,const std::string& marker);
private:

	virtual plotterBase * invokePlotter()const=0;

	virtual void addPlotData(const size_t & idx)=0;
	virtual float getYMax(const size_t& idx,bool divbw)const=0;
	virtual float getYMin(const size_t& idx,bool divbw)const=0;

	//hide .... throw
    plotStyle& getUpperStyle(){throw std::logic_error("NA");}
    const plotStyle& getUpperStyle()const{throw std::logic_error("NA");}
    plotStyle& getRatioStyle(){throw std::logic_error("NA");}
    const plotStyle& getRatioStyle()const{throw std::logic_error("NA");}

    float getXAxisLowerLimit()const{return 0;}
    float getXAxisHigherLimit()const{return 0;}

	void clear(){}
	void clearPlots(){}

	bool plotIsApproxSymmetric(float thresh)const{return false;}


	const corrMatrix* corrm_;

	std::string stylefile_;

	TLegend* globalleg_;

	std::vector<float> separators_;
	std::vector<size_t> yaxisconnects_;
	/*
	float bottommarg_,topmarg_,leftmarg_,rightmarg_;*/

	float resolutionmultiplier_;

};

}




#endif /* TTZANALYSIS_TOOLS_INTERFACE_plotterMultiColumn_H_ */
