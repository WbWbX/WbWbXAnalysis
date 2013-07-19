#include "../interface/MainAnalyzer.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "eventLoop.h"

MainAnalyzer::MainAnalyzer(){

  AutoLibraryLoader::enable();
  freplaced_=0;
  dataname_="data";
  writeAllowed_=true;

}


int MainAnalyzer::checkForWriteRequest(){
  for(size_t i=0;i<p_askwrite.size();i++){
    if(p_askwrite.get(i)->preadready()){
      p_askwrite.get(i)->pread(); //to free pipe again
      return i; //return first ready to write!
    }
  }
  return -1;
}




int MainAnalyzer::start(){

  using namespace std;
  using namespace ztop;

  
  /// put MC code here, make containerlist private!!
 
  clear();

  readFileList();

  //TString name=channel_+"_"+energy_+"_"+syst_;

  if(channel_=="" || energy_=="" || syst_ == ""){
    std::cout << "MainAnalyzer::start Analyzer not properly named - check!" << std::endl;
    return -1;
  }

  analysisplots_.setName(getOutFileName());
  analysisplots_.setSyst(getSyst());
  int btagsyst=getBTagSF()->getSystematic();
  //load btag:
  if(!(getBTagSF()->makesEff())){
    std::cout << "loading b-tag File: " << btagsffile_ << std::endl;
    std::ifstream testfile(btagsffile_.Data());
    if(!testfile){
      std::cout << "b-tag File " << btagsffile_ << " not found, exit (-3)" << std::endl;
      return -3;
    }
    getBTagSF()->readFromTFile(btagsffile_);
    getBTagSF()->setSystematic(btagsyst);
  }

  ///communication stuff...

  size_t filenumber=infiles_.size();

  //create pipes
  p_idx.open(filenumber);
  p_finished.open(filenumber);
  p_allowwrite.open(filenumber);
  p_askwrite.open(filenumber);

  daughPIDs_.resize(filenumber,0);

  //spawn child processes
  for(size_t i=0;i<filenumber;i++){
     daughPIDs_.at(i)=fork();
    if(daughPIDs_.at(i)==0){ //child
      analyze(p_idx.get(i)->pread());
      //   std::cout << p_idx.get(i)->pread() << std::endl;
       std::exit(EXIT_SUCCESS);
    }
    else{ //send start signal for ith daughter process
         p_idx.get(i)->pwrite(i);
	 usleep(50000);
    }
  }

  //daughters never reach here

  // if(NoneEqual(daughPIDs_,0)) //recheck

  std::cout << "running on " << filenumber << " files" << std::endl;
  std::vector<int> succ;
  succ.resize(filenumber,0);

  while(!NoneEqual(succ,0)){ //wait for daughters until all are done or failed

    //get done or failed
    for(size_t i=0;i<filenumber;i++){ 
      if(p_finished.get(i)->preadready())
	succ.at(i)=p_finished.get(i)->pread(); //testing
    }
    
    int it_readytowrite=checkForWriteRequest();
    if(it_readytowrite >= 0){ // daugh ready to write
      p_allowwrite.get(it_readytowrite)->pwrite(1);
      succ.at(it_readytowrite)=p_finished.get(it_readytowrite)->pread();   //wait for successful/ns write

      int done=0,sdone=0;
      std::cout << "\n\n" << std::endl;
      for(size_t i=0;i<filenumber;i++){
	if(succ.at(i) == 0){
	  std::cout <<  "running:\t" <<infiles_.at(i) <<std::endl;
	}
	if(succ.at(i) !=0){
	  done++;
	}
	if(succ.at(i) >0){
	  sdone++;
	}
      }
      for(size_t i=0;i<filenumber;i++){
	if(succ.at(i) <0){
	  std::cout  << "failed:  \t" << infiles_.at(i) << std::endl;
	}
      }
      for(size_t i=0;i<filenumber;i++){
	if(succ.at(i) >0){
	  std::cout  << "done:   \t" << infiles_.at(i) << std::endl;
	}
      }

	  std::cout << "\n\n" << sdone << "(" << done-sdone << " failed) / " << filenumber << "done\n\n"<< std::endl;
	} //else do nothing - none ready to write
    
    // put statusbars here maybe ask for status via pipes
    

    usleep(100000); //only check every 1000ms
  }
  sleep(1);
  bool nonefailed=true;
  for(size_t i=0;i<succ.size();i++){
    std::cout << succ.at(i) << "\t" << infiles_.at(i) << std::endl;
    if(succ.at(i) < 0)
      nonefailed=false;
  }
  if(nonefailed)
    return 1; //only parent
  else
    return -1;
  
}

TString MainAnalyzer::replaceExtension(TString filename){
 
  for(size_t i=0;i<ftorepl_.size();i++){
    if(filename.Contains(ftorepl_.at(i))){
      freplaced_++;
      return filename.ReplaceAll(ftorepl_.at(i),fwithfix_.at(i));
    }
  }
  return filename;
}

void MainAnalyzer::readFileList(){
  using namespace ztop;
  using namespace std;

 

  infiles_.clear();
  legentries_.clear();
  colz_.clear();
  norms_.clear();
  legord_.clear();

  ifstream inputfiles (filelist_.Data());
  string filename;
  string legentry;
  int color;
  double norm;
  string oldline="";
  size_t legord;
  if(inputfiles.is_open()){
    while(inputfiles.good()){
      inputfiles >> filename; 

      if(((TString)filename).Contains("#")){
    	  getline(inputfiles,filename); //just ignore complete line
    	  //	std::cout << "ignoring: " << filename << std::endl;
    	  continue;
      }
      inputfiles >> legentry >> color >> norm >> legord;
      if(oldline != filename){

    	  std::cout << "adding: " << replaceExtension(filename) << "\t" << legentry << "\t" << color << "\t" << norm << "\t" << legord << std::endl;

	infiles_    << replaceExtension(filename);
	legentries_ << legentry;
	colz_       << color;
	norms_      << norm;
	legord_ << legord;
	oldline=filename;
      }
      
    }
    if((uint)freplaced_ != fwithfix_.size()){
      cout << "replacing at least some postfixes was not sucessful! expected to replace "<< fwithfix_.size() << " replaced "<< freplaced_ << " exit" << endl;

    }
  }
  else{
    cout << "MainAnalyzer::setFileList(): input file list not found" << endl;
  }

}

void MainAnalyzer::copyAll(const MainAnalyzer & analyzer){
  
  channel_=analyzer.channel_;
  syst_=analyzer.syst_;
  energy_=analyzer.energy_;
  dataname_=analyzer.dataname_;

  lumi_=analyzer.lumi_;
  datasetdirectory_=analyzer.datasetdirectory_;
  puweighter_= analyzer.puweighter_;
  filelist_=analyzer.filelist_;
  jeradjuster_= analyzer.jeradjuster_;
  jecuncertainties_= analyzer.jecuncertainties_;
  btagsf_ = analyzer.btagsf_;
  analysisplots_ = analyzer.analysisplots_;
  showstatusbar_=analyzer.showstatusbar_;

  infiles_=analyzer.infiles_;
  legentries_=analyzer.legentries_;
  colz_=analyzer.colz_;
  norms_=analyzer.norms_;
  outfileadd_=analyzer.outfileadd_;

  //pipes are NOT in here. they need to be created during running?

  writeAllowed_=analyzer.writeAllowed_;
}

MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
  copyAll(analyzer);
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
  copyAll(analyzer);
  return *this;
}

void MainAnalyzer::analyze(size_t i){

  //  std::cout << " analyze " << i<< std::endl;
  analyze(infiles_.at(i),legentries_.at(i),colz_.at(i),norms_.at(i),legord_.at(i),i);

}



