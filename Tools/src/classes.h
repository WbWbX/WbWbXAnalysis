
#ifndef ZTOP_TOOLS_CLASSES_H
#define ZTOP_TOOLS_CLASSES_H

#define ZTOP_TOOLS_CLASSES_H_FORGETBOOST

#include "WbWbXAnalysis/Tools/interface/histoBin.h"
#include "WbWbXAnalysis/Tools/interface/indexMap.h"
#include "WbWbXAnalysis/Tools/interface/histoContent.h"
#include "WbWbXAnalysis/Tools/interface/histo1D.h"

#include "WbWbXAnalysis/Tools/interface/histo2D.h"
#include "WbWbXAnalysis/Tools/interface/histoStack.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/Tools/interface/histoStyle.h"
#include "WbWbXAnalysis/Tools/interface/plotStyle.h"
//#include "../interface/canvasStyle.h"
//#include "../interface/plotter.h"


//#include "WbWbXAnalysis/Tools/TUnfold/TUnfold.h"
//#include "WbWbXAnalysis/Tools/TUnfold/TUnfoldBinning.h"
//#include "WbWbXAnalysis/Tools/TUnfold/TUnfoldDensity.h"
//#include "WbWbXAnalysis/Tools/TUnfold/TUnfoldSys.h"
#include "WbWbXAnalysis/Tools/interface/unfolder.h"

#include "WbWbXAnalysis/Tools/interface/histoUnfolder.h"
#include "WbWbXAnalysis/Tools/interface/graph.h"
#include "WbWbXAnalysis/Tools/interface/graphFitter.h"
#include "WbWbXAnalysis/Tools/interface/plot.h"
#include "WbWbXAnalysis/Tools/interface/tObjectList.h"

#include "WbWbXAnalysis/Tools/interface/textFormatter.h"

#include "WbWbXAnalysis/Tools/interface/plotterBase.h"
#include "WbWbXAnalysis/Tools/interface/plotterControlPlot.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "WbWbXAnalysis/Tools/interface/plotterCompare.h"

#include "WbWbXAnalysis/Tools/interface/infoCreater.h"
#ifndef __CINT__
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#endif

namespace
{
struct dictionary_tools {

	std::vector<double> klklkl;
	std::vector<TString> kkj;
	std::pair<TString,std::vector<double> > jj;
	std::vector<std::pair<TString,std::vector<double> > > j;
	std::vector<long int> kl;
	std::vector<int> klsdsda;
	ztop::histoBin isaojewd;
	std::vector<ztop::histoBin> asdqwewq;
	ztop::histoBins asdkeowd;
	std::vector<ztop::histoBins> asdweddddw;
	std::map<TString,size_t > kokssd;
	std::map<size_t,TString > koksasdsd;
	ztop::indexMap<TString> sdijiewjf;
	std::vector<ztop::indexMap<TString> > safdewdewdwed;
	ztop::histoContent asdijewj;
	std::vector<ztop::histoContent> sadljiewjfiejwf;


	ztop::histo1D a;
	std::vector<ztop::histo1D> b;
	ztop::histo2D aa;
	std::vector<ztop::histo2D> bb;
	ztop::histoStack asda;
	std::vector<ztop::histoStack> bbd;
	ztop::histoStackVector aaa;
	std::vector<ztop::histoStackVector> bbb;


	std::vector<TH2D> ijij;
	std::map<TString,std::vector<TH2D> > kokd;

	std::vector<ztop::unfolder*> asldkjwed;
	ztop::histo1DUnfold  sdfdfsdfsdfsd;
	std::vector<ztop::histo1DUnfold> adasijweodf;



	//TUnfoldV17 *sdfsdfsdf;
	//TUnfoldBinningV17 *sdfsdfsdfsdff;
	//TUnfoldDensityV17 *sdfewfeerfw;
	//TUnfoldSysV17 *fweewfewfds;

	ztop::unfolder asdsdad;
	std::vector<ztop::unfolder> sdadasdasdsl;
	ztop::histoUnfolder ijsdfoijerg;

	ztop::textBox dsfsd;
	ztop::histoStyle sdfewfij;
	ztop::plotStyle dsfdsfl;
	//ztop::canvasStyle dsfdf;
//	ztop::plotter ijdfsij;

	std::vector<ztop::textBox> vdsfsd;
	std::vector<ztop::histoStyle> vsdfewfij;
	std::vector<ztop::plotStyle> vdsfdsfl;


	ztop::graph weewrefd;
	std::vector<ztop::graph> vweewrefd;

	ztop::simpleFitter weewrefdsad;
	ztop::graphFitter weewrefdsaasdsad;

	ztop::tObjectList jhwefiuhwefjn;
	std::vector<ztop::tObjectList> sdwefwerewfds;

	ztop::taggedObject tagewfdsfds;
	std::vector<ztop::taggedObject::tags> tagsqwdwed;
	std::vector<ztop::taggedObject> taggedobjectsoijdof;

	ztop::plot pltggg;


	ztop::textFormatter dfewr23rewf;

	ztop::plotterControlPlot sdfewrewrwf;
	ztop::plotterMultiplePlots sdfdfewewrw;
	ztop::plotterCompare ewwefevrefwefewf;


    std::vector<ztop::histo1D *> sdfsdfwef;
    std::vector<std::vector<ztop::histo2D *> > sddf_sdfsf;

	ztop::infoCreater weoirgjerowijgre;
#ifndef __CINT__
	ztop::fileReader readleityoulikeootmode;
#endif


};
}

#undef ZTOP_TOOLS_CLASSES_H_FORGETBOOST

#endif
