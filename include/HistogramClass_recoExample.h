#ifndef __HISTOGRAMCLASS_RECOEXAMPLE_H__
#define __HISTOGRAMCLASS_RECOEXAMPLE_H__

#include "BaseAnalysis.h"

#include "TObject.h"
#include "TH2F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class HistogramClass_recoExample: public BaseAnalysis {

 public:
  HistogramClass_recoExample(const std::string& aCluserCollName, const std::string& aPosHitCollName, int aEventToAnalyse, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~HistogramClass_recoExample();

  void Initialize_histos();

  TH2F* hAllCellEnergy;
  TH2F* hClusterCellEnergy;
  TH2F* hClusterEnergy;

  std::vector<TH2F*> hVector;

 private:
  virtual void processEvent(podio::EventStore& store, int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  std::string m_clusterCollName;
  std::string m_posHitCollName;
  int m_eventToAnalyse;
  double m_energy;
  double m_etaMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;

};

#endif
