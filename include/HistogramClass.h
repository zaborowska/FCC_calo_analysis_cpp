#ifndef __HISTOGRAMCLASS_H__
#define __HISTOGRAMCLASS_H__

#include "BaseAnalysis.h"
#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class HistogramClass: public BaseAnalysis {

 public:
  HistogramClass(double aEnergy, double aSf);
  ~HistogramClass();

  virtual void processEvent(podio::EventStore& store, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;

  void Initialize_histos();

  TH1F* hHitEnergy;
  TH1F* hCellEnergy;
  TH1F* hGenPt;

 private:
  double m_energy;
  double m_sf;

};

#endif
