#ifndef __HISTOGRAMCLASS_PROFILES_H__
#define __HISTOGRAMCLASS_PROFILES_H__

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

class HistogramClass_profiles {

 public:
  HistogramClass_profiles(double sf, double ENE, TString particle);
  ~HistogramClass_profiles();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();
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

  // vector of all TH1F histograms (for easier manipulation)
  std::vector<TH1F*> histVector;

 private:
  double SF;
  TString PARTICLE;
  double ENERGY;

};

#endif
