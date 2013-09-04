/*
 * indexMap.C
 *
 *  Created on: Aug 30, 2013
 *      Author: kiesej
 */

#include "../interface/indexMap.h"
#include <iostream>
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

void IndexMap(){

	using namespace std;
	using namespace ztop;

	indexMap<TString> map1;

	map1.push_back("9 bla");
	map1.push_back("8 bla");
	map1.push_back("7 bla");
	map1.push_back("6 bla");
	map1.push_back("5 bla");
	map1.push_back("4 bla");
	map1 << "3 bla";

	map1 << "3 bla";
	map1 << "3 bla";

	map1 << "2 bla";

	indexMap<TString> map2;

	map2  << "2 bla" << "1 blu" << "2 blu" << "3 blu" << "2 bla" << "1 bla";

	for(size_t i=0 ;i < map1.size();i++)
		cout << map1.getData(i) << endl;

	cout << endl;
	for(int i=5; i< 10;i++)
		cout << map1.getIndex(toTString(i)+" bla") << endl;

	map1.eraseByData("6 bla");

	for(size_t i=0 ;i < map1.size();i++)
		cout <<i <<"   "<<  map1.getData(i) << endl;

	cout << endl;

	map2<<map1;

	for(size_t i=0 ;i < map2.size();i++)
			cout <<i <<"   "<<  map2.getData(i) << endl;

	map2.eraseByData("2 bla");
		cout << endl;
		for(size_t i=0 ;i < map2.size();i++)
			cout << i <<"   "<< map2.getData(i) << endl;
		map2.eraseByData("2 bla");
			cout << endl;
			for(size_t i=0 ;i < map2.size();i++)
				cout << i <<"   "<< map2.getData(i) << endl;
			map2.eraseByData("7 bla");
				cout << endl;
				for(size_t i=0 ;i < map2.size();i++)
					cout << i <<"   "<< map2.getData(i) << endl;
}
