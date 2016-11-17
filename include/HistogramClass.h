#ifndef __HISTOGRAMCLASS_H__
#define __HISTOGRAMCLASS_H__

#include "TObject.h"
#include "TH1F.h"

class HistogramClass {

 public:
  HistogramClass(double aEnergy);
  ~HistogramClass();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* hHitEnergy;
  TH1F* hCellEnergy;
  TH1F* hGenPt;
  std::vector<TH1F*> hVector;

 private:
  double m_energy;

};

#endif
