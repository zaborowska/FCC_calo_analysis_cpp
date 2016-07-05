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
  TH1F* h_longProfile;
  TH1F* h_ptGen;

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
  TH1F* h_layers[NLAYERS_MAX];

  unsigned createMask(unsigned a, unsigned b);
  void truncated_mean(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose);
  void gaussian_fit(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose);

};

#endif
