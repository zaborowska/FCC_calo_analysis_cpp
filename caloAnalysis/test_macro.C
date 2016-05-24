#include "CaloAnalysis.h"

#include "TCanvas.h"
#include "TSystem.h"

void test_macro()
{ 
  //Does not work in root6... 
  //seems we can do that from the command line but not
  //in an unnamed macro.
  //anyway, the python version is better, check it out.
  gSystem->Load("libcaloanalysis-myanalysis");

  CaloAnalysis ma;
  ma.loop("../output.root");
  TCanvas c1;
  c1.Divide(2,1);
  c1.cd(1);
  ma.hitenergy->Draw();
  
}
