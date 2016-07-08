#ifndef __HISTOGRAMCLASS_MORE_H__
#define __HISTOGRAMCLASS_MORE_H__

#include "TObject.h"
#include "TH1F.h"
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
  TH1F* h_cellEnergy;
  TH1F* h_ptGen;
  TH1F* h_pGen;
  TH1F* h_EGen;
  TH1F* h_phiGen;
  TH1F* h_etaGen;
  std::vector<TH1F*> histVector;

 private:
  double SF;
  TString PARTICLE;
  double ENERGY;
  const int nbins=100;
  const int nbins_phi=64;
  const double phi_min=-TMath::Pi();
  const double phi_max=TMath::Pi();
  const double eta_min=-8.;
  const double eta_max=8.;

};

#endif
