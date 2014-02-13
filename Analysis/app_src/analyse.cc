#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
//#include "Analyzer.cc"
//should be taken care of by the linker!


void analyse(TString channel, TString Syst, TString energy, TString outfileadd, double lumi, bool dobtag, bool status,bool testmode,TString maninputfile,TString mode){ //options like syst..

    bool didnothing=false;
    //some env variables
    TString cmssw_base=getenv("CMSSW_BASE");
    TString scram_arch=getenv("SCRAM_ARCH");

    using namespace std;
    using namespace ztop;
    if(testmode){
        ztop::container1D::debug =false;
        ztop::containerStack::debug=false;
        ztop::containerStackVector::debug=false;
    }
    bool userandomtagging=true;

    TString inputfilewochannel="config.txt";
    TString inputfile=channel+"_"+energy+"_"+inputfilewochannel;
    // TString
    if(maninputfile!="")
        inputfile=maninputfile;
    //do not prepend path (batch submission)

    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[parameters-begin]");
    fr.setEndMarker("[parameters-end]");
    fr.readFile(inputfile.Data());


    TString treedir,jecfile,pufile,muonsffile,muonsfhisto,elecsffile,elecsfhisto,trigsffile,elecensffile,muonensffile; //...
    TString btagWP;

    if(lumi<0)
        lumi=fr.getValue<double>("Lumi");
    treedir=              fr.getValue<TString>("inputFilesDir");
    jecfile=   cmssw_base+fr.getValue<TString>("JECFile");
    muonsffile=cmssw_base+fr.getValue<TString>("MuonSFFile");
    muonsfhisto=          fr.getValue<TString>("MuonSFHisto");
    elecsffile=cmssw_base+fr.getValue<TString>("ElecSFFile");
    elecsfhisto=          fr.getValue<TString>("ElecSFHisto");
    trigsffile=cmssw_base+fr.getValue<TString>("TriggerSFFile");
    pufile=    cmssw_base+fr.getValue<TString>("PUFile");
    //elecensffile =cmssw_base+fr.getValue<TString>("ElecEnergySFFile");
    //muonensffile =cmssw_base+fr.getValue<TString>("MuonEnergySFFile");

    btagWP=               fr.getValue<TString>("btagWP");
    TString btagfile=     fr.getValue<TString>("btagFile");

    TString trigsfhisto="scalefactor eta2d with syst";


    std::cout << "inputfile read" << std::endl;

    //hard coded:

    system("mkdir -p output");
    TString outdir="output";


    if(dobtag){
        btagfile=outdir+"/"+ channel+"_"+energy+"_"+Syst+"_btags.root";
        system(("rm -f "+btagfile).Data());
    }


    ///set input files list etc (common)


    MainAnalyzer ana;
    ana.setMaxChilds(6);
    ana.setMode(mode);
    ana.setShowStatus(status);
    ana.setTestMode(testmode);
    ana.setLumi(lumi);
    ana.setDataSetDirectory(treedir);
    ana.getPUReweighter()->setDataTruePUInput(pufile+".root");
    ana.setChannel(channel);
    ana.setEnergy(energy);
    if(energy == "8TeV"){
        ana.getPUReweighter()->setMCDistrSum12();
    }
    else if(energy == "7TeV"){
        ana.getPUReweighter()->setMCDistrFall11();
    }
    ana.getElecSF()->setInput(elecsffile,elecsfhisto);
    ana.getMuonSF()->setInput(muonsffile,muonsfhisto);
    ana.getTriggerSF()->setInput(trigsffile,trigsfhisto);

    ana.getElecEnergySF()->setGlobal(1,0.5,0.5);
    ana.getMuonEnergySF()->setGlobal(1,0.5,0.5);

    ana.getTopPtReweighter()->setFunction(reweightfunctions::toppt);
    ana.getTopPtReweighter()->setSystematics(reweightfunctions::nominal);

    ana.setOutFileAdd(outfileadd);
    ana.setOutDir(outdir);
    ana.getBTagSF()->setMakeEff(dobtag);
    ana.setBTagSFFile(btagfile);
    ana.getBTagSF()->useRandomTechnique(userandomtagging);
    ana.getBTagSF()->setWorkingPoint(btagWP);
    ana.getJECUncertainties()->setFile((jecfile).Data());
    ana.getJECUncertainties()->setSystematics("no");
    if(testmode && dobtag){
        ana.setBTagSFFile(btagfile+"TESTMODE");
        system(("rm -f "+btagfile+"TESTMODE").Data());
    }
    //add indication for non-correlated syst by adding the energy to syst name!! then the getCrossSections stuff should recognise it

    //although it produces overhead, add background rescaling here?

    if(Syst=="nominal"){
        //all already defined
    }
    else if(Syst=="TRIGGER_down"){
        ana.getTriggerSF()->setSystematics("down");
    }
    else if(Syst=="TRIGGER_up"){
        ana.getTriggerSF()->setSystematics("up");
    }
    else if(Syst=="ELECSF_up"){
        ana.getElecSF()->setSystematics("up");
    }
    else if(Syst=="ELECSF_down"){
        ana.getElecSF()->setSystematics("down");
    }
    else if(Syst=="MUONSF_up"){
        ana.getMuonSF()->setSystematics("up");
    }
    else if(Syst=="MUONSF_down"){
        ana.getMuonSF()->setSystematics("down");
    }
    else if(Syst=="MUONES_up"){
        ana.getMuonEnergySF()->setSystematics("up");
    }
    else if(Syst=="MUONES_down"){
        ana.getMuonEnergySF()->setSystematics("down");
    }
    else if(Syst=="ELECES_up"){
        ana.getElecEnergySF()->setSystematics("up");
    }
    else if(Syst=="ELECES_down"){
        ana.getElecEnergySF()->setSystematics("down");
    }
    else if(Syst=="JES_up"){
        ana.getJECUncertainties()->setSystematics("up");
    }
    else if(Syst=="JES_down"){
        ana.getJECUncertainties()->setSystematics("down");
    }
    /*
  else if(Syst=="7TeV_JES_up"){
    ana.getJECUncertainties()->setSystematics("up");
  }
  else if(Syst=="7TeV_JES_down"){
    ana.getJECUncertainties()->setSystematics("down");
  }
     */ // etc

    else if(Syst=="JER_up"){
        ana.getJERAdjuster()->setSystematics("up");
    }
    else if(Syst=="JER_down"){
        ana.getJERAdjuster()->setSystematics("down");
    }
    else if(Syst=="PU_up"){
        ana.getPUReweighter()->setDataTruePUInput(pufile+"_up.root");
    }
    else if(Syst=="PU_down"){
        ana.getPUReweighter()->setDataTruePUInput(pufile+"_down.root");
    }
    else if(Syst=="BTAGH_up"){
        ana.getBTagSF()->setSystematic("heavy up");
    }
    else if(Syst=="BTAGH_down"){
        ana.getBTagSF()->setSystematic("heavy down");
    }
    else if(Syst=="BTAGL_up"){
        ana.getBTagSF()->setSystematic("light up");
    }
    else if(Syst=="BTAGL_down"){
        ana.getBTagSF()->setSystematic("light down");
    }
    else if(Syst=="TOPPT_up"){
        ana.getTopPtReweighter()->setSystematics(reweightfunctions::up);
    }
    else if(Syst=="TOPPT_down"){
        ana.getTopPtReweighter()->setSystematics(reweightfunctions::down);
    }
    ///////////////////file replacements/////////////
    else if(Syst=="TT_MATCH_up"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_ttmup.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttmup.root");
    }
    else if(Syst=="TT_MATCH_down"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_ttmdown.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttmdown.root");
    }
    else if(Syst=="TT_SCALE_up"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_ttscaleup.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttscaleup.root");
    }
    else if(Syst=="TT_SCALE_down"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_ttscaledown.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttscaledown.root");
    }
    else if(Syst=="Z_MATCH_up"){
        ana.setFilePostfixReplace("60120.root","60120_Zmup.root");
    }
    else if(Syst=="Z_MATCH_down"){
        ana.setFilePostfixReplace("60120.root","60120_Zmdown.root");
    }
    else if(Syst=="Z_SCALE_up"){
        ana.setFilePostfixReplace("60120.root","60120_Zscaleup.root");
    }
    else if(Syst=="Z_SCALE_down"){
        ana.setFilePostfixReplace("60120.root","60120_Zscaledown.root");
    }
    else if(Syst=="MT_6_down"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt166.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt166.5.root");
    }
    else if(Syst=="MT_3_down"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt169.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt169.5.root");
    }
    else if(Syst=="MT_1_down"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt171.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt171.5.root");
    }
    else if(Syst=="MT_1_up"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt173.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt173.5.root");
    }
    else if(Syst=="MT_3_up"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt175.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt175.5.root");
    }
    else if(Syst=="MT_6_up"){
        ana.setFilePostfixReplace("ttbar.root","ttbar_mt178.5.root");
        ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt178.5.root");
    }

    //......
    else{
        didnothing=true;
    }



    ana.setFileList(inputfile);
    ana.setSyst(Syst);


    std::cout << "doing btag: " << dobtag <<std::endl;



    if(didnothing){
        //create a file outputname_norun that gives a hint that nothing has been done
        //and check in the sleep loop whether outputname_norun exists
        //not needed because merging is done by hand after check, now
        std::cout << "analyse "<< channel <<"_" << energy <<"_" << Syst << " nothing done - maybe some input strings wrong" << std::endl;
    }
    else{
        //if PDF var not found make the same file as above
        //end of configuration

        // recreate file

        TString fulloutfilepath=ana.getOutPath()+".root";
        TString workdir=ana.getOutDir() +"../";
        TString batchdir=workdir+"batch/";

        system(("rm -rf "+fulloutfilepath).Data());

        int fullsucc=ana.start();

        ztop::container1DStackVector csv;

        if(fullsucc>=0){

            csv.loadFromTFile(ana.getOutPath()+".root",ana.getPlots()->getName());
            vector<TString> dycontributions;
            dycontributions << "Z#rightarrowll" << "DY#rightarrowll";
            if(!channel.Contains("emu")){
                rescaleDY(&csv, dycontributions);
                csv.writeAllToTFile(ana.getOutPath()+".root",true,!testmode);
            }
            else if(testmode){
                csv.writeAllToTFile(ana.getOutPath()+".root",true,!testmode);
            }

            if(!testmode){
                system(("touch "+batchdir+ana.getOutFileName()+"_fin").Data());
            }
            sleep(1);
            std::cout <<"\nFINISHED with "<< channel <<"_" << energy <<"_" << Syst <<std::endl;


        }
        else{
            std::cout << "at least one job failed!\n"
                    << "error code meaning: "
                    << "-99: std::exception thrown somewhere\n"
                    << "-3 : problems finding b-tagging efficiencies (file/file format) \n"
                    << "-2 : write to output file failed "
                    << "-1 : input file not found\n"
                    << std::endl;
            //put more once you introduce one
            if(!testmode){
                system(("touch "+batchdir+ana.getOutFileName()+"_failed").Data());
            }
        }
    }



}




//////////////////
#include "TtZAnalysis/Tools/interface/optParser.h"

int main(int argc, char* argv[]){

    using namespace ztop;

    optParser parse(argc,argv);
    TString channel= parse.getOpt<TString>  ("c","emu");         //-c channel
    TString syst   = parse.getOpt<TString>  ("s","nominal");     //-s <syst>
    TString energy = parse.getOpt<TString>  ("e","8TeV");        //-e default 8TeV
    double lumi    = parse.getOpt<float>    ("l",-1);            //-l default -1
    bool dobtag    = parse.getOpt<bool>     ("b",false);         //-b switches on default false
    TString outfile= parse.getOpt<TString>  ("o","");            //-o <outfile> should be something like channel_energy_syst.root // only for plots
    bool status    = parse.getOpt<bool>     ("S",false);         //-S enables default false
    bool testmode  = parse.getOpt<bool>     ("T",false);         //-T enables default false
    TString mode   = parse.getOpt<TString>  ("m","xsec");        //-m (xsec,....) default xsec changes legends? to some extend
    TString inputfile= parse.getOpt<TString>  ("i","");          //-i empty will use automatic
    parse.doneParsing();

    bool mergefiles=false;
    std::vector<TString> filestomerge;

    if(mergefiles){
        //do the merging with filestomerge
    }
    else{
        analyse(channel, syst, energy, outfile, lumi,dobtag , status, testmode,inputfile,mode );
    }
}
