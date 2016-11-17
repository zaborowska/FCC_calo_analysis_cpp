#include "HistogramClass.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

HistogramClass::HistogramClass(double aEnergy) {
  m_energy = aEnergy;
}

HistogramClass::~HistogramClass(){}


void HistogramClass::Initialize_histos() {
  hHitEnergy = new TH1F("hHitEnergy","", 200, 0, m_energy);
  hVector.push_back(hHitEnergy);

  hCellEnergy = new TH1F("hCellenergy","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  hVector.push_back(hCellEnergy);

  hGenPt = new TH1F("hGenPt","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  hVector.push_back(hGenPt);
}


void HistogramClass::Reset_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }
}

void HistogramClass::Delete_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    delete (*iterator);
  }
  hVector.clear();

}
