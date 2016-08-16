#ifndef __CALOANALYSIS_CELL_H__
#define __CALOANALYSIS_CELL_H__

#include "HistogramClass_cell.h"

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_cell {

 public:
  CaloAnalysis_cell(const double sf, const double ENE, const TString particle);
  ~CaloAnalysis_cell();

  void loop(const std::string filename);  //Open the file in the reader and loop through the events
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);

  HistogramClass_cell* histClass; 

 private:
  const double GeV=1000;
  double SF;               // 1/sampling_fraction
  TString PARTICLE;        // Particle type: e/mu
  double ENERGY;           // Beam energy
  double SumE_cell;    // Total hit energy per event

};

#endif
