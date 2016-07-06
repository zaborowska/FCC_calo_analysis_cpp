#ifndef __HISTOGRAMCLASS_H__
#define __HISTOGRAMCLASS_H__

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

class HistogramClass {

 public:
  HistogramClass(double sf, double ENE, TString particle);
  ~HistogramClass();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* h_hitEnergy;
  TH1F* h_cellEnergy;
  TH1F* h_ptGen;
  std::vector<TH1F*> histVector;

 private:
  double SF;
  TString PARTICLE;
  double ENERGY;

};

#endif
