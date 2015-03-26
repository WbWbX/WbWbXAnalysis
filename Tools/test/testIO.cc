/*
 * testIO.cc
 *
 *  Created on: Mar 5, 2015
 *      Author: kiesej
 */




#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include <fstream>
#include <string>
#include "TtZAnalysis/Tools/interface/histoBin.h"
#include "TtZAnalysis/Tools/interface/histo1D.h"
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include <ctime>

void WriteString(std::ofstream& file,const std::string& str)
{
	// get the length of the string data
	unsigned len = str.size();

	// write the size:
	file.write( reinterpret_cast<const char*>( &len ), sizeof(len) );

	// write the actual string data:
	file.write( str.c_str(), len );
}

std::string ReadString(std::ifstream& file)
{
	// this probably isn't the optimal way to do it, but whatever
	std::string str;

	// get the length
	unsigned len;
	file.read( reinterpret_cast<char*>( &len ), sizeof(len) );

	// we can't read to string directly, so instead, create a temporary buffer
	if(len > 0)
	{
		char* buf = new char[len];
		file.read( buf, len );
		str.append( buf, len );
		delete[] buf;
	}
	return str;
}

#include <stdint.h>

class testclass{
public:
	testclass():name(""),data(0),otherdata(0){}

	void writeToFile(std::string fname){
		std::fstream saveFile;
		saveFile.open(fname.data(), std::fstream::binary | std::fstream::app | std::fstream::out );
		uint16_t len=name.size();
		saveFile.write(reinterpret_cast<char *>(&len), sizeof(len));
		saveFile.write( name.c_str(), len );
		saveFile.write(reinterpret_cast<char *>(&data), sizeof(data));
		saveFile.write(reinterpret_cast<char *>(&otherdata), sizeof(otherdata));
		saveFile.close();
	}

	void readFromFile(std::string fname){
		std::fstream saveFile;
		saveFile.open(fname.data(), std::fstream::binary | std::fstream::app | std::fstream::in );

		uint16_t len;
		saveFile.read( reinterpret_cast<char*>( &len ), sizeof(len) );
		std::string str="";
		if(len > 0){
			char* buf = new char[len];
			saveFile.read( buf, len );
			str.append( buf, len );
			delete[] buf;
		}
		name=str;
		saveFile.read( reinterpret_cast<char*>( &data ), sizeof(data) );
		saveFile.read( reinterpret_cast<char*>( &otherdata ), sizeof(otherdata) );
		saveFile.close();

	}

	std::string name;
	int data;
	float otherdata;

};
#include <iostream>
invokeApplication(){

	using namespace ztop;
	/*
	TRandom3 * rand=new TRandom3();
	std::cout << "creating containers" <<std::endl;
	histo1D c(histo1D::createBinning(10000,0,100));
	histo1D c2=c.createOne();
	c2*=100.;
	for(size_t i=0;i<c.getBins().size();i++){
		c2.setBinStat(i,i);
	}
	c=c2.createPseudoExperiment(rand);
	for(size_t i=0;i<c.getBins().size();i++){
		//c.getBin(i).setName("verylongbinname"+toTString(i));
		c.setBinEntries(i,i);
		c.setBinStat(i,i);
	}

	for(size_t i=0;i<400;i++){
		c2=c2.createPseudoExperiment(rand);
		c.addErrorContainer(toTString(i)+"_up",c2);
		c2=c2.createPseudoExperiment(rand);
		c.addErrorContainer(toTString(i)+"_down",c2);
	}
	//time_t now = time(0);
	 time_t now = time(0);
	std::cout << "writing file uncomp" << ctime(&now) << std::endl;
	c.writeToFile("/nfs/dust/cms/user/kiesej/testfile.ztop");
	now = time(0);
	std::cout << "writing file root" << ctime(&now)<<std::endl;
	c.writeToTFile("/nfs/dust/cms/user/kiesej/testfile.root");
	now = time(0);
	std::cout << "writing file comp" << ctime(&now)<<std::endl;
	c.writeToFile("/nfs/dust/cms/user/kiesej/testfile.gz");
	now = time(0);
	std::cout << "writing file comp done" << ctime(&now)<<std::endl;

	histo1D c3;
	now = time(0);
	std::cout << "read file" << ctime(&now)<<std::endl;
	c3.readFromFile("/nfs/dust/cms/user/kiesej/testfile.ztop");
	now = time(0);
	std::cout << "read file comp" << ctime(&now)<<std::endl;
	c3.readFromFile("/nfs/dust/cms/user/kiesej/testfile.gz");
	now = time(0);
	std::cout << "read file comp done" << ctime(&now)<<std::endl;

	for(size_t i=0;i<c.getBins().size();i++){
		ZTOP_COUTVAR(c3.getBinContent(i));
		ZTOP_COUTVAR(c.getBinContent(i));
		ZTOP_COUTVAR(c3.getBinStat(i));
		ZTOP_COUTVAR(c.getBinStat(i));
		ZTOP_COUTVAR(c3.getBinEntries(i));
		ZTOP_COUTVAR(c.getBinEntries(i));
		ZTOP_COUTVAR(c3.getBin(i).getName());
		ZTOP_COUTVAR(c.getBin(i).getName());
	}

	//stacks
	ztop::histoStack stack;
	for(size_t i=0;i<20;i++){
		stack.push_back(c*(i+0.2),toTString(i),1,i);
	}
	stack.push_back(c*(20+0.2),"data",1,99);
	now = time(0);
	std::cout << "writing stakc file" << ctime(&now)<<std::endl;
	stack.writeToFile("/nfs/dust/cms/user/kiesej/testfile_stack.ztop");
	now = time(0);
	std::cout << "writing  stack file done" << ctime(&now)<<std::endl;



	if(c==c3){
		std::cout << "equal" <<std::endl;
	}

//	system("cd /nfs/dust/cms/user/kiesej");
	system("ls -ltrh /nfs/dust/cms/user/kiesej/testfile*");
//	system("cd -");

	delete rand;
	 */
	/*
	histoStackVector csv;
	time_t now = time(0);
	std::cout << "loading from root file" << ctime(&now)<<std::endl;
	csv.loadFromTFile("/nfs/dust/cms/user/kiesej/emu_7TeV_175.5_nominal_syst.root");
	now = time(0);
	std::cout << "loading from root file done" << ctime(&now)<<std::endl;
	std::cout << "writing to root file" << ctime(&now)<<std::endl;
	csv.writeAllToTFile("/nfs/dust/cms/user/kiesej/emu_7TeV_175.5_nominal_syst2.root",true,true);
	now = time(0);
	std::cout << "writing to root file done" << ctime(&now)<<std::endl;

	std::cout << "writing to ztop file" << ctime(&now)<<std::endl;
	csv.writeToFile("/nfs/dust/cms/user/kiesej/emu_7TeV_175.5_nominal_syst.ztop");
	now = time(0);
	std::cout << "writing to ztop file done" << ctime(&now)<<std::endl;
	histoStackVector csv2;
	std::cout << "loading from ztop file" << ctime(&now)<<std::endl;
	csv2.readFromFile("/nfs/dust/cms/user/kiesej/emu_7TeV_175.5_nominal_syst.ztop");
	now = time(0);
	std::cout << "loading frpm ztop file done" << ctime(&now)<<std::endl;

//csv2.setName("fromztop");
	if(csv != csv2){
		std::cout << "not equal!!" <<std::endl;
	}
	csv2.writeAllToTFile("/nfs/dust/cms/user/kiesej/emu_7TeV_175.5_nominal_syst_plots.root",true,false);

	std::string filename="testoutfile";
	std::fstream saveFile;
	saveFile.open(filename.data(), std::ios_base::binary| std::ios_base::trunc | std::fstream::out );

	ztop::histo1D::c_makelist=false;

	std::vector<histo1D> bla;
	std::vector<float> bins=histo1D::createBinning(1,1,2);
	for(size_t i=0;i<20;i++)
		bla.push_back(histo1D(bins,"bla"+toTString(i)));

	std::cout << "bla" <<std::endl;

	IO::serializedWrite(bla,saveFile);
	saveFile.close();

	saveFile.open(filename.data(), std::ios_base::binary | std::fstream::in );

	bla.clear();
	IO::serializedRead(bla,saveFile);
	saveFile.close();
	for(size_t i=0;i<bla.size();i++)
		std::cout << bla.at(i).getName() <<std::endl;
	//bla.clear();
	std::cout << "bla1" <<std::endl;
	 */


	TRandom3 * rand=new TRandom3();

	histoStackVector csvfull;
	int sysc=0;
	for(size_t nsys=0;nsys<129;nsys++){

		histoStackVector csv;
		//histoStackVector::debug=true;
		for(float smpl=0.;smpl<20;smpl++){

			histo1D::c_makelist=true;
			histo1D somecont(histo1D::createBinning(3000,1,2),"blablaname","xaxis","yaxis");
			histo1D somecont2(histo1D::createBinning(9000,-1,4),"blablaname2","xaxis2","yaxis2");
			histo1D somecont3(histo1D::createBinning(1000,8,100),"blablaname3","xaxis3","yaxis3");
			histo1D somecont4(histo1D::createBinning(9000,-100,400),"blablaname4","xaxis2","yaxis2");
			float weight=1+smpl/3+rand->Gaus(1, (nsys/30));
			for(size_t i=0;i< 2000;i++){
				somecont.fill(rand->Gaus(0.5,0.1*(smpl/4+1)),weight);
				somecont2.fill(rand->Gaus(3*(smpl/5+1),2),weight);
				somecont3.fill(rand->Gaus(60*(smpl/10+1),10),weight);
				somecont4.fill(rand->Gaus(60*(20*(smpl/10+1)),10),weight);
			}



			csv.addList(toTString(smpl),630+smpl,1,smpl+1);

		}
		TString sys=toTString(sysc);
		if(nsys%2 ==0){
			sys+="_up";
			sysc++;
		}
		else
			sys+="_down";
		if(nsys<1){
			std::cout << "defined nominal" <<std::endl;
			csvfull=csv;
		}
		else{
			//histoStack::debug=true;
			//histo1D::debug=true;
			std::cout << "added error " << sys<<std::endl;
			csvfull.addErrorStackVector(sys,csv);
			//histo1D::debug=false;
		}
	}
	std::cout << "add pseudo " <<std::endl;
	//histo1D::debug=true;
	csvfull.addPseudoData(rand);

	histoStackVector csvcp=csvfull;

	coutDateTime();
	std::cout << "writing to root\n" <<std::endl;
	csvfull.writeAllToTFile("/nfs/dust/cms/user/kiesej/rootfile.root",true,true);

	coutDateTime();
	std::cout << "writing to ztop\n" <<std::endl;
	csvcp.writeToFile("/nfs/dust/cms/user/kiesej/ztopfile.ztop");

	coutDateTime();
	std::cout << "reading from ztop\n" <<std::endl;
	histoStackVector csv2;
	csv2.readFromFile("/nfs/dust/cms/user/kiesej/ztopfile.ztop");

	coutDateTime();
	std::cout << "writing to root plots\n" <<std::endl;
	//csv2.writeAllToTFile("/nfs/dust/cms/user/kiesej/rootfile2.root",true,false);

	if(csv2 == csvfull){
		std::cout << "equal!" <<std::endl;
	}


	return 0;
}
