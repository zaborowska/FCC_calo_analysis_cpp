
#ifndef __CALOANALYSIS_MYANALYSIS_H__
#define __CALOANALYSIS_MYANALYSIS_H__

#include "TObject.h"
#include "TH1F.h"
#include "TString.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class CaloAnalysis {

 public:
  CaloAnalysis(const double sf, const double ENE, const std::string particle);
  ~CaloAnalysis();

  void loop(const std::string filename);
  void processEvent(podio::EventStore& store, bool verbose,
		    podio::ROOTReader& reader);

  TH1F* hitenergy;
  TH1F* cellenergy;
  TH1F* longprofile;
  TH1F* hitphi;
  TH1F* deltaphi;
  TH1F* phi_out;
  TH1F* phi_outliers;
  TH1F* phi_max;
  TH1F* y_max;
  TH1F* x_max;
  TH1F* r_max;
  TH1F* x_outliers;
  TH1F* y_outliers;
  TH1F* z_outliers;
  TH1F* r_outliers;
  TH1F* phi_prim;
  TH1F* p_prim;
  TH1F* pt_prim;
  TH1F* phi_prim_out;
  TH1F* diff_phimaxhit_phi_prim;
  TH1F* phi_brem;
  TH1F* p_brem;
  TH1F* pt_brem;
  TH1F* r_brem;
  TH1F* phi_brem_out;
  TH1F* p_brem_out;
  TH1F* pt_brem_out;
  TH1F* r_brem_out;
  TH1F* h_ene_diff;

 private:
  const double GeV = 1000;
  double SF = 1.0;
  TString PARTICLE = "e";
  double ENERGY = 100.0;
  static const int NLAYERS_MAX = 500;
  const double rmin = 2700.;
  const double rmax = 3400.;
  // const double dr = 4.24+2.44;
  double SumE_layer[NLAYERS_MAX];
  double SumE_hit_ecal;
  unsigned int nlayers;
  double dr;
  TH1F* phi_e;
  TH1F* x_e;
  TH1F* y_e;
  TH1F* z_e;
  TH1F* r_e;
  TH1F* h_layers[NLAYERS_MAX];

  unsigned createMask(unsigned a, unsigned b);
  void truncated_mean(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose);
  void gaussian_fit(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose);

};

#endif
