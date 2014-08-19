#include "../interface/systMerger.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"

int main(int argc, char* argv[]){
	using namespace ztop;
	using namespace std;

	containerStackVector::fastadd=false; //be careful because of parallization

	optParser parser(argc,argv);
	TString outputadd=parser.getOpt<TString>("o","","additional string to be added to output file names");
	TString inputadd =parser.getOpt<TString>("i","","additional string to be added to input file names");

	std::vector<std::string> allfiles=parser.getRest<std::string>();

	if(allfiles.size() < 1){
		std::cout << "needs minimum 1 input file" << std::endl;
		parser.coutHelp();
	}
	std::vector<std::string> sysnominalids;
	//search for specific sysnominals
	for(size_t i=0;i<allfiles.size();i++){
		std::string sysnomid="_sysnominal";
		size_t pos=allfiles.at(i).find(sysnomid);
		if(pos != std::string::npos){
			//pos+=sysnomid.length();
			std::string newnomid=allfiles.at(i).substr(0,pos);//get everything up to "_sysnominal"
			textFormatter tf;
			tf.setDelimiter("_");
			tf.setComment("");
			tf.setTrim("");
			std::vector<std::string> formattedout=tf.getFormatted(newnomid);
			std::string prefix = formattedout.at(formattedout.size()-1);
			newnomid = prefix+sysnomid;
			//std::cout << "found " << newnomid << std::endl;
			if(std::find(sysnominalids.begin(),sysnominalids.end(),newnomid) == sysnominalids.end())
				sysnominalids.push_back(newnomid);
		}

	}
	//systMerger::debug=true;
	std::vector<TString> partialout;
	std::vector<TString > nominalstartstrings;
	for(size_t i=0;i<sysnominalids.size();i++){
		std::cout << "merging relative variations to " << sysnominalids.at(i) << std::endl;
		systMerger partialmerge;
		partialmerge.setDrawCanvases(false);
		partialmerge.setNominalID(sysnominalids.at(i));
		partialmerge.setIgnoreName("_nominal"); //dont ignore
		partialmerge.setInFileAdd(inputadd);
		partialmerge.setOutFileAdd(outputadd);
		partialmerge.setInputStrings(allfiles);
		std::vector<TString> pout=partialmerge.start();
		partialout.insert(partialout.end(),pout.begin(),pout.end());

	}




	//main merger
	systMerger * merger=new systMerger();
	//systMerger::debug=false;
	merger->setDrawCanvases(false);
	merger->setInFileAdd(inputadd);
	merger->setOutFileAdd(outputadd);
	merger->setInputStrings(allfiles);
	merger->setIgnoreName("_sysnominal");
	std::vector<TString> mergedfiles=merger->start();
	delete merger;

	if(partialout.size()>0){
		//add relative variations
		textFormatter tf;
		tf.setComment("_nominal"); //this will cut everything but start strings
		tf.setDelimiter("");
		for(size_t i=0;i<mergedfiles.size();i++){
			if(!mergedfiles.at(i).Contains("_nominal")) continue; //should not be the case
			containerStackVector * nom= new containerStackVector();
			nom->loadFromTFile(mergedfiles.at(i));
			//nom->writeAllToTFile(mergedfiles.at(i)+"_noreladds.root",true,true);
			TString startstring=tf.getFormatted(mergedfiles.at(i).Data()).at(0);
			//by default this should be only one for each merged file
			//now look if there are relative variations to be added
			for(size_t pit=0;pit<partialout.size();pit++){
				if(partialout.at(pit).BeginsWith(startstring)){
					std::cout << "adding relative variations from  " << partialout.at(pit)
									<< "\nto " << nom->getName()<< std::endl;
					//do by hand
					containerStackVector * csvpart= new containerStackVector();
					csvpart->loadFromTFile(partialout.at(pit));
					container2D::debug=true;
					std::vector<size_t> syssmoothened;
					//syssmoothened=csvpart->removeSystematicsSpikes(false,-1,100,0.333,15);

					size_t oldsyssize=nom->getNSyst();
					nom->addRelSystematicsFrom(*csvpart);
					size_t newsystsize=nom->getNSyst();
					for(size_t addsys=oldsyssize;addsys<newsystsize;addsys++)
						nom->removeSystematicsSpikes(false,addsys,100,0.333,15);
					container2D::debug=false;

					delete csvpart;
				}
			}
			nom->writeAllToTFile(mergedfiles.at(i),true,true);
			delete nom;
		}
	}





}
