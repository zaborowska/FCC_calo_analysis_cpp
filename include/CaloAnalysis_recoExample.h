#ifndef CALOANALYSIS_RECOEXAMPLE_H
#define CALOANALYSIS_RECOEXAMPLE_H

#include "HistogramClass_recoExample.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_recoExample {

 public:
  CaloAnalysis_recoExample(const std::string& aCluserCollName, const std::string& aPosHitCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  ~CaloAnalysis_recoExample();
  /// Open the file in the reader and loop through the events
  void analyseEvent(const std::string& aFilename, int aNumEvent);
  /// Event analysis
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);
  /// Retrieve filled histograms
  inline HistogramClass_recoExample& histograms() {return m_histograms;};

 private:
  std::string m_clusterCollName;
  std::string m_posHitCollName;
  double m_energy;
  HistogramClass_recoExample m_histograms;
};

#endif /* CALOANALYSIS_RECOEXAMPLE_H */
