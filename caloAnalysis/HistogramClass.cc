#include "HistogramClass.h"

// ROOT
#include "TMath.h"

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

  h_phiGen = new TH1F("h_phiGen","", 64, -TMath::Pi(), TMath::Pi());
  histVector.push_back(h_phiGen);
  h_etaGen = new TH1F("h_etaGen","", 100, -etaMax, etaMax);
  histVector.push_back(h_etaGen);

  h_phiHit = new TH1F("h_phiHit","", 64, -TMath::Pi(), TMath::Pi());
  histVector.push_back(h_phiHit);
  h_etaHit = new TH1F("h_etaHit","", 100, -etaMax, etaMax);
  histVector.push_back(h_etaHit);

  h_deltaPhi = new TH1F("h_deltaPhi","", 100, -0.1, 0.1);
  histVector.push_back(h_deltaPhi);
  h_deltaEta = new TH1F("h_deltaEta","", 100, -0.1, 0.1);
  histVector.push_back(h_deltaEta);
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
