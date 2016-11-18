#ifndef CALOANALYSIS_RECOMONITOR_H
#define CALOANALYSIS_RECOMONITOR_H

#include "HistogramClass_recoMonitor.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_recoMonitor {

 public:
  CaloAnalysis_recoMonitor(const std::string& aCluserCollName, const std::string& aParticleCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~CaloAnalysis_recoMonitor();
  /// Open the file in the reader and loop through the events
  void loop(const std::string& aFilenameSim, const std::string& aFilenameRec, bool aVerbose);
  /// Event analysis
  void processEvent(podio::EventStore& aStoreSim,podio::ROOTReader& aReaderSim,
    podio::EventStore& aStoreRec, podio::ROOTReader& aReaderRec, bool aVerbose);
  /// Retrieve filled histograms
  inline HistogramClass_recoMonitor& histograms() {return m_histograms;};

 private:
  std::string m_clusterCollName;
  std::string m_particleCollName;
  double m_energy;
  HistogramClass_recoMonitor m_histograms;
};

#endif /* CALOANALYSIS_RECOMONITOR_H */
