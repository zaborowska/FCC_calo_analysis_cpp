#include "HistogramClass_cell.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

HistogramClass_cell::HistogramClass_cell(double sf, double ENE, TString particle)
{
  PARTICLE=particle;
  ENERGY = ENE;
}

HistogramClass_cell::~HistogramClass_cell()
{
}


void HistogramClass_cell::Initialize_histos()
{

  if (PARTICLE=="e") {
    h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  }
  else {
    if (PARTICLE=="mu") h_cellEnergy = new TH1F("h_cellEnergy","h_cellEnergy", 1000, 0, ENERGY-0.8*ENERGY);
    else {
      std::cout << "WARNING!!! Histogram ranges for " << PARTICLE << " particle not defined!!!" <<std::endl;
      h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", 100, 0, ENERGY+0.2*ENERGY);
    }
  }
  histVector.push_back(h_cellEnergy);

  //h_cellId = new TH1F("h_cellId","h_cellId", 1000, 0,5000e6);
  h_cellId = new TH1F("h_cellId","h_cellId", 1000, 3.35e7,3.36e7); 

}


void HistogramClass_cell::Reset_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }

} 

void HistogramClass_cell::Delete_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    delete (*iterator);
  }
  
  histVector.clear();

}
