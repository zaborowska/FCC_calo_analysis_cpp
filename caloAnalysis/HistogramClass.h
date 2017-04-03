#ifndef __HISTOGRAMCLASS_H__
#define __HISTOGRAMCLASS_H__

#include "TObject.h"
#include "TH1F.h"

class HistogramClass {

 public:
  HistogramClass(double ENE);
  ~HistogramClass();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* h_hitEnergy;
  TH1F* h_cellEnergy;
  TH1F* h_ptGen;
  TH1F* h_phiGen;
  TH1F* h_etaGen;
  TH1F* h_phiHit;
  TH1F* h_etaHit;
  TH1F* h_deltaPhi;
  TH1F* h_deltaEta;
  std::vector<TH1F*> histVector;

 private:
  double ENERGY;
  const double etaMax = 4.5;

};

#endif
