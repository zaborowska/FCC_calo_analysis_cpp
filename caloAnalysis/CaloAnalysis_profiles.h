#ifndef __CALOANALYSIS_PROFILES_H__
#define __CALOANALYSIS_PROFILES_H__

#include "HistogramClass_profiles.h"

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_profiles {

 public:
  CaloAnalysis_profiles(const double sf, const double ENE, const TString particle);
  ~CaloAnalysis_profiles();

  void loop(const std::string filename);  //Open the file in the reader and loop through the events
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader, unsigned i);

  HistogramClass_profiles* histClass; 

 private:
  const double GeV=1000;
  double SF;               // 1/sampling_fraction
  TString PARTICLE;        // Particle type: e/mu
  double ENERGY;           // Beam energy
  double SumE_hit;    // Total hit energy per event
  const double RcaloMin = 2700.;
  const double RcaloThickness = 800.;
  const double layerThickness = 6.;
  const double EtaMax = 10.0;
  const double X0 = 15.586;   //average X0 for 4 mm Lar + 2 mm Pb
};

#endif
