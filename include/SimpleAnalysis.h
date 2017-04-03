#ifndef SIMPLEANALYSIS_H
#define SIMPLEANALYSIS_H

#include "BaseAnalysis.h"
#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class SimpleAnalysis: public BaseAnalysis {

 public:
  SimpleAnalysis(double aEnergy, double aSf);
  ~SimpleAnalysis();

  TH1F* hHitEnergy;
  TH1F* hCellEnergy;
  TH1F* hGenPt;

 private:
  virtual void processEvent(podio::EventStore& store, int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  void Initialize_histos();
  double m_energy;
  double m_sf;
};

#endif /* SIMPLEANALYSIS_H */
