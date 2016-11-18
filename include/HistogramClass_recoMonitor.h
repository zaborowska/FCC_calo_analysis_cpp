#ifndef HISTOGRAMCLASS_RECOMONITOR_H
#define HISTOGRAMCLASS_RECOMONITOR_H

#include "TObject.h"
#include "TH2F.h"

class HistogramClass_recoMonitor {

 public:
  HistogramClass_recoMonitor(double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~HistogramClass_recoMonitor();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  // energy
  TH1F* hEn;
  TH2F* hEnFncPhi;
  // space
  TH1F* hEta;
  TH1F* hPhi;
  TH2F* hPhiFncPhi;
  TH2F* hEtaFncEta;
  // number of clusters
  TH1F* hNo;
  TH2F* hNoFncPhi;
  TH1F* hEnDiffMoreClu;
  TH1F* hEtaDiffMoreClu;
  TH1F* hPhiDiffMoreClu;
  TH1F* hRDiffMoreClu;

  std::vector<TH1*> hVector;

 private:
  double m_energy;
  double m_etaMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;

};

#endif
