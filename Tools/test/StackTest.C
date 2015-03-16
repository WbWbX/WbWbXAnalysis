#include "../interface/histoStackVector.h"
#include "../interface/miscUtils.h"




top::container1DStack StackTest(){

  using namespace top;
  using namespace std;

  container1DStack stack;
  vector<float> bins;
  for(int i=0;i<100;i++) bins << i;

  stack.setDataLegend("data");

  histo1D cont(bins,"contr","x","y");
  histo1D cont2=cont;

  for(float i=0;i<1000;i++) cont.fill(pow(i/100,2));
  stack.push_back(cont, "contr1", kRed, 1);
  cont2=cont2+cont;
  cont.clear();

  for(float i=0;i<1000;i++) cont.fill(pow(i/200,3));
  stack.push_back(cont, "contr2", kBlue, 1);
  cont2=cont2+cont;
  cont.clear();

  for(float i=0;i<1000;i++) cont.fill(pow(i/300,4));
  stack.push_back(cont, "contr3", kGreen, 1);
  cont2=cont2+cont;
  cont.clear();


  stack.push_back(cont2, "data", kBlack, 1);

  return stack;

  TCanvas * c = stack.makeTCanvas();
  c->Draw();


}

//void StackVectorTest(){}
