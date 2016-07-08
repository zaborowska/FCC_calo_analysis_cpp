#include "HistogramClass_more.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

HistogramClass_more::HistogramClass_more(const double sf, const double ENE, const TString particle)
{
  PARTICLE=particle;
  ENERGY = ENE;
}

HistogramClass_more::~HistogramClass_more()
{
}


void HistogramClass_more::Initialize_histos()
{

  h_hitEnergy = new TH1F("h_hitEnergy","h_hitEnergy", 2*nbins, 0, ENERGY);
  histVector.push_back(h_hitEnergy);

  const double ene_fraction = 0.2;

  if (PARTICLE=="e") {
    h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", nbins, ENERGY-ene_fraction*ENERGY, ENERGY+ene_fraction*ENERGY);
  }
  else {
    if (PARTICLE=="mu") h_cellEnergy = new TH1F("h_cellEnergy","h_cellEnergy", nbins, 0, ENERGY-0.8*ENERGY);
    else {
      std::cout << "WARNING!!! Histogram ranges for " << PARTICLE << " particle not defined!!!" <<std::endl;
      h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", nbins, 0, ENERGY+ene_fraction*ENERGY);
    }
  }
  histVector.push_back(h_cellEnergy);

  //Truth information
  h_ptGen = new TH1F("h_ptGen","h_ptGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_ptGen);
  h_pGen = new TH1F("h_pGen","h_pGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_pGen);
  h_EGen = new TH1F("h_EGen","h_EGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_EGen);
  h_phiGen = new TH1F("h_phiGen","h_phiGen", nbins, phi_min, phi_max);
  histVector.push_back(h_phiGen);
  h_etaGen = new TH1F("h_etaGen","h_etaGen", nbins, eta_min, eta_max);
  histVector.push_back(h_etaGen);

}


void HistogramClass_more::Reset_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }

} 

void HistogramClass_more::Delete_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    delete (*iterator);
  }
  
  histVector.clear();

}
