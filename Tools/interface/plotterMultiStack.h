/*
 * plotterMultiStack.h
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_plotterMultiStack_H_
#define TTZANALYSIS_TOOLS_INTERFACE_plotterMultiStack_H_

#include "plotterBase.h"
#include "plotterControlPlot.h"
#include "plotterMultiColumn.h"
#include "histoStack.h"


namespace ztop{

class plotterMultiStack : public plotterMultiColumn{
public:


	plotterMultiStack():plotterMultiColumn(){
		readStyleFromFileInCMSSW("/src/WbWbXAnalysis/Tools/styles/plotterMultiStack_standard.txt");
	}
	~plotterMultiStack(){}

	void addStyleForAllPlots(const std::string &,const std::string &,const std::string &);


	void addStack(const histoStack *s);

	void cleanMem(){stacks_.clear(); plotterMultiColumn::cleanMem();}

protected:


private:

	plotterBase * invokePlotter()const{return new plotterControlPlot();}
	void addPlotData(const size_t & idx);//

	 float getYMax(const size_t& idx,bool divbw)const;
	 float getYMin(const size_t& idx,bool divbw)const;

	std::vector<const histoStack*> stacks_;

	 void readStylePriv(const std::string & file, bool requireall);


};

}




#endif /* TTZANALYSIS_TOOLS_INTERFACE_plotterMultiStack_H_ */
