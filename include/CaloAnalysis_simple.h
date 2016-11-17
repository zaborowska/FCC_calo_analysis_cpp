#ifndef __CALOANALYSIS_SIMPLE_H__
#define __CALOANALYSIS_SIMPLE_H__

#include "HistogramClass.h"

#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_simple {

 public:
  CaloAnalysis_simple(const double aSf, const double aEnergy);
  ~CaloAnalysis_simple();

  void loop(const std::string& aFilename);  //Open the file in the reader and loop through the events
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);
  inline HistogramClass& histograms() {return m_histograms;};

 private:
  double m_sf;               // 1/sampling_fraction
  double m_energy;           // Beam energy
  HistogramClass m_histograms;

};

#endif
