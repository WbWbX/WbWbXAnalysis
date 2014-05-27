/*
 * fileReader.cc
 *
 *  Created on: Oct 10, 2013
 *      Author: kiesej
 */
#include "../interface/fileReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ztop{



bool fileReader::debug=false;







void fileReader::readFile(const std::string &filename){

    using namespace std;
    lines_.clear();
    ifstream myfile (filename.data());
    if (myfile.is_open())
    {
        tempinfilename_=filename;
        if(debug){
            std::cout << "fileReader::readFile: opened file.\n searching for start marker " << start_
                    <<" until end marker " << end_ << "\n"
                    << "delimiter: " << delimiter_ << " comments: " << comment_ << std::endl;

        }
        if(blindmode_){
            vector <string> record;
            lines_.push_back(record);
        }
        bool started=false;
        bool ended=false;
        while ( myfile.good() )
        {
            if(ended)
                break;
            string s;

            if (!getline( myfile, s )) break;

            if(blindmode_){
                lines_.at(0).push_back(s);
                continue;
            }
            else{
                trimcomments(s);
                trim(s);
            }

            if(s.size()<1)
                continue;
            if(!blindmode_ && !started && start_.size() !=0 && s.find(start_) == string::npos) //wait for start marker on line basis
                continue;

            if(debug && !started)
                std::cout << "found start marker in line" << std::endl;

            //if(end_.size() !=0 && s.compare(end_) == 0) //wait for start marker
            //break;

            istringstream ss( s );
            vector <string> record;
            bool noentry=true;
            while (ss)
            {

                string s2;
                if (!getline( ss, s2, *delimiter_.data() )) break;
                if(!started && start_.size() !=0 && s2 != start_){ //wait for start marker on word by word basis
                    if(debug)
                        std::cout << s2 << " not yet startmarker" << std::endl;
                    continue;
                }
                else if(!started){
                    started=true;
                    if(start_.size() >0){
                        if(debug)
                            std::cout << s2 << " is start marker, begin reading" << std::endl;
                        continue; //dont read start marker itself
                    }
                }
                if(end_.size() >0 && s2 == end_){
                    if(debug)
                        std::cout << s2 << " is end marker, stop reading" << std::endl;
                    ended=true;
                    break;
                }
                if(debug)
                    std::cout << "read \"" << s2 << "\""<<std::endl;
                trim(s2);
                if(debug)
                    std::cout << "trimmed to \"" << s2 << "\"" << std::endl;
                if(s2.size()<1)
                    continue;
                noentry=false;
                record.push_back( s2 );
            }
            if(!ended && !noentry){
                lines_.push_back(record);
                record.clear();
            }
        }

        myfile.close();
    }
    else{
        cout << "fileReader::readFile: could not read file" <<std::endl;
        return;
    }
}
std::vector<std::string> fileReader::getData(const size_t &line) const{
    if(line>=lines_.size()){
        std::cout << "fileReader::getData: line out of range" <<std::endl;
        return std::vector<std::string>();
    }
    return lines_.at(line);
}
/**
 * if file has entries like somevariable=blabla
 * getValue("somevariable") will return "blabla"
 * if value is definied several times, an exception is thrown
 * less performance but safer
 * can be switched off by bool
 * then last value is returned
 *
 * if value is not found empty string will be returned
 */
std::string fileReader::getValueString(const std::string & val, bool checkdoubles){
    std::string out;
    size_t count=0;
    //search all entries:
    for(size_t line=0;line<nLines();line++){
        for(size_t entr=0;entr<nEntries(line);entr++){
            std::string & s=lines_.at(line).at(entr);
            size_t pos=s.find(val);
            if(pos != std::string::npos){ //found
                if(checkdoubles && count > 0){//except
                    throw std::logic_error("fileReader::getValue: value defined twice! Source of errors!");
                }
                pos=s.find("=");
                if(pos!= std::string::npos){
                    if(pos+1 >= s.size())
                        return std::string("");
                    std::string s2(s.begin()+pos+1,s.end());
                    trim(s2);
                    if(s2.size()<1)
                        return std::string("");
                    out=s2;
                    count++;
                }
            }
        }
    }
    return out;
}
}//ztop
