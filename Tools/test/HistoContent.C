/*
 * HistoContent.C
 *
 *  Created on: Aug 30, 2013
 *      Author: kiesej
 */


#include "../interface/histoContent.h"

void fillAll(ztop::histoContent & h, double val, double stat,int layer=-1){
	for(size_t bin=0;bin<h.size();bin++){
		if(bin>4){
			val=val/2;
			stat=stat/3;
		}
		h.getBin(bin,layer).addToContent(val);
		h.getBin(bin,layer).addToStat(stat);

	}
}
void HistoContent(){
	using namespace std;
	using namespace ztop;

	histoContent::setOperatorDefaults();

	histoContent hist1(2);

	histoContent hist2(2);


	fillAll(hist1,1,0.2);
	fillAll(hist2,2,0.3);

	//hist2.cout();

	histoContent added = hist1+hist2;
	hist1.addLayerFromNominal("sys4",hist2);


	hist1.cout();

	added.addLayerFromNominal("sys",hist1);
	added.addLayerFromNominal("sys2",hist2);
	std::cout << "\n---" << std::endl;
	added.cout();

	histoContent histSys=added+hist1;
	std::cout << "\n----last----" << std::endl;
	histSys.cout();



	histoContent nominal=histSys;
	nominal.removeAdditionalLayers();
	std::cout << "\n----only nominal----" << std::endl;
	nominal.cout();
	histoContent reldiff = nominal - histSys;
	std::cout << "\n----only diff----" << std::endl;
	reldiff.cout();
	reldiff /= nominal;
	reldiff.clearLayerContent(-1); //clears nominal. only syst now
	reldiff.clearLayerStat(-1);
	std::cout << "\n----relative diff (nominal 0)----" << std::endl;
	reldiff.cout();
	histoContent lhsnominal=added;
	lhsnominal.removeAdditionalLayers();
	//for each syst in reldiff, a lhs nominal is copied and multiplied by reldiff;
	lhsnominal *= reldiff;

}
