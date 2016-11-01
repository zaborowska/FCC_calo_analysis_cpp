#include "HistogramClass_profiles.h"

// STL
#include <vector>
#include <iostream>

HistogramClass_profiles::HistogramClass_profiles(double ENE)
{
  ENERGY = ENE;
}

HistogramClass_profiles::~HistogramClass_profiles()
{
}


void HistogramClass_profiles::Initialize_histos()
{

  h_hitEnergy = new TH1F("h_hitEnergy","", 200, 0, ENERGY);
  histVector.push_back(h_hitEnergy);

  h_cellEnergy = new TH1F("h_cellenergy","", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  histVector.push_back(h_cellEnergy);

  h_radialProfile = new TH1F("h_radialProfile","", 100, 0, 10);
  histVector.push_back(h_radialProfile);

  h_longProfile = new TH1F("h_longProfile","", 116, 0, 45);
  histVector.push_back(h_longProfile);

  h_radialProfile_particle = new TH1F("h_radialProfile_particle","", 100, 0, 10);
  histVector.push_back(h_radialProfile_particle);

  h_longProfile_particle = new TH1F("h_longProfile_particle","", 116, 0, 45);
  histVector.push_back(h_longProfile_particle);

  h_ptGen = new TH1F("h_ptGen","", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  histVector.push_back(h_ptGen);

  h_pdgGen = new TH1F("h_pdgGen","", 200,-100,100);
  histVector.push_back(h_pdgGen);
}


void HistogramClass_profiles::Reset_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }

} 

void HistogramClass_profiles::Delete_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    delete (*iterator);
  }
  
  histVector.clear();

}
