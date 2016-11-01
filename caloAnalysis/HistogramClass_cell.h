#ifndef __HISTOGRAMCLASS_CELL_H__
#define __HISTOGRAMCLASS_CELL_H__

#include "TObject.h"
#include "TH1F.h"

class HistogramClass_cell {

 public:
  HistogramClass_cell(double ENE);
  ~HistogramClass_cell();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* h_cellEnergy;
  TH1F* h_cellId;
  
  TH1F* h_ene_eta;
  TH1F* h_ene_phi;
  TH1F* h_ene_r;

  TH1F* h_cellEnergy_check;
  TH1F* h_ene_eta_check;
  TH1F* h_ene_phi_check;
  TH1F* h_ene_r_check;
  
  std::vector<TH1F*> histVector;

 private:
  double ENERGY;

};

#endif
