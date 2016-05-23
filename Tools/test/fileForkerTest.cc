/*
 * fileForkerTest.cc
 *
 *  Created on: 19 May 2016
 *      Author: kiesej
 */




#include "../interface/fileForker.h"
#include "../interface/textFormatter.h"

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

namespace ztop{


class testFork: public fileForker{
public:
	testFork():fileForker(){}
	~testFork(){}

protected:
	bool createOutFile()const{return true;}

	void process(){
		//std::cout << getInputFileName() << std::endl;
		for(int i=0;i<101;i++){
			usleep(50e3*getInputFileName().size());
			reportBusyStatus(i);
		}

		processEndFunction();
	}

	fileforker_status writeOutput(){
		std::cout << "writing...." << getInputFileName()  << std::endl;
		sleep(5);
		return ff_status_child_success;
	}

};

}

int main(){

	using namespace ztop;

	std::vector<std::string> ins;
	ins.push_back("file1");
	ins.push_back("file 2");
	ins.push_back("file  3");
	ins.push_back("file 4");
	ins.push_back("file 5");
	ins.push_back("file   6");
	ins.push_back("file     7");
	ins.push_back("file  8");

	testFork f;
	f.debug=false;
	f.setInputFiles(ins);
	f.setMaxChilds(3);
	f.prepareSpawn();

	fileForker::fileforker_status stat=fileForker::ff_status_parent_busy;
	while(stat==fileForker::ff_status_parent_busy || stat== fileForker::ff_status_parent_childstospawn){

		f.spawnChilds();
		stat=f.getStatus();
		for(size_t i=0;i<ins.size();i++)
			std::cout << textFormatter::fixLength(ins.at(i),10) << ' ' << f.getStatus(i) << "  " << f.getBusyStatus(i)<<std::endl;
		//std::cout << "statcheck: " << stat << std::endl;
		sleep (1);
	}

}
