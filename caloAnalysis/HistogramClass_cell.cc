#include "HistogramClass_cell.h"

// STL
#include <vector>
#include <iostream>

HistogramClass_cell::HistogramClass_cell(double ENE)
{
  ENERGY = ENE;
}

HistogramClass_cell::~HistogramClass_cell()
{
}


void HistogramClass_cell::Initialize_histos()
{

  h_cellEnergy = new TH1F("h_cellEnergy","", 100, ENERGY-0.8*ENERGY, ENERGY+0.8*ENERGY);
  histVector.push_back(h_cellEnergy);
  h_cellEnergy_check = new TH1F("h_cellEnergy_check","", 100, ENERGY-0.8*ENERGY, ENERGY+0.8*ENERGY);
  histVector.push_back(h_cellEnergy_check);

  h_cellId = new TH1F("h_cellId","", 1000, 0,5000e6);
  histVector.push_back(h_cellId);

  h_ene_eta = new TH1F("h_ene_eta","", 200, -2.0,2.0);
  histVector.push_back(h_ene_eta);

  h_ene_phi = new TH1F("h_ene_phi","", 314, -3.1416 ,3.1416);
  histVector.push_back(h_ene_phi);

  h_ene_r = new TH1F("h_ene_r","", 110, 1500.,2600.);
  histVector.push_back(h_ene_r);

  h_ene_eta_check = new TH1F("h_ene_eta_check","", 200, -2.0,2.0);
  histVector.push_back(h_ene_eta_check);

  h_ene_phi_check = new TH1F("h_ene_phi_check","", 314, -3.1416 ,3.1416);
  histVector.push_back(h_ene_phi_check);

  h_ene_r_check = new TH1F("h_ene_r_check","", 110, 1500.,2600.);
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
