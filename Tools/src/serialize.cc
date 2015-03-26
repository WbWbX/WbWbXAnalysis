/*
 * serialize.cc
 *
 *  Created on: Mar 13, 2015
 *      Author: kiesej
 */




#include "../interface/serialize.h"
#include <iostream>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{
namespace IO{

#define ZTOP_IO_FILEHEADER "Ztop file format\nbinary object storage, version: "

/**
 * compression has to be done afterwards
 * This takes care of the ztop headers
 */
void openOutFile(std::ofstream& saveFile,const std::string& filename){
	saveFile.open(filename.data(), std::ios_base::binary | std::ios_base::trunc | std::fstream::out );
	if(!saveFile){
		std::string errstr="ztop::IO::openInFile: could not open output file "+filename;
		throw std::runtime_error(errstr);
	}
	//add ztop header
	std::string header=ZTOP_IO_FILEHEADER;
	serializedWrite(header,saveFile);
	int version=ZTOP_IO_FORMATVERSION;
	serializedWrite(version,saveFile);
}

/**
 * compression has to be done afterwards
 * This takes care of the ztop headers
 */
void openInFile(std::ifstream& saveFile,const std::string& filename){
	saveFile.open(filename.data(), std::ios_base::binary | std::fstream::in );
	if(!saveFile){
		std::string errstr="ztop::IO::openInFile: could not open input file "+filename;
		throw std::runtime_error(errstr);
	}
	std::string rheader;
	serializedRead(rheader,saveFile);
	std::string header=ZTOP_IO_FILEHEADER;
	if(header!=rheader){
		throw std::runtime_error("ztop::IO::openInFile: not a ztop file format!");
	}
	int version;
	serializedRead(version,saveFile);
	if(version != (int)ZTOP_IO_FORMATVERSION)
		throw std::runtime_error("ztop::IO::openInFile: wrong version, likely the input is not usable");
}
#undef ZTOP_IO_FILEHEADER

}
}
