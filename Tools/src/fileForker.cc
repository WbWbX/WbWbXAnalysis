/*
 * fielForker.cc
 *
 *  Created on: 17 May 2016
 *      Author: kiesej
 */




/*  General thoughts on splitting MainAnalyzer
 *   needs to be removed:
 *  - PUreweighter etc
 *  - Some of the ttbar cross section automated normalization
 *  - Normalization info in general has to be
 *    generalised
 *  - setCacheProperties can go to TTree handler maybe
 *
 *
 *
 *
 *
 *  maybe something to go to Tools
 *
 *  incorporate the following capabilities:
 *  - managing pipes
 *
 *  - file IO
 *    - only provides read/write locks
 *    - some way to report back status
 *      (make function that only triggers on full 1%)
 *       - virtual function what is shown in addition to
 *         "%" before and after. should return a string
 *         for length check
 *    - some exception handling and forwarding
 *    - give some enums for errors/success feedback
 *    - no file list input here
 *
 *
 *
 *  some comments
 *   - bury ownindex somewhere here
 *   - daughter pids only accessible here
 *   - accessible only as const ref from inheriting classes (getChildidx)
 *   - after forking: set the indecies automatically
 *     - virtual function! associateIndex(void)
 *
 *
 *
 *  next inheritance layer
 *   - define file list input and which parameters
 *   - implements associateIndex(){index=getChildidx()...}
 */

#include "../interface/fileForker.h"
#include <sys/stat.h>
#include <iostream>
#include <sys/types.h>
#include <stdexcept>

namespace ztop{
fileForker::fileForker():debug(false),
		ischild_(false),spawnready_(false),
		maxchilds_(12),
		runningchilds_(0),
		donechilds_(0),
		lastspawned_(-1),
		PID_(0),
		ownchildindex_(FF_PARENTINDEX),
		processendcalled_(false)
{



}


fileForker::~fileForker(){
	//nothing to be done
}


fileForker::fileforker_status fileForker::prepareSpawn(){
	if(!createOutFile())
		return ff_status_parent_generror;
	lastspawned_=-1;
	size_t filenumber=inputfiles_.size();
	p_idx.open(filenumber);
	p_allowwrite.open(filenumber);
	p_askwrite.open(filenumber);
	p_status.open(filenumber);
	p_busystatus.open(filenumber);

	childPids_.resize(filenumber,0);

	busystatus_.resize(filenumber,0);
	childstatus_.resize(filenumber,ff_status_child_hold);
	spawnready_=true;
	return ff_status_parent_success;
}

fileForker::fileforker_status fileForker::spawnChilds(){
	if(debug)
		std::cout << "fileForker::spawnChilds" << std::endl;
	using namespace std;
	using namespace ztop;
	if(!spawnready_)
		throw std::logic_error("fileForker::spawnChilds: first prepare spawn");


	size_t filenumber=inputfiles_.size();
	//spawn
	if(filenumber>1){
		for(size_t i=0;i<filenumber;i++){
			while(p_busystatus.get(i)->preadready())
				busystatus_.at(i)=p_busystatus.get(i)->pread();
		}


		for(int i=lastspawned_+1;i<(int)filenumber;i++){
			if(runningchilds_>=maxchilds_)
				break;
			if(debug)
				std::cout << "last: " << lastspawned_<< " i: "<< i << std::endl;
			childPids_.at(i)=fork();
			if(childPids_.at(i)==0){//child process
				try{
					if(debug)
						std::cout << "fileForker::spawnChilds child "<<i << std::endl;
					ownchildindex_=p_idx.get(i)->pread(); //wait until parent got ok
					processendcalled_=false;
					process();
					if(!processendcalled_){
						throw std::logic_error("fileForker: implementation of process() must call processEndFunction() at its end!");
					}
					std::exit(EXIT_SUCCESS);
				}catch(std::exception& e){
					std::cout << "*************** Exception ***************" << std::endl;
					std::cout << e.what() << std::endl;
					std::cout << "in " << getInputFileName() << '\n'<<std::endl;
					writeReady_block(); //pro-forma
					writeDone(ff_status_child_exception);
					std::exit(EXIT_FAILURE);
				}
			}
			else{ //parent process
				if(debug)
					std::cout << "fileForker::spawnChilds parent "<< std::endl;
				ownchildindex_=FF_PARENTINDEX;//start processing
				p_idx.get(i)->pwrite(i); //send start signal
				childstatus_.at(i)=ff_status_child_busy;
				runningchilds_++;
				lastspawned_=i;

				//check for write requests
			}
		}

		//future: this could go to another thread, if so, make ++ and -- atomic
		//here, there is no root bogus, so it should be no problem (are pipes thread safe?)
		int it_readytowrite=checkForWriteRequest();
		if(it_readytowrite >= 0){ // daugh ready to write
			p_allowwrite.get(it_readytowrite)->pwrite(it_readytowrite);
			childstatus_.at(it_readytowrite)=ff_status_child_writing;
			if(debug)
				std::cout << "fileForker::spawnChilds waiting for write "<< std::endl;
			childstatus_.at(it_readytowrite)=(fileforker_status)p_status.get(it_readytowrite)->pread();
			//done
			runningchilds_--;
			donechilds_++;
		}

		if(donechilds_ == filenumber)
			return ff_status_parent_success;


	}
	else{ //only one process
		ownchildindex_=0;
		process();
	}


	for(size_t c=0;c<filenumber;c++){

	}
	return ff_status_parent_busy;
}

fileForker::fileforker_status fileForker::getStatus(const size_t& childindex)const{

	return childstatus_.at(childindex);

}

fileForker::fileforker_status fileForker::getStatus()const{
	if(debug){
		std::cout << "fileForker::getStatus: "<< donechilds_<<"/"<< inputfiles_.size()<< std::endl;
		std::cout << "child stats: " <<std::endl;
		for(size_t c=0;c<inputfiles_.size();c++){
			std::cout << c << "\t"  << (int)childstatus_.at(c) <<"\t"<<inputfiles_.at(c) <<std::endl;
		}
	}
	if(! inputfiles_.size())
		return ff_status_parent_success;
	if(lastspawned_<(int)inputfiles_.size()-1)
		return ff_status_parent_childstospawn;
	if(donechilds_<inputfiles_.size())
		return ff_status_parent_busy;
	return
			ff_status_parent_success;
}



int fileForker::getBusyStatus(const size_t& childindex){
	while(p_busystatus.get(childindex)->preadready())
		busystatus_.at(childindex)=p_busystatus.get(childindex)->pread();
	return busystatus_.at(childindex);
}

bool fileForker::writeReady_block(){
	if(ownchildindex_==FF_PARENTINDEX)
		return false;

	p_askwrite.get(ownchildindex_)->pwrite(ownchildindex_);
	p_allowwrite.get(ownchildindex_)->pread();
	return true;

}
void fileForker::writeDone(fileforker_status stat){
	p_status.get(ownchildindex_)->pwrite(stat);

}

void fileForker::reportStatus(fileforker_status stat){
	p_status.get(ownchildindex_)->pwrite(stat);
}
void fileForker::reportBusyStatus(int bstat){
	p_busystatus.get(ownchildindex_)->pwrite(bstat);
}

void fileForker::processEndFunction(){
	processendcalled_=true;
	writeReady_block() ;
	fileforker_status stat=writeOutput();
	writeDone(stat);
}

int fileForker::checkForWriteRequest(){
	for(size_t i=0;i<p_askwrite.size();i++){
		if(p_askwrite.get(i)->preadready()){
			p_askwrite.get(i)->pread(); //to free pipe again
			return i; //return first ready to write!
		}
	}
	return -1;
}

}




