#ifndef histoStackVector_h
#define histoStackVector_h

#include "histoStack.h"
//#include <vector>
#include "TStyle.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "serialize.h"
// class to handle containerstacks and to add systematics to the MC contributions etc.
// writes plots with and without ratio to TFiles
class TRandom3;

namespace ztop{


class histoStackVector:public taggedObject{

public:
	histoStackVector();
	histoStackVector(TString);
	~histoStackVector();



	void setSyst(TString sys){isSyst_=sys;}
	const TString& getSyst()const{return isSyst_;}

	bool operator == (const histoStackVector&)const;
	bool operator != (const histoStackVector&rhs)const{
		return !(*this==rhs);
	}

	void listStacks();
	std::vector<TString> getStackNames(bool);
	void setLegendOrder(TString , size_t); //do it afterwards

	// void add(ztop::histo1D &, TString, int, double);
	void add(ztop::histo1D &, TString, int, double,int legor=-1); //+legorder
	void add(ztop::histo2D &, TString, int, double,int legor=-1); //+legorder
	void add(ztop::histo1DUnfold &, TString, int, double,int legor=-1); //+legorder

	void addList( TString, int, double,int legor=-1); //+legorder
	void addList2D( TString, int, double,int legor=-1);
	void addList1DUnfold( TString, int, double,int legor=-1);

	void addSignal(const TString &);

	const std::vector<ztop::histoStack>& getVector()const{return stacks_;}
	std::vector<ztop::histoStack>& getVector(){return stacks_;}
	const ztop::histoStack & getStack(const TString&,size_t startidx=0)const;
	ztop::histoStack & getStack(const TString&,size_t startidx=0);
	const ztop::histoStack & getStack(unsigned int n)const{return stacks_.at(n);}
	ztop::histoStack & getStack(unsigned int n){return stacks_.at(n);}
	void removeContribution(TString);

	void addMCErrorStackVector(const TString&,const ztop::histoStackVector&);
	void addErrorStackVector(const TString&,const ztop::histoStackVector&);
	void addMCErrorStackVector(const ztop::histoStackVector&);
	void addGlobalRelMCError(TString,double);
	void getRelSystematicsFrom(const ztop::histoStackVector&);
	void addRelSystematicsFrom(const ztop::histoStackVector&,bool ignorestat=false,bool strict=false);
	void removeError(TString);
	void renameSyst(TString,TString); //! old new

	size_t getNSyst()const{if(stacks_.size()>0)return stacks_.at(0).getSystSize(); else return 0;}

	std::vector<size_t> removeSpikes(bool inclUFOF=false,int limittoindex=-1,float strength=100000,float sign=0.3,float threshold=8);

	void multiplyNorm(TString, double, TString); //! legendname, multiplier, stepID
	void multiplyNorms(TString, std::vector<double>, std::vector<TString>, bool showmessages=false);   //! multiplies norm of all MC with legendname  ,  with factor  ,  for step identifier string (step_string contains the id string)
	void multiplyAllMCNorms(double);

	void addPseudoData(TRandom3 * rand);

	static int treesplitlevel;

	void writeAllToTFile(TString, bool recreate=false, bool onlydata=false, TString treename="containerStackVectors");
	void printAll(TString,TString,TString);
	void writeAllToTFile(TFile * , TString treename="containerStackVectors");
	void loadFromTree(TTree * t,const TString& csvname="*");
	histoStackVector* getFromTree(TTree *t ,const TString& csvname="*");
	void listAllInTree(TTree *);

	void loadFromTFile(TFile * ,const TString& csvname="*", TString treename="containerStackVectors");

	void loadFromTFile(const TString& filename ,const TString& csvname="*", TString treename="containerStackVectors");

	size_t size(){return stacks_.size();}

	void clear(){stacks_.clear();name_="";}
	static std::vector<histoStackVector*> csv_list;
	static bool csv_makelist;
	static void csv_clearlist(){csv_list.clear();}
	static bool debug;

	static bool fastadd;


#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const;
	template <class T>
	void readFromStream(T & stream);
#endif


	void writeToFile(const std::string& filename)const;
	void readFromFile(const std::string& filename);

private:
	TString isSyst_;
	std::vector<ztop::histoStack> stacks_;
	TString tempsig_;


};


#ifndef __CINT__
template <class T>
inline void histoStackVector::writeToStream(T & stream)const{
	taggedObject::writeToStream(stream);
	IO::serializedWriteShort(isSyst_,stream);
	IO::serializedWrite(stacks_,stream);
	IO::serializedWriteShort(tempsig_,stream);

}
template <class T>
inline void histoStackVector::readFromStream(T & stream){
	taggedObject::readFromStream(stream);
	IO::serializedReadShort(isSyst_,stream);
	IO::serializedRead(stacks_,stream);
	IO::serializedReadShort(tempsig_,stream);
}

#endif


typedef  histoStackVector container1DStackVector;

}


#endif
