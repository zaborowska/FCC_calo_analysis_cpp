#include "HistogramClass.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

HistogramClass::HistogramClass(double ENE)
{
  ENERGY = ENE;
}

HistogramClass::~HistogramClass()
{
}


void HistogramClass::Initialize_histos()
{

  h_hitEnergy = new TH1F("h_hitEnergy","", 200, 0, ENERGY);
  histVector.push_back(h_hitEnergy);

  h_cellEnergy = new TH1F("h_cellenergy","", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  histVector.push_back(h_cellEnergy);

  h_ptGen = new TH1F("h_ptGen","", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  histVector.push_back(h_ptGen);

}


void HistogramClass::Reset_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }

} 

void HistogramClass::Delete_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    delete (*iterator);
  }
  
  histVector.clear();

}
