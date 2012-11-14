#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"


void containerUF_test(){
  using namespace top;
  using namespace std;

  vector<float> binning;
  binning << 1 << 3 << 4 << 7;

  container1D nominal(binning, "testcontainer", "xvalues", "yvalues");
  nominal.fill(2, 5);
  nominal.fill(3.4,2.5);
  nominal.fill(6, 2);
  nominal.fill(9, 2);

  container1D deviation1(binning); // "up"
  deviation1.fill(2,5.3);
  deviation1.fill(3.4,2.7);
  deviation1.fill(6, 2.4);

  container1D deviation2(binning); // "up"
  deviation2.fill(2,4.8);
  deviation2.fill(3.4,2);
  deviation2.fill(6, 1);

  container1D deviation3(binning); // "up"
  deviation3.fill(2,4.5);
  deviation3.fill(3.4,1);
  deviation3.fill(6, 0.5);


  container1D deviation4(binning); // "up"
  deviation4.fill(2,7.5);
  deviation4.fill(3.8,1);
  deviation4.fill(6, 2.5);






  // nominal.setAllErrorsZero();


  nominal.addErrorContainer( "var1_up" , deviation1);
  nominal.addErrorContainer("var1_down" , deviation2);
  nominal.addErrorContainer( "var2_up" , deviation3);
  nominal.addErrorContainer("var2_down" , deviation4);

  nominal.transformStatToSyst("stat");

  // nominal.getTGraph("",false)->Draw("AP");
  
  //weird behaviour... check! recompiling might be the solution
  
  container1DUF bla(nominal);

  //  bla.getTGraph("",false)->Draw("AP");

  int bin=bla.getBinNo(2.401);

  container1DUF sysbreaks=bla.breakDownRelSystematicsInBin(bin);
  sysbreaks.getTGraph("",false,true)->Draw("AP");

  sysbreaks.coutBinContents();
  cout << "\n\n" <<endl;

  bla.coutBinContents();

}
