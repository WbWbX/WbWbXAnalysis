#ifndef Analysis_Utils_h
#define Analysis_Utils_h

#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
//#include "MainAnalyzer.h"
#include <cstdlib>

/*
template<class a>
void recreateRelations(typename std::vector<a *> & mothers, typename std::vector<b *> & daughters){

  for(size_t j=0;i<mothers.size();i++){
    a * mother=mothers.at(i);
    for(size_t j=0;j<daughters.size();j++){
      b * daughter=daughters.at(j);
      typename std::vector<a *> mothers;


    }
  }

}

*/
///DON'T !!! do options stuff here!!!

////options parser

TString getSyst(int argc, char* argv[]){
  using namespace ztop;
  TString out;
  bool foundoption=false;
  for(int i=1;i<argc;i++){ 
    if (i + 1 != argc){
      if((TString)argv[i] == "-s"){
	foundoption=true;
	i++;
      }
      else if(((TString)argv[i]).Contains("-")){ //any other option
	foundoption=false;
      }
      if(foundoption){
	TString temp=argv[i];
	out = temp;
      }
    }
  }
  return out;
}

TString getOutFile(int argc, char* argv[]){
  TString out;
  for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    if (i + 1 != argc){
      if((TString)argv[i] == "-o")
	out=(TString)argv[i+1];
      // std::cout << "outfile " << out << std::endl;
    }
  }
  return out;
}

TString getChannel(int argc, char* argv[]){
  TString out;
  for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    if (i + 1 != argc){
      if((TString)argv[i] == "-c")
	out=(TString)argv[i+1];
      // std::cout << "outfile " << out << std::endl;
    }
  }
  return out;
}

TString getEnergy(int argc, char* argv[]){
  TString out="8TeV";
  for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    if (i + 1 != argc){
      if((TString)argv[i] == "-e")
	out=(TString)argv[i+1];
      // std::cout << "outfile " << out << std::endl;
    }
  }
  return out;
}
TString getOutfile(int argc, char* argv[]){
  TString out="def_out.root";
  for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    if (i + 1 != argc){
      if((TString)argv[i] == "-o")
	out=(TString)argv[i+1];
      // std::cout << "outfile " << out << std::endl;
    }
  }
  return out;
}
float getLumi(int argc, char* argv[]){
  double out=19100;
  for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    if (i + 1 != argc){
      if((TString)argv[i] == "-l")
	out=atof(argv[i+1]);
      // std::cout << "outfile " << out << std::endl;
    }
  }
  return out;
}
bool prepareBTag(int argc, char* argv[]){
for(int i=1;i<argc;i++){
    //  std::cout << argv[i] << std::endl;; 
    //if (i + 1 != argc){
      if((TString)argv[i] == "-b")
	return true;
      // std::cout << "outfile " << out << std::endl;
      // }
  }
 return false;
}

void rescaleDY(ztop::container1DStackVector * vec, std::vector<TString> contributions, double scalescale=1, bool textout=true, TString identifier="dilepton invariant massZ "){

  using namespace ztop; 

  std::vector<TString> ident;ident.clear();
  std::vector<double> scales;scales.clear();
  for(int i=5;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymc = 0;
    std::vector<TString> allbutdyanddata;

    for(unsigned int j=0;j<contributions.size();j++){
      dymc += vec->getStack(identifier+stepstring).getContribution(contributions.at(j)).integral();
      allbutdyanddata << contributions.at(j);
    }
    double d = vec->getStack(identifier+stepstring).getContribution("data").integral();
    allbutdyanddata << "data";
    double rest = vec->getStack(identifier+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(rest==0) rest=1;
    double scale = (d-rest)/dymc;
    scales << scale*scalescale;
    ident << stepstring;
    if(textout) std::cout << "Scalefactor for "<< vec->getName() << " " << stepstring << ": " << scale << std::endl;
  }
  //  ztop::container1DStackVector rescaled=vec;
  for(unsigned int i=0;i<contributions.size();i++){
    vec->multiplyNorms(contributions.at(i), scales, ident);
  }

}


#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

template<class T>
class IPCPipe{
public:
IPCPipe(){
  fds[0].events = POLLRDNORM | POLLIN;
  fds[1].events = POLLOUT | POLLWRBAND;;
  pipe(pfds);
}
  ~IPCPipe(){close(pfds[0]);close(pfds[1]);}

  T pwrite(T c){return write(pfds[1], &c , 256);}
  T pread(){read(pfds[0], buf, 256);return buf[0];}

  int preadready(){
    fds[0].fd=pfds[0];
    fds[1].fd=pfds[1];
    poll(fds, 2, 100);
    if (fds[0].revents & (POLLRDNORM | POLLIN))
      return 1;
    else 
      return 0;
  }
  
  
private:
  int pfds[2];
  struct pollfd fds[2];
  T buf[256];

};

template<class T>
class IPCPipes{
public:
  IPCPipes(){size_=0;}
  IPCPipes(size_t Size){size_=Size;openPipes();}
  ~IPCPipes(){if(size_!=0)closePipes();}

  void open(size_t numPipes){size_=numPipes;openPipes();}
  size_t size(){return size_;}

  IPCPipe<T> * get(size_t i){if(i<size_) return ipcs_[i]; else return 0;}
  // size_t pollAll(int timeout=100)

  private:
  size_t size_;
  std::vector<IPCPipe<T> *> ipcs_;
  void openPipes(){
    for(size_t i=0;i<size_;i++){
      ipcs_.push_back(new IPCPipe<T>);
    }
  }
  void closePipes(){
    for(size_t i=0;i<ipcs_.size();i++)
      if(ipcs_.at(i)) delete ipcs_.at(i);
  }
};

template<class T>
bool allEqual(std::vector<T> vec, T val){
  for(size_t i=0;i<vec.size();i++){
    if(vec.at(i) != val)
      return false;
  }
  return true;
}
template<class T>
bool NoneEqual(std::vector<T> vec, T val){
  for(size_t i=0;i<vec.size();i++){
    if(vec.at(i) == val)
      return false;
  }
  return true;
}


#endif
