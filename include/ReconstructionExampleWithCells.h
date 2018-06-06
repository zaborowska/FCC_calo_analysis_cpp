#ifndef RECONSTRUCTIONEXAMPLEWITHCELLS_H
#define RECONSTRUCTIONEXAMPLEWITHCELLS_H

#include "BaseAnalysis.h"

#include "TObject.h"
#include "TH2F.h"
#include "TH1F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class ReconstructionExampleWithCells: public BaseAnalysis {

 public:
  ReconstructionExampleWithCells(const std::string& aCluserCollName, const std::string& aPosHitCollName, int aEventToAnalyse, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, int aNoLayers, double aDEta, double aDPhi, std::string aEncoder, double aOffsetEta, double aOffsetPhi);
  ~ReconstructionExampleWithCells();

  void Initialize_histos();

  TH2F* hAllCellEnergy;
  TH2F* hClusterCellEnergy;
  TH2F* hClusterEnergy;
  std::vector<TH2F*> hLayerEnergy;

  std::vector<TH2F*> hVector;

  TH1F* hContainment;
  std::vector<TH1F*> hLayerContainment;
  std::vector<TH1F*> hLayerContainmentPercent;
  TH1F* hContainmentPercent;
  TH1F* hLayerContainment95;
  TH1F* hLayerContainment90;
  TH1F* hLayerContainment85;

 private:
  virtual void processEvent(podio::EventStore& store, int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  std::string m_clusterCollName;
  std::string m_posHitCollName;
  int m_eventToAnalyse;
  double m_energy;
  double m_etaMax;
  int m_noEta;
  int m_noPhi;
  int m_noLayer;
  double m_dEta;
  double m_dPhi;
  double m_dR;
  double m_noR;
  std::string m_encoder;
  double m_offsetEta;
  double m_offsetPhi;

};

#endif /* RECONSTRUCTIONEXAMPLEWITHCELLS_H */
