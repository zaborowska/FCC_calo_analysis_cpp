#ifndef __HISTOGRAMCLASS_PROFILES_H__
#define __HISTOGRAMCLASS_PROFILES_H__

#include "BaseAnalysis.h"

#include "TObject.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class HistogramClass_profiles: public BaseAnalysis {

 public:
  HistogramClass_profiles(double aEnergy, double aSf);
  ~HistogramClass_profiles();

  virtual void processEvent(podio::EventStore& store, int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;

  /// Hit energy
  TH1F* h_hitEnergy;
  /// Calibrated hit energy (SF)
  TH1F* h_cellEnergy;
  /// Radial profile, direction of the shower from hits in the first layer
  TH1F* h_radialProfile;
  /// Long. profile, direction of the shower from hits in the first layer
  TH1F* h_longProfile;
  /// Radial profile, direction of the shower from the generated particle
  TH1F* h_radialProfile_particle;
  /// Long. profile, direction of the shower from the generated particle
  TH1F* h_longProfile_particle;
  /// pt of the generated particle
  TH1F* h_ptGen;
  /// PDG code of the generated particle
  TH1F* h_pdgGen;

 private:
  void Initialize_histos();
  double m_energy;
  double m_sf;
  double SumE_hit;    // Total hit energy per event
  const double RcaloMin = 1950.;
  const double RcaloThickness = 800.;
  const double layerThickness = 6.;
  const double EtaMax = 10.0;
  const double X0 = 15.586; //average X0 for 4 mm Lar + 2 mm Pb

};

#endif
