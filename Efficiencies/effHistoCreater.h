#include "../../DataFormats/interface/NTElectron.h"
#include "../../DataFormats/interface/NTMuon.h"
#include "../../DataFormats/interface/NTSuClu.h"
#include "../../DataFormats/interface/NTEvent.h"
#include "../../DataFormats/interface/NTTrack.h"
#include "../../plugins/leptonSelector.h"
#include <vector>
#include <TString.h>

#include "../../plugins/reweightPU.h"
#include "../../plugins/histo1D.h"

#include <iostream>

#include "../../plugins/miscUtils.h"



namespace top{


  class effHistoCreater{

  public:
    effHistoCreater(){};
    effHistoCreater(TString outputfilename){outname_=outputfilename;binwidth_=0.5;massmin_=60;massmax_=120;userhoiso_=true;};
    ~effHistoCreater(){};
    
    void setIsMuons(bool IsMuons){ismuons_=IsMuons;}
    void setPtBins(vector<float> Ptbins){ptbins_=Ptbins;} 
      //fill ascending! important!
    void setEtaBins(vector<float> Etabins){etabins_=Etabins;}
      //fill ascending! important!
    void setMassRange(float min, float max){massmin_=min;massmax_=max;}
    void setBinWidth(float Width){binwidth_=Width;}
    void setMCFile(TString MCFile){mcfile_=MCFile;}
    void setDataFile(TString DataFile){datafile_=DataFile;}
    void setPuFile(TString PUFile){pufile_=PUFile;}
    void doIDIsoBoth(int what){idisoboth_=what;}
    void setOutFile(TString Outfile){outname_=Outfile;}
    void setUseSuClus(bool use){usesuclus_=use;}
    void setUseTracks(bool use){usetracks_=use;}
    void setUseRhoIso(bool use){userhoiso_=use;}

    void makeFitHistos(){
      using namespace std;
      using namespace top;
      
      ///specify inv mass binning
      float massrangemin=massmin_;
      float massrangemax=massmax_;
      float binwidth=binwidth_;
      
      TString outputfile=outname_;
      bool isMuons=ismuons_;
      vector<float> etabins=etabins_;
      vector<float> ptbins=ptbins_;
      
      
      leptonSelector lepSel;
      lepSel.setMassRange(massrangemin,massrangemax);
      lepSel.setUseRhoIsoForElectrons(userhoiso_);
      
      //specify trees etc
      TFile * MCFile = new TFile(mcfile_);
      TTree *tMC = (TTree*)(MCFile->Get("PFTree/pfTree"));
      
      long int nEntriesMC=tMC->GetEntries();
      
      vector<float> massbins;
      for(float i=0;i<=(massrangemax-massrangemin)/binwidth;i++) massbins.push_back(i*binwidth+massrangemin);
      
      const unsigned int Nptbins=ptbins.size()-1;
      const unsigned int Netabins=etabins.size()-1;
      
      ///prepare the binned inv mass distributions
      vector<vector<histo1D> > invmassData;
      vector<vector<histo1D> > invmassMC;
      vector<vector<histo1D> > invmassDataPassed;
      vector<vector<histo1D> > invmassMCPassed;
      
      for(unsigned int i=1;i<Netabins+1;i++){
	vector<histo1D> tempvec;
	for(unsigned int j=1;j<Nptbins+1;j++){
	  histo1D temp=histo1D(massbins);
	  tempvec.push_back(temp);
	}
	invmassData.push_back(tempvec);
	invmassMC.push_back(tempvec);
	invmassDataPassed.push_back(tempvec);
	invmassMCPassed.push_back(tempvec);
      }
      


      vector<NTMuon> * pMuons = 0;
      tMC->SetBranchAddress("NTMuons",&pMuons); 
      vector<NTElectron> * pElectrons = 0;
      tMC->SetBranchAddress("NTElectrons",&pElectrons);
      vector<NTTrack> * pTracks = 0;
      tMC->SetBranchAddress("NTTracks",&pTracks);
      vector<NTSuClu> * pSuClus = 0;
      tMC->SetBranchAddress("NTSuClu",&pSuClus);

      NTEvent * pEvent = 0;
      tMC->SetBranchAddress("NTEvent",&pEvent);

      PUReweighter PUweight;
      PUweight.setDataTruePUInput(pufile_);
      PUweight.setMCDistrSum12();


      //configure rho iso adder:
      elecRhoIsoAdder RhoIsoAdder(false,false);

      //nEntriesMC=20000;
      
      cout << "creating " << outname_ << endl;

      for(long int entry=0;entry<nEntriesMC;entry++){  //main loop over MCtree
	tMC->GetEntry(entry);
	if((100*entry)%nEntriesMC){
	  cout << "\rMC: " << 100*entry/nEntriesMC +1<< "%";
	  fflush(stdin);
	}
	double puweight=1;
	puweight=PUweight.getPUweight(pEvent->truePU());

	effTriple effvars;

	if(isMuons){
	  vector<NTMuon> treeMuons=*pMuons;
	  vector<NTMuon> idmuons=lepSel.selectIDMuons(treeMuons);
	  vector<NTMuon> isomuons=lepSel.selectIsolatedMuons(idmuons);
	  vector<NTMuon> tagmuons=lepSel.selectTagMuons(isomuons);
	  vector<NTTrack> tracks=*pTracks;
	  if(idisoboth_==0){
	    if(usetracks_){
	      effvars = lepSel.getMatchingProbeMuon(tracks, tagmuons, idmuons);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeMuon(treeMuons, tagmuons, idmuons);
	    }
	  }
	  else if(idisoboth_==1){
	    effvars = lepSel.getMatchingProbeMuon(idmuons, tagmuons, isomuons);
	  }
	  else if(idisoboth_==2){
	    if(usetracks_){
	      effvars = lepSel.getMatchingProbeMuon(tracks, tagmuons, isomuons);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeMuon(treeMuons, tagmuons, isomuons);
	    }
	  }

	}
	else{
	  //  double rho=pEvent->getIsoRho(2); //PUJets as in twiki

	  vector<NTElectron> rhoElectrons = *pElectrons;
	  //	  RhoIsoAdder.setRho(rho);
	  //	  RhoIsoAdder.addRhoIso(rhoElectrons);

	  vector<NTElectron> idelecs=lepSel.selectIDElectrons(rhoElectrons);
	  vector<NTElectron> isoelecs=lepSel.selectIsolatedElectrons(idelecs);
	  vector<NTElectron> tagelecs=lepSel.selectTagElectrons(isoelecs);
	  vector<NTSuClu>    suclus=*pSuClus;
	  if(idisoboth_==0){
	    if(usesuclus_){
	      effvars = lepSel.getMatchingProbeElec(suclus, tagelecs , idelecs);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeElec(rhoElectrons, tagelecs , idelecs);
	    }
	  }
	  else if(idisoboth_==1){
	    effvars = lepSel.getMatchingProbeElec(idelecs,tagelecs , isoelecs);
	  }
	  else if(idisoboth_==2){
	    if(usesuclus_){
	      effvars = lepSel.getMatchingProbeElec(suclus,tagelecs , isoelecs);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeElec(rhoElectrons,tagelecs , isoelecs);
	    }
	  }

	}

	if(effvars.mass() ==0) continue;
	
	double eta=effvars.p4().Eta();
	double pt=effvars.p4().Pt();
	bool probepasses=effvars.passed();

	//fill probes
	//check which bin
	for(unsigned int i=0;i<Netabins;i++){
	  if(eta<etabins[i+1]){
	    bool filled=false;
	    for(unsigned int j=0;j<Nptbins;j++){
	      if(pt<ptbins[j+1]){
		invmassMC[i][j].fill(effvars.mass(),puweight);
		break;
		filled=true;
	      }
	    }
	    if(filled) break;
	  }
	}
	//select passing probes
	
	if(probepasses){
	  //check which bin
	  for(unsigned int i=0;i<Netabins;i++){
	    if(eta<etabins[i+1]){
	      bool filled=false;
	      for(unsigned int j=0;j<Nptbins;j++){
		if(pt<ptbins[j+1]){
		  invmassMCPassed[i][j].fill(effvars.mass(),puweight);
		  break;
		  filled=true;
		}
	      }
	      if(filled) break;
	    }
	  }
	} // probepasses
      } //MC tree loop

      
      MCFile->Close();
      TFile * dataFile = new TFile(datafile_);
      TTree *tdata = (TTree*)(dataFile->Get("PFTree/pfTree"));

      /// Fill data
      tdata->SetBranchAddress("NTMuons",&pMuons); 
      tdata->SetBranchAddress("NTElectrons",&pElectrons); 
      
      tdata->SetBranchAddress("NTEvent",&pEvent); 
      tdata->SetBranchAddress("NTTracks",&pTracks);
      tdata->SetBranchAddress("NTSuClu",&pSuClus);


      long int nEntriesData=tdata->GetEntries();
      //nEntriesData=10000;

      cout << endl;
      for(long int entry=0;entry<nEntriesData;entry++){  //main loop over datatree
	tdata->GetEntry(entry);
	if((100*entry)%nEntriesData){
	  cout << "\rdata: " << 100*entry/nEntriesData +1<< "%";
	  fflush(stdin);
	}

	effTriple effvars;

	if(isMuons){
	  vector<NTMuon> treeMuons=*pMuons;
	  vector<NTMuon> idmuons=lepSel.selectIDMuons(treeMuons);
	  vector<NTMuon> isomuons=lepSel.selectIsolatedMuons(idmuons);
	  vector<NTMuon> tagmuons=lepSel.selectTagMuons(isomuons);
	  vector<NTTrack> tracks=*pTracks;
	  if(idisoboth_==0){
	    if(usetracks_){
	      effvars = lepSel.getMatchingProbeMuon(tracks, tagmuons, idmuons);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeMuon(treeMuons, tagmuons, idmuons);
	    }
	  }
	  else if(idisoboth_==1){
	    effvars = lepSel.getMatchingProbeMuon(idmuons, tagmuons, isomuons);
	  }
	  else if(idisoboth_==2){
	    if(usetracks_){
	      effvars = lepSel.getMatchingProbeMuon(tracks, tagmuons, isomuons);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeMuon(treeMuons, tagmuons, isomuons);
	    }
	  }

	}
	else{
	
	  double rho=pEvent->getIsoRho(2); //PUJets as in twiki

	  vector<NTElectron> rhoElectrons = *pElectrons;
	  //	  RhoIsoAdder.setRho(rho);
	  //	  RhoIsoAdder.addRhoIso(rhoElectrons);

	  vector<NTElectron> idelecs=lepSel.selectIDElectrons(rhoElectrons);
	  vector<NTElectron> isoelecs=lepSel.selectIsolatedElectrons(idelecs);
	  vector<NTElectron> tagelecs=lepSel.selectTagElectrons(isoelecs);
	  vector<NTSuClu>    suclus=*pSuClus;
	  if(idisoboth_==0){
	    if(usesuclus_){
	      effvars = lepSel.getMatchingProbeElec(suclus, tagelecs , idelecs);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeElec(rhoElectrons, tagelecs , idelecs);
	    }
	  }
	  else if(idisoboth_==1){
	    effvars = lepSel.getMatchingProbeElec(idelecs,tagelecs , isoelecs);
	  }
	  else if(idisoboth_==2){
	    if(usesuclus_){
	      effvars = lepSel.getMatchingProbeElec(suclus,tagelecs , isoelecs);
	    }
	    else{
	      effvars = lepSel.getMatchingProbeElec(rhoElectrons,tagelecs , isoelecs);
	    }
	  }

	}

	if(effvars.mass() ==0) continue;
	double eta=effvars.p4().Eta();
	double pt=effvars.p4().Pt();
	bool probepasses=effvars.passed();


	//fill probeDs
	//check which bin
	for(unsigned int i=0;i<Netabins;i++){
	  if(eta<etabins[i+1]){
	    bool filled=false;
	    for(unsigned int j=0;j<Nptbins;j++){
	      if(pt<ptbins[j+1]){
		invmassData[i][j].fill(effvars.mass());
		break;
		filled=true;
	      }
	    }
	    if(filled) break;
	  }
	}
        
	
	if(probepasses){
	  //check which bin
	  for(unsigned int i=0;i<Netabins;i++){
	    if(eta<etabins[i+1]){
	      bool filled=false;
	      for(unsigned int j=0;j<Nptbins;j++){
		if(pt<ptbins[j+1]){
		  invmassDataPassed[i][j].fill(effvars.mass());
		  break;
		  filled=true;
		}
	      }
	      if(filled) break;
	    }
	  }
	} // probeDpasses
      } //data tree fill
      cout << endl;
      
      //delete tdata;
      //delete tMC;

      dataFile->Close();
      
      //write Histos to file and tree with pt and eta bins

      TFile *outfile = new TFile(outname_, "RECREATE");

      for(unsigned int i=0; i< Netabins;++i){
	for(unsigned int j=0; j< Nptbins;++j){ 
	  invmassData[i][j].getTH1D(toTString(i) + " " + toTString(j) + " probedData") -> Write();
	  
	  invmassDataPassed[i][j].getTH1D(toTString(i) + " " + toTString(j) + " passedData") -> Write();
	  
	  invmassMC[i][j].getTH1D(toTString(i) + " " + toTString(j) + " probedMC") -> Write();
	  
	  invmassMCPassed[i][j].getTH1D(toTString(i) + " " + toTString(j) + " passedMC") -> Write();
	  
	}
      }
      
      //write tree with binvectors

      int netabins=etabins.size();
      int nptbins=ptbins.size();
      TTree * outtree = new TTree("binning", "binning");
      outtree->Branch("etabins",&etabins);
      outtree->Branch("ptbins",&ptbins);
      outtree->Branch("netabins",&netabins,"netabins/I");
      outtree->Branch("nptbins",&nptbins,"nptbins/I");
      outtree->Fill();
      outtree->Write();
      outfile->Close();

      //invmassMC[0][1].getTH1D(" probedMC") ->Draw();

      //delete outtree; delete outfile; delete tMC; delete tdata; delete dataFile; delete MCFile;
      
    }
    
  private:
    bool ismuons_;
    vector<float> etabins_;
    vector<float> ptbins_;
    float massmin_,massmax_;
    float binwidth_;
    TString mcfile_,datafile_,pufile_,outname_;
    int idisoboth_;
    bool usesuclus_,usetracks_,userhoiso_;
    
  };
}
