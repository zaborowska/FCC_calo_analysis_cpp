#ifndef __CALOANALYSIS_MORE_H__
#define __CALOANALYSIS_MORE_H__

#include "HistogramClass_more.h"

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis_more {

 public:
  //Arguments: 1/sampl. fraction, beam energy (GeV), particle type (e/mu), Number of events to be processed (<0 => all events)
  CaloAnalysis_more(const double sf, const double ENE, const TString particle, const int nevents);
  ~CaloAnalysis_more();

  void loop(const std::string filename);  //Open the file in the reader and loop through the events
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);

  HistogramClass_more* histClass; 

 private:
  const double GeV=1000;
  const double eHitMin=0.5; //Cut off for min. hit energy (MeV)
  const double rmin_calo = 2600;
  const double rmax_calo = 3500;
  double layer_thickness = 9.0; //Not necessary the simulated thickness, defines layers in the histograms
  double phi_slice_diff;
  static const unsigned int NLAYERS_MAX = 140; //Coresponds to layer_thickness of 5 mm
  unsigned int n_layers;
  unsigned int n_slices_phi;

  double SF;               // 1/sampling_fraction
  TString PARTICLE;        // Particle type: e/mu
  int NEVENTS;             // Number of events to be processed: if <0 => all events
  double ENERGY;           // Beam energy (GeV)
  double SumE_hit_ecal;    // Total hit energy in event
  double SumE_layer[NLAYERS_MAX]; // Total hit energy per layer in event
};

#endif
