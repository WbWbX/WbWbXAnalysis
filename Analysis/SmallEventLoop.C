
//FOR LATER: come necessary includes
//#include "../DataFormats/src/classes.h" //gets all the dataformats
#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/DataFormats/interface/NTMet.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
#include <iostream>
#include "TCanvas.h"


void smallEventLoop(void){

  using namespace top;
  using namespace std;

  TString infile="path to file";



  // FOR LATER: open file .... etc .. initialise pointer to objects read from TTree
  TFile * f = new TFile(infile,"READ");

  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTPFElectrons",&pElectrons);
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets);
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent); 



  // start main loop /////////////////////////////////////////////////////////

  Long64_t nEntries=t->GetEntries(); //how many events are stored in the tree (that holds information on the events)?

  nEntries=nEntries*0.001; //should be faster for the first tests, uncomment if you like

  cout << "running on " << nEntries << " entries..." << endl;

  bool displaystatusbar=false; //do you want to display a status bar
  // (warning, this interferes with other output you do in you revent loop!)


  for(Long64_t entry=0;entry<nEntries;entry++){ //loop over all entries

   if(displaystatusbar)
	   displayStatusBar(entry,nEntries);

    t->GetEntry(entry); //FOR LATER: here all the pointers pMuons,pElectrons ... are set

    /*
     * now, each collection of objects can be selected via the pointer.
     * e.g. to get an electron from the event, do NTElectron anElectron=pElectrons->at(someindex)
     *
     * You can get its pt via anElectron.pt() ... (the "." adresses properties or functions of the object)
     *
     * be careful! if the index does not exists, the program exits. To make it safe you can do a loop like the following:
     */

    for(size_t i=0;i<pElectrons->size();i++){ // here, i never exceeds the size of the electron vector, so you are safe
    	NTElectron * electron = & pElectrons->at(i);
    	/*
    	 * FOR LATER:
    	 * this is another way of adressing the electron object. The pointer NTElectron * (note the "*") does not copy the object
    	 * (as anElectron=pElectrons->at(i) would do)
    	 * but only points to the object (which is in memory)
    	 * the "&" right in front of "pElectrons->at(i)" returns the adress of the object after it
    	 * that is exactly what we want. We get a pointer that points to the object in memory
    	 *
    	 * FOR NOW:
    	 * or (for physicists):
    	 * now you use "->" instead of "." and everything is usually faster ;)
    	 *
    	 * Now you can e.g. print out the electron pt
    	 */

    	double elecpt=electron->pt();



    	cout << elecpt << endl;

    	/*
    	 * You can find other proporties you can access in the corresponding header files NTElectron.h, NTMuon.h, NTJet.h ..
    	 */

    }
    cout << endl; // don't delete, thats only for the status bar


  }


  f->Close(); //FOR LATER: close the file
  delete f; //FOR LATER: delete the object you created with "new" from memory


}

