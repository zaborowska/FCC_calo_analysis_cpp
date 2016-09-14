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
    h_cellEnergy = new TH1F("h_cellEnergy","h_cellEnergy", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
    h_cellEnergy_check = new TH1F("h_cellEnergy_check","h_cellEnergy_check", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  }
  else {
    if (PARTICLE=="mu") {
      h_cellEnergy = new TH1F("h_cellEnergy","h_cellEnergy", 1000, 0, ENERGY-0.8*ENERGY);
      h_cellEnergy_check = new TH1F("h_cellEnergy_check","h_cellEnergy_check", 1000, 0, ENERGY-0.8*ENERGY);
    }
    else {
      std::cout << "WARNING!!! Histogram ranges for " << PARTICLE << " particle not defined!!!" <<std::endl;
      h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", 100, 0, ENERGY+0.2*ENERGY);
      h_cellEnergy_check = new TH1F("h_cellenergy_check","h_cellEnergy_check", 100, 0, ENERGY+0.2*ENERGY);
    }
  }
  histVector.push_back(h_cellEnergy);
  histVector.push_back(h_cellEnergy_check);

  h_cellId = new TH1F("h_cellId","h_cellId", 1000, 0,5000e6);
  histVector.push_back(h_cellId);

  h_ene_eta = new TH1F("h_ene_eta","h_ene_eta", 21, -2.0,2.0);
  histVector.push_back(h_ene_eta);

  h_ene_phi = new TH1F("h_ene_phi","h_ene_phi", 5, -3.1416 ,3.1416);
  histVector.push_back(h_ene_phi);

  h_ene_r = new TH1F("h_ene_r","h_ene_r", 116, 2700.,3400.);
  histVector.push_back(h_ene_r);

  h_ene_eta_check = new TH1F("h_ene_eta_check","h_ene_eta_check", 21, -2.0,2.0);
  histVector.push_back(h_ene_eta_check);

  h_ene_phi_check = new TH1F("h_ene_phi_check","h_ene_phi_check", 5, -3.1416 ,3.1416);
  histVector.push_back(h_ene_phi_check);

  h_ene_r_check = new TH1F("h_ene_r_check","h_ene_r_check", 116, 2700.,3400.);
  histVector.push_back(h_ene_r_check);

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
