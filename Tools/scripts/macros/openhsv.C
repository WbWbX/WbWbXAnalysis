/*
 * openhsv.C
 *
 *  Created on: 28 Sep 2016
 *      Author: jkiesele
 */


ztop::histoStackVector * _hsv=0;

void openhsv(std::string infile){

	_hsv=new ztop::histoStackVector();
	try{
		_hsv->readFromFile(infile);
	}
	catch(std::exception& e){
		std::cout << "failed to read histoStackVector from file " << infile << "\nerror message below:"<< std::endl;
		std::cout << e.what() <<std::endl;
		delete _hsv;
		_hsv=0;
	}
	std::cout << "read histoStackVector as _hsv" <<std::endl;
}
