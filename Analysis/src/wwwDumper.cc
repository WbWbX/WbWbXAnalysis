/*
 * wwwDumper.cc
 *
 *  Created on: Feb 21, 2014
 *      Author: kiesej
 */

#include "../interface/wwwDumper.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include <time.h>
#include <fstream>
#include <string>
#include "WbWbXAnalysis/Tools/interface/fileReader.h"

namespace ztop{

void wwwDumper::makePlotHTML(TString title, TString descr, TString where , ztop::histoStackVector& csv){
    std::ofstream fileout((where+"index.html").Data());

    using namespace std;

    string header=(string)"<html><head><title>"+ (string)title  +(string)"</title></head><body>";
    header+="<b>"+title + "</b>\n<br/>\n<hr /><br/>\n";
    header+=descr;
    header+="\n<hr /><br/>\n";
    fileout << header;

    //some table stuff for the plots
    std::vector<TString> names=csv.getStackNames(true);
    std::vector<TString> fullnames=csv.getStackNames(false);
    string plotspart;

    for(size_t i=0;i<names.size();i++){
        plotspart+="<b>" + names.at(i) + "</b><br/>\n";
        for(size_t j=0;j<fullnames.size();j++){
            if(fullnames.at(j).BeginsWith(names.at(i)+" step")){
                TString newname=fullnames.at(j);
                newname.ReplaceAll(" ","_");
                plotspart+="<img src="+ newname +".png width=\"270\" height=\"175\" alt=" + newname +".png />";
            }
        }
        plotspart+="\n<hr /><br/>\n";
    }
    csv.printAll("",where,".png");

    fileout << plotspart;

    string endfile="</body>";
    fileout << endfile;

    fileout.close();
}
int wwwDumper::appendToIndex(TString title, TString description,TString plotdir, TString indexfile){
    using namespace ztop;
    using namespace std;

    string strReplace="<!--NEW ENTRY HERE - DON'T DELETE-->";

    string strNew = "<!--NEW ENTRY HERE - DON'T DELETE-->";
    strNew+="\n";
    strNew+=(string)"<b>" + (string)"<a href=\"" +plotdir+ "/index.html\">"+title+"</a></b>\n<br/>";
    strNew+=description+"\n<br/><br/><hr />\n";
    std::ifstream filein(indexfile); //File to read from
    std::ofstream fileout((indexfile+"_tmp").Data()); //Temporary file
    if(!filein || !fileout)
    {
        cout << "Error opening files!" << endl;
        return -1;
    }

    string strTemp;
    //bool found = false;
    while(filein.good())
    {
        if (!getline( filein, strTemp )) break;

        if(strTemp == strReplace){
            strTemp = strNew;
            //found = true;
        }
        strTemp += "\n";
        fileout << strTemp;
        //if(found) break;
    }
    fileout.close();
    filein.close();
    system(("mv "+indexfile+"_tmp "+indexfile).Data());
    return 0;
}
int wwwDumper::dumpToWWW(TString infile, TString title,TString description){
    using namespace ztop;

    if(title ==""){
        std::cerr << "at least title needs to be specified" << std::endl;
        return -1;
    }


    TString dumpdir="/afs/desy.de/user/k/kiesej/www/analysisDump/";
    TString index="index.html";
    TString fullIndex=dumpdir+index;

    std::ofstream testtoken((dumpdir+"testtmp").Data());
    if(!testtoken){
        std::cerr << "File " << dumpdir<< " not accessible, no token?" <<std::endl;
        return -1;
    }
    system(("rm -f "+dumpdir+"testtmp").Data());



    TString plotdir=title;
    plotdir.ReplaceAll(" ","_");
    time_t seconds;

    seconds = time (NULL);
    plotdir+=toTString<time_t>(seconds);

    fileReader fr;
    fr.setComment("<br/>");
    std::string shortdescr=description.Data();
    fr.trimcomments(shortdescr);


    appendToIndex(title,shortdescr,plotdir,fullIndex);
    plotdir+="/";
    plotdir=dumpdir+plotdir;
    system(("mkdir "+plotdir).Data());
    histoStackVector csv;
    TString csvname=infile;
    csvname.ReplaceAll(".root","");
    csv.loadFromTFile(infile,csvname);//getFromFile(infile);
    makePlotHTML(title,  description, plotdir,csv);
    return 0;
}



}
