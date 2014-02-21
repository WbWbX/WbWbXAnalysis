/*
 * containerTest.C
 *
 *  Created on: Sep 2, 2013
 *      Author: kiesej
 */

#include "../interface/container.h"
#include "../../../TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TCanvas.h"

void containerTest(){
	using namespace ztop;
	using namespace std;

	container1D::debug =false;
	histoBins::showwarnings =true;


	vector<float> bins;
	bins << 0 << 3 << 5 << 6 << 10;

	container1D cont1(bins);

	for(int i=0;i<20000;i++){
		cont1.fill(1);
		cont1.fill(4);
		cont1.fill(4);
		cont1.fill(5.3,1);
		cont1.fill(7,3);
	}

	cout << "copy container" << endl;

	container1D cont2=cont1;

	cout << "add rel error" << endl;

	cont2.addGlobalRelErrorDown("bla",0.1);
	cont2.addGlobalRelErrorUp("bla",0.);

	cont2.addGlobalRelErrorDown("blu",0.3);
	cont2.addGlobalRelErrorUp("blu",0.);

	cont1*=1.1;
	cont2.addErrorContainer("blub_up",cont1);
	cont2.addErrorContainer("blub_down",cont1*(1/1.1));

	TCanvas * c1=new TCanvas();

	cout << "draw only stat" << endl;
	cont2.getTH1D("",false,true)->Draw("e1");
	cout << "draw with sys" << endl;

	cont2.getTGraph("",false,false)->Draw("same,P");

	TCanvas * c2=new TCanvas();
	c2->cd();

	cont1*=(1/1.1);
	cont1*=10.;

	container1D::debug=true;
	container1D ctemp=cont2;
	//just for testing
	for(size_t bin=0;bin<cont2.getBins().size();bin++)
	    cont2.setBinStat(bin,0);
    cont2*=0.1;
	cont2.coutFullContent();
	cont2.removeAllSystematics();

    cont2.addGlobalRelErrorDown("oldbla",0.1);
    cont2.addGlobalRelErrorUp("oldbla",0.);
    cout << "oldsys" << endl;
    cont2.coutFullContent();
	cont2.addRelSystematicsFrom(ctemp);
	cout << "after addRelSystematicsFrom" << endl;
	cont2.coutFullContent();



	cont2.getTH1D("",false,true)->Draw("e1");
	cont2.getTGraph("",false,false)->Draw("same,P");

}
int main(){
    containerTest();
    return 0;
}

