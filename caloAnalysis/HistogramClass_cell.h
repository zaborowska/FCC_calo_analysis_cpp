#ifndef __HISTOGRAMCLASS_CELL_H__
#define __HISTOGRAMCLASS_CELL_H__

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

class HistogramClass_cell {

 public:
  HistogramClass_cell(double sf, double ENE, TString particle);
  ~HistogramClass_cell();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* h_cellEnergy;
  TH1F* h_cellId;
  std::vector<TH1F*> histVector;

 private:
  double SF;
  TString PARTICLE;
  double ENERGY;

};

#endif
