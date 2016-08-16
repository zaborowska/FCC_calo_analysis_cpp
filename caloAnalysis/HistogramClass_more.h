#ifndef __HISTOGRAMCLASS_MORE_H__
#define __HISTOGRAMCLASS_MORE_H__

#include "TObject.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TMath.h"

class HistogramClass_more {

 public:
  HistogramClass_more(const double sf, const double ENE, const TString particle);
  ~HistogramClass_more();

  void Initialize_histos();
  void Delete_histos();
  void Reset_histos();

  TH1F* h_hitEnergy;
  TH1F* h_totalHitEnergy;
  TH1F* h_totalCellEnergy;
  TH1F* h_phiDiff;

  TH1F* h_longProfile;
  TH1F* h_radialProfile;

  TGraphErrors* g_longProfile;
  TGraphErrors* g_radialProfile;

  TH1F* h_ptGen;
  TH1F* h_pGen;
  TH1F* h_EGen;
  TH1F* h_phiGen;
  TH1F* h_etaGen;

  static const unsigned int NLAYERS_MAX=140; //Coresponds to layer_thickness of 5 mm                                                              

  TH1F* h_layer_e[NLAYERS_MAX];
  TH1F* h_layer_rho[NLAYERS_MAX];
  TH1F* h_layer_phi[NLAYERS_MAX];

  TH1F* h_slicePhi_e[NLAYERS_MAX];
  TH1F* h_slicePhi_phi[NLAYERS_MAX];

  std::vector<TH1F*> histVector;

 private:
  double SF;
  TString PARTICLE;
  double ENERGY;
  const int nbins=100;
  const int nbins_phi=64;
  const double rmin_calo=2600.;
  const double rmax_calo=3500.;
  const double phi_min=-TMath::Pi();
  const double phi_max=TMath::Pi();
  const double eta_min=-8.;
  const double eta_max=8.;
  unsigned int n_layers;
  unsigned int n_slices_phi;
 
};

#endif
