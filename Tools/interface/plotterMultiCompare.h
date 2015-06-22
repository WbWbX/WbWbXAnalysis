/*
 * plotterMultiCompare.h
 *
 *  Created on: Jun 2, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTICOMPARE_H_
#define TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTICOMPARE_H_


#include "plotterCompare.h"
#include "plotterMultiColumn.h"
#include "histo1D.h"
#include "graph.h"

namespace ztop{

class plotterMultiCompare : public plotterMultiColumn{
public:


	plotterMultiCompare():plotterMultiColumn(){
		readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/plotterMultiCompare_standard.txt");
	}
	~plotterMultiCompare(){}


	void setNominalPlots(const std::vector<histo1D> *c,bool divbw=true);
	void setComparePlots(const std::vector<histo1D> *c,size_t idx,bool divbw=true);
	void setNominalPlots(const std::vector<graph> *c);
	void setComparePlots(const std::vector<graph> *c,size_t idx);


protected:


private:

	plotterBase * invokePlotter()const{return new plotterCompare();}

	void addPlotData(const size_t & idx){} //no need, temp is in individual plotters

	float getYMax(const size_t& idx,bool divbw)const;
	float getYMin(const size_t& idx,bool divbw)const;

	void readStylePriv(const std::string & file, bool requireall);

};

}



#endif /* TTZANALYSIS_TOOLS_INTERFACE_PLOTTERMULTICOMPARE_H_ */
