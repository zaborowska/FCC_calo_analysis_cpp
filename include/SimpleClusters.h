#ifndef SIMPLECLUSTERS_H
#define SIMPLECLUSTERS_H

#include "BaseAnalysis.h"
#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class SimpleClusters: public BaseAnalysis {

 public:
  SimpleClusters(double aEnergy);
  ~SimpleClusters();

  TH1F* hClustersEnergy;
  TH1F* hFirstClusterEnergy;

 private:
  virtual void processEvent(podio::EventStore& store, int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  void Initialize_histos();
  double m_energy;
};

#endif /* SIMPLEANALYSIS_H */
