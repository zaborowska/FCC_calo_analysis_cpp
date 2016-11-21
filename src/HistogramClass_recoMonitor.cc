#include "HistogramClass_recoMonitor.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <cmath>

HistogramClass_recoMonitor::HistogramClass_recoMonitor(double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_energy(aEnergy), m_etaMax(aEtaMax), m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi) {}

HistogramClass_recoMonitor::~HistogramClass_recoMonitor(){}


void HistogramClass_recoMonitor::Initialize_histos() {

  hEn = new TH1F("energy",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    101,0.8*m_energy,1.2*m_energy);
  hEnFncPhi = new TH2F("energy_phi",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;energy (GeV)").c_str(),
    m_noPhi,-M_PI,M_PI,
    11,0.8*m_energy,1.2*m_energy);
  hEta = new TH1F("eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;fraction of events").c_str()
    ,101,-10*m_dEta,10*m_dEta);
  hPhi = new TH1F("phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;fraction of events").c_str(),
    101,-10*m_dPhi,10*m_dPhi);
  hPhiFncPhi = new TH2F("phi_phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;#Delta#varphi").c_str(),
    m_noPhi,-M_PI,M_PI,
    101,-10*m_dPhi,10*m_dPhi);
  hEtaFncEta = new TH2F("eta_eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#eta;#Delta#eta").c_str(),
    m_noEta,-1.8,1.8,
    101,-10*m_dEta,10*m_dEta);
  hNo = new TH1F("clusters",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);fraction of events;number of clusters per event").c_str(),
    5,-0.5,4.5);
  hNoFncPhi = new TH2F("clusters_phi",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;number of clusters per event").c_str(),
    m_noPhi,-M_PI,M_PI,
    5,-0.5,4.5);
  hEnDiffMoreClu = new TH1F("energy_diff",
    ("#DeltaE/E for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta E / E;number of events").c_str(),
    101,0,1);
  hEtaDiffMoreClu = new TH1F("eta_diff",
    ("#Delta#eta for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;number of events").c_str(),
    101,-10*m_dEta,10*m_dEta);
  hPhiDiffMoreClu = new TH1F("phi_diff",
    ("#Delta#varphi for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;number of events").c_str(),
    101,-2.1*M_PI,2.1*M_PI);
  hRDiffMoreClu = new TH1F("R_diff",
    ("#Delta R for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta R;number of events").c_str(),
    101,-50*m_dPhi,50*m_dPhi);

  hVector.push_back(hEn);
  hVector.push_back(hEnFncPhi);
  hVector.push_back(hEta);
  hVector.push_back(hPhi);
  hVector.push_back(hEtaFncEta);
  hVector.push_back(hPhiFncPhi);
  hVector.push_back(hNo);
  hVector.push_back(hNoFncPhi);
  hVector.push_back(hEnDiffMoreClu);
  hVector.push_back(hEtaDiffMoreClu);
  hVector.push_back(hPhiDiffMoreClu);
  hVector.push_back(hRDiffMoreClu);
}


void HistogramClass_recoMonitor::Reset_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }
}

void HistogramClass_recoMonitor::Delete_histos() {
  for (auto iterator=hVector.begin(); iterator<hVector.end(); iterator++) {
    delete (*iterator);
  }
  hVector.clear();

}
