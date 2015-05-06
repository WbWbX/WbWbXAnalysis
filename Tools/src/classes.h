
#ifndef ZTOP_TOOLS_CLASSES_H
#define ZTOP_TOOLS_CLASSES_H

#define ZTOP_TOOLS_CLASSES_H_FORGETBOOST

#include "TtZAnalysis/Tools/interface/histoBin.h"
#include "TtZAnalysis/Tools/interface/indexMap.h"
#include "TtZAnalysis/Tools/interface/histoContent.h"
#include "TtZAnalysis/Tools/interface/histo1D.h"

#include "TtZAnalysis/Tools/interface/histo2D.h"
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TtZAnalysis/Tools/interface/histoStyle.h"
#include "TtZAnalysis/Tools/interface/plotStyle.h"
//#include "../interface/canvasStyle.h"
//#include "../interface/plotter.h"


//#include "TtZAnalysis/Tools/TUnfold/TUnfold.h"
//#include "TtZAnalysis/Tools/TUnfold/TUnfoldBinning.h"
//#include "TtZAnalysis/Tools/TUnfold/TUnfoldDensity.h"
//#include "TtZAnalysis/Tools/TUnfold/TUnfoldSys.h"
#include "TtZAnalysis/Tools/interface/unfolder.h"

#include "TtZAnalysis/Tools/interface/histoUnfolder.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/graphFitter.h"
#include "TtZAnalysis/Tools/interface/plot.h"
#include "TtZAnalysis/Tools/interface/tObjectList.h"

#include "TtZAnalysis/Tools/interface/textFormatter.h"

#include "TtZAnalysis/Tools/interface/plotterBase.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/plotterCompare.h"

#include "TtZAnalysis/Tools/interface/infoCreater.h"
#ifndef __CINT__
#include "TtZAnalysis/Tools/interface/fileReader.h"
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
