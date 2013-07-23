/*
 * eventLoop.h
 *
 *  Created on: Jul 17, 2013
 *      Author: kiesej
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "../interface/MainAnalyzer.h"


/**
 * for a given input filename (a root file that exists in datasetdirectory_), the event loop is started
 * legord describes the legend order value
 * don't touch anaid
 */
void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm,size_t legord,size_t anaid){


  using namespace std;
  using namespace ztop;


  bool isMC=true;
  if(legendname==dataname_) isMC=false;


/*
 * check if file exists and open file
 */


  TFile *f;
  if(!fileExists((datasetdirectory_+inputfile).Data())){
	  std::cout << datasetdirectory_+inputfile << " not found!!" << std::endl;
	  p_finished.get(anaid)->pwrite(-1);
	  return;
  }
  else{
	  f=TFile::Open(datasetdirectory_+inputfile);
  }

/*
 * book histograms here
 * e.g. TH1D elecpt_step1 = new TH1D("...);
 */



  /*
   * this part modifies the normalisation from the theory cross section to account for the
   * number of generated events and the luminosity
   */
  double oldnorm=norm;

  double genentries=0;
  if(isMC){
	  TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
	  genentries=tnorm->GetEntries();
	  delete tnorm;
	  norm = lumi_ * norm / genentries;
	  else{//not mc
		  norm=1;
	  }

/*
 * some couts to check whether something went wrong
 */

  cout << "running on: " << datasetdirectory_ << inputfile << "    legend: " << legendname << "\nxsec: " << oldnorm << ", genEvents: " << genentries <<endl;

  /*
   * get main analysis input tree and prepare branches
   */

  TTree * t = (TTree*) f->Get("PFTree/PFTree");

  TBranch * b_TriggerBools=0;
  std::vector<bool> * p_TriggerBools=0;
  t->SetBranchAddress("TriggerBools",&p_TriggerBools, &b_TriggerBools);

  TBranch * b_Electrons=0;
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTPFElectrons",&pElectrons,&b_Electrons);

  TBranch * b_Muons=0;
  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons, &b_Muons);

  TBranch * b_Jets=0;
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets, &b_Jets);

  TBranch * b_Met=0;
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet,&b_Met);

  TBranch * b_Event=0;
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent,&b_Event);



  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////// start main loop /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////

  Long64_t nEntries=t->GetEntries();
  if(norm==0) nEntries=0; //skip for norm0

  for(Long64_t entry=0;entry<nEntries;entry++){
    if(showstatusbar_) displayStatusBar(entry,nEntries);

/*
 *  Here, the weights are prepared for the vertex multiplicity reweighting
 *  In order to do that, information from the NTEvent branch is needed,
 *  so b_Event->GetEntry(entry) reads the necessary piece of information from the input tree
 *  now, the pEvent pointer is valid and points to the information
 */
    b_Event->GetEntry(entry);
    double puweight=1;
    if (isMC) puweight = getPUReweighter()->getPUweight(pEvent->truePU());

/*
 * as a next step, you should as for the right triggers to be fired
 * again, we "open" the trigger bool branch and thereby set the address of
 * p_TriggerBools
 */

    b_TriggerBools->GetEntry(entry);

    /*
     * now for muons... etc
     */

    b_Muons->GetEntry(entry);

  }//main event loop ends

  if(showstatusbar_)std::cout << std::endl; //for status bar

  /*
   * this statement might need to be moved because of TH1 ownership issues
   */

  f->Close();
  delete f;


  ///////////////////////////////
  ///////////////////////////////
  ///////////////////////////////
  ///////////////////////////////
  //     WRITE OUTPUT PART     //
  ///////////////////////////////
  ///////////////////////////////
  ///////////////////////////////
  ///////////////////////////////

/*
 * don't change this part, asking main program to perform write operation
 */
  p_askwrite.get(anaid)->pwrite(anaid);
  std::cout << anaid << " (" << inputfile << ")" << " asking for write permissions to " <<getOutPath() << endl;
  int canwrite=p_allowwrite.get(anaid)->pread();
  if(canwrite>0){ //wait for permission

	  /*
	   * write permission is granted, file is writeable
	   */

    TFile * outfile;
    /*
     * if outfile does not exist, yet, create it
     */

    if(!fileExists((getOutPath()+".root").Data()))
      outfile=new TFile(getOutPath()+".root","RECREATE");
    else
      outfile=new TFile(getOutPath()+".root","READ");

    outfile->Close();
    delete outfile;

    // some snippets that might come handy or not
/*
    outtree->Fill();
    outtree->Write("",TObject::kOverwrite);//"",TObject::kOverwrite);
    outfile->Close();
    delete outfile;
*/
    ///btagsf

    /*
     * report that writing output was successful
     */

    p_finished.get(anaid)->pwrite(1); //turns of write blocking, too
  }
  else{
    p_finished.get(anaid)->pwrite(-2); //write failed (turns of write blocking)
  }




}



#endif /* EVENTLOOP_H_ */
