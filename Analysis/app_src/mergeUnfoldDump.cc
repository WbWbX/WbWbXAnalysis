/*
 * mergeUnfoldDump.cc
 *
 *  Created on: Nov 26, 2013
 *      Author: kiesej
 */




/*
 * should do the whole thing one day (for inclusive xsec measurement)
 * -> merges systematics as given to mergeSyst
 * -> unfolds everything
 * -> dumps all plots and resulting systematics on the web
 */

#include "appFunctions.h"

namespace application{
int mergeUnfoldDump(std::vector<TString> infiles, TString outfile, TString title,TString descr,
		TString ignoresyst, bool unfoldverbose, std::vector<TString> signals){

	std::vector<TString> outfilenames=mergeSyst::mergeSyst(infiles,outfile,ignoresyst);

	for(size_t i=0;i<outfilenames.size();i++){
		TString file=outfilenames.at(i);
		file.ReplaceAll(".root","");
		TString unfoldoutputstring=unfoldApp::unfold("",file+".root","",unfoldverbose,false);
		unfoldoutputstring.ReplaceAll("\n","<br/>");
		dumpToWWW::dumpToWWW(file+".root",title+"_"+file,descr+"<br/><br/>"+unfoldoutputstring);

	}

	return 0;
}

}//application

int main(int argc, char* argv[]){
	//get all options
	std::vector<TString> infiles; TString outfile(""); TString title; TString descr;
	TString ignoresyst(""); bool unfoldverbose(false); std::vector<TString> signals;



	for(int i=1;i<argc;i++){

		if((TString)argv[i] == "-o"){
			if (i + 1 != argc){
				outfile=(TString)argv[i+1];
				i++;
			}
			continue;
		}
		else if((TString)argv[i] == "-v"){
			unfoldverbose=true;
		}
		else if((TString)argv[i] == "-s"){
			if (i + 1 != argc){
				TString signalline=(TString)argv[i+1];
				//do some while getline vodoo
				signals.push_back(signalline);
				///
				i++;
			}
			continue;
		}
		else if((TString)argv[i] == "-i"){
			if (i + 1 != argc){
				ignoresyst=(TString)argv[i+1];
				i++;
			}
			continue;
		}
		else if((TString)argv[i] == "-t"){
			if (i + 1 != argc){
				title=(TString)argv[i+1];
				i++;
			}
			continue;
		}
		else if((TString)argv[i] == "-d"){
			if (i + 1 != argc){
				descr=(TString)argv[i+1];
				i++;
			}
			continue;
		}
		//no option. must be infile

		infiles.push_back((TString)argv[i]);

	}

	///check if everything is ok here!
	if(infiles.size() < 1)
		return -1;
	if(descr.Length() <1)
		return -1;
	if(title.Length()<1)
		return -1;



	return application::mergeUnfoldDump(infiles,outfile,title,descr,ignoresyst,unfoldverbose,signals);

}
