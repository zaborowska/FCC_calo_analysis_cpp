#ifndef __CALOANALYSIS_SIMPLE_H__
#define __CALOANALYSIS_SIMPLE_H__

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_simple {

 public:
  CaloAnalysis_simple(const double sf, const double ENE, const std::string particle);
  ~CaloAnalysis_simple();

  void loop(const std::string filename);
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);

  TH1F* h_hitEnergy;
  TH1F* h_cellEnergy;
  TH1F* h_ptGen;

 private:
  const double GeV=1000;
  double SF;
  TString PARTICLE;
  double ENERGY;
  double SumE_hit_ecal;

};

#endif
