
#ifndef ZTOP_TOOLS_CLASSES_H
#define ZTOP_TOOLS_CLASSES_H

#define ZTOP_TOOLS_CLASSES_H_FORGETBOOST

#include "../interface/histoBin.h"
#include "../interface/indexMap.h"
#include "../interface/histoContent.h"
#include "../interface/histo1D.h"

#include "../interface/histo2D.h"
#include "../interface/histoStack.h"
#include "../interface/histoStackVector.h"
#include "../interface/histoStyle.h"
#include "../interface/plotStyle.h"
//#include "../interface/canvasStyle.h"
//#include "../interface/plotter.h"


#include "../TUnfold/TUnfold.h"
#include "../TUnfold/TUnfoldBinning.h"
#include "../TUnfold/TUnfoldDensity.h"
#include "../TUnfold/TUnfoldSys.h"
#include "../interface/unfolder.h"

#include "../interface/histoUnfolder.h"
#include "../interface/graph.h"
#include "../interface/graphFitter.h"
#include "../interface/plot.h"
#include "../interface/tObjectList.h"

#include "../interface/textFormatter.h"

#include "../interface/plotterBase.h"
#include "../interface/plotterControlPlot.h"
#include "../interface/plotterMultiplePlots.h"
#include "../interface/plotterCompare.h"

#include "../interface/infoCreater.h"
#ifndef __CINT__
#include "../interface/fileReader.h"
#endif

namespace
{
struct dictionary {

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



	TUnfoldV17 sdfsdfsdf;
	TUnfoldBinningV17 sdfsdfsdfsdff;
	TUnfoldDensityV17 sdfewfeerfw;
	TUnfoldSysV17 fweewfewfds;

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
