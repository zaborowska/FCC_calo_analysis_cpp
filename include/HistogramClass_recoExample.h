#ifndef __HISTOGRAMCLASS_RECOEXAMPLE_H__
#define __HISTOGRAMCLASS_RECOEXAMPLE_H__

#include "TObject.h"
#include "TH2F.h"

class HistogramClass_recoExample {

 public:
  HistogramClass_recoExample(double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~HistogramClass_recoExample();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH2F* hAllCellEnergy;
  TH2F* hClusterCellEnergy;
  TH2F* hClusterEnergy;

  std::vector<TH2F*> hVector;

 private:
  double m_energy;
  double m_etaMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;

};

#endif
