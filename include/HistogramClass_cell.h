#ifndef __HISTOGRAMCLASS_CELL_H__
#define __HISTOGRAMCLASS_CELL_H__

#include "BaseAnalysis.h"
#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class HistogramClass_cell: public BaseAnalysis  {

 public:
  HistogramClass_cell(double aEnergy, double aSf);
  ~HistogramClass_cell();

  virtual void processEvent(podio::EventStore& store, int aEventId, bool aVerbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;

  void Initialize_histos();

  TH1F* h_cellEnergy;
  TH1F* h_cellId;

  TH1F* h_ene_eta;
  TH1F* h_ene_phi;
  TH1F* h_ene_r;

  TH1F* h_cellEnergy_check;
  TH1F* h_ene_eta_check;
  TH1F* h_ene_phi_check;
  TH1F* h_ene_r_check;

 private:
  double m_energy;
  double m_sf;
  double SumE_cell;    // Total hit energy per event
};

#endif
