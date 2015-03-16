/*
 * serialize.h
 *
 *  Created on: Mar 9, 2015
 *      Author: kiesej
 */

#ifndef SERIALIZE_H_
#define SERIALIZE_H_

//#include <fstream>
#include "TString.h"
#include <string>
#include "stdint.h"
#include <stdexcept>
#include <fstream>

#define ZTOP_IO_FORMATVERSION 1

namespace ztop{
namespace IO{

//static bool debug=false;

template <class T, class U>
void serializedWrite(const T& in, U& saveFile);

template <class T, class U>
void serializedRead(T& in, U& saveFile);


/**
 * compression has to be done afterwards
 * This takes care of the ztop headers
 */
void openOutFile(std::ofstream& stream,const std::string& filename);

/**
 * compression has to be done afterwards
 * This takes care of the ztop headers
 */
void openInFile(std::ifstream& stream,const std::string& filename);
////////implementation



template <class T, class U>
inline void serializedWrite(const T& in, U& saveFile){
	saveFile.write(reinterpret_cast<const char *>(&in), sizeof(in));
}
template <class T, class U>
inline void serializedRead(T& in, U& saveFile){
	saveFile.read(reinterpret_cast< char *>(&in), sizeof(in));
}

//TString spec

template <class U>
inline void serializedWrite(const TString& in, U& saveFile){
	if(in.Length()>65535)
		throw std::out_of_range("serializedWrite: strings are limited to uint16_t length");
	uint16_t len=in.Length();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	saveFile.write(in.Data(), len);
}

template <class U>
inline void serializedRead(TString& in, U& saveFile){
	uint16_t len=0;
	saveFile.read( reinterpret_cast<char*>( &len ), sizeof(len) );
	in="";
	if(len > 0){
		char* buf = new char[len];
		saveFile.read( buf, len );
		in.Append(buf,len);
		delete[] buf;
	}
}





//std::string spec

template <class U>
inline void serializedWrite(const std::string& in, U& saveFile){
	if(in.length()>65535)
		throw std::out_of_range("serializedWrite: strings are limited to uint16_t length");
	uint16_t len=in.length();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	saveFile.write(in.data(), len);
}

template <class U>
inline void serializedRead(std::string& in, U& saveFile){
	uint16_t len=0;
	saveFile.read( reinterpret_cast<char*>( &len ), sizeof(len) );
	in="";
	if(len > 0){
		char* buf = new char[len];
		saveFile.read( buf, len );
		in = std::string(buf,len);
		delete[] buf;
	}
}



//vector not string!
/*
 * only for flat data types not containing pointers
 */

template <class T, class U>
inline void serializedWrite(const std::vector<T>& in, U& saveFile){
	size_t len=in.size();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	for(size_t i=0;i<len;i++)
		serializedWrite(in.at(i),saveFile);
}

/*
 * only for flat data types not containing pointers
 */
template <class T, class U>
inline void serializedRead(std::vector<T>& in, U& saveFile){
	size_t len=0;
	saveFile.read(reinterpret_cast< char *>(&len), sizeof(len));
	in.resize(len,T());
	for(size_t i=0;i<len;i++)
		serializedRead(in.at(i),saveFile);
}

//////compat

template <class T, class U>
inline void serializedWriteVector(const std::vector<T>& in, U& saveFile){
	serializedWrite(in,saveFile);
}

template <class T, class U>
inline void serializedWriteStringVector(const std::vector<T>& in, U& saveFile){
	serializedWrite(in,saveFile);
}

template <class T, class U>
inline void serializedReadVector(std::vector<T>& in, U& saveFile){
	serializedRead(in,saveFile);
}
template <class T, class U>
inline void serializedReadStringVector(std::vector<T>& in, U& saveFile){
	serializedRead(in,saveFile);
}
template <class U>
inline void serializedReadShort(std::string& in, U& saveFile){
	serializedRead(in,saveFile);
}
template <class U>
inline void serializedReadShort(TString& in, U& saveFile){
	serializedRead(in,saveFile);
}
template <class U>
inline void serializedWriteShort(const std::string& in, U& saveFile){
	serializedWrite(in,saveFile);
}
template <class U>
inline void serializedWriteShort(const TString& in, U& saveFile){
	serializedWrite(in,saveFile);
}

}
}

/**
 * call after having specialized serializedWrite/Read functions!
 */
#define ZTOP_IO_SERIALIZE_SPECIALIZEVECTORS(x) \
		template<class T> \
		inline void serializedWrite(const std::vector<x>&in, T&saveFile){ \
	size_t len=in.size(); \
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len)); \
	for(size_t i=0;i<len;i++) \
	serializedWrite(in.at(i),saveFile); \
} \
template<class T> \
inline void serializedRead(std::vector<x>&in, T&saveFile){ \
	size_t len=0; \
	saveFile.read(reinterpret_cast< char *>(&len), sizeof(len)); \
	in.resize(len,x()); \
	for(size_t i=0;i<len;i++) \
	serializedRead(in.at(i),saveFile); \
} \


#endif /* SERIALIZE_H_ */
