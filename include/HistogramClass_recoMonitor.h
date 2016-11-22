#ifndef HISTOGRAMCLASS_RECOMONITOR_H
#define HISTOGRAMCLASS_RECOMONITOR_H

#include "TwoFileAnalysis.h"

#include "TObject.h"
#include "TH2F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class HistogramClass_recoMonitor: public TwoFileAnalysis {

 public:
  HistogramClass_recoMonitor(const std::string& aCluserCollName, const std::string& aPosHitCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~HistogramClass_recoMonitor();

  void Initialize_histos();

  // energy
  TH1F* hEn;
  TH2F* hEnFncPhi;
  // space
  TH1F* hEta;
  TH1F* hPhi;
  TH2F* hPhiFncPhi;
  TH2F* hEtaFncEta;
  // number of clusters
  TH1F* hNo;
  TH2F* hNoFncPhi;
  TH1F* hEnDiffMoreClu;
  TH1F* hEtaDiffMoreClu;
  TH1F* hPhiDiffMoreClu;
  TH1F* hRDiffMoreClu;


 private:
  virtual void processEvent(podio::EventStore& store1, podio::EventStore& store2,int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  std::string m_clusterCollName;
  std::string m_particleCollName;
  double m_energy;
  double m_etaMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;

};

#endif
