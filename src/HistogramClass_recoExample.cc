#include "HistogramClass_recoExample.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <cmath>

HistogramClass_recoExample::HistogramClass_recoExample(double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_energy(aEnergy), m_etaMax(aEtaMax), m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi) {}

HistogramClass_recoExample::~HistogramClass_recoExample(){}


void HistogramClass_recoExample::Initialize_histos() {
  hAllCellEnergy = new TH2F("all cells","calo towers (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  hClusterEnergy = new TH2F("cluster","cluster seeds (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  hClusterCellEnergy = new TH2F("cells associated to clusters","cells in reconstructed cluster (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  hVector.push_back(hAllCellEnergy);
  hVector.push_back(hClusterCellEnergy);
  hVector.push_back(hClusterEnergy);
}


void HistogramClass_recoExample::Reset_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }
}

void HistogramClass_recoExample::Delete_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    delete (*iterator);
  }
  hVector.clear();

}
