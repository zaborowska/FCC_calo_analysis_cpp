#ifndef SINGLEPARTICLERECOMONITORS_H
#define SINGLEPARTICLERECOMONITORS_H

#include "BaseTwoFileAnalysis.h"
#include "Decoder.h"

#include "TObject.h"
#include "TH2F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class SingleParticleRecoMonitors: public BaseTwoFileAnalysis {

 public:
  SingleParticleRecoMonitors(const char* aCluserCollName, const char* aPosHitCollName, double aEnergy, double aEtaMax, double aPhiMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi);
  SingleParticleRecoMonitors(const std::string& aCluserCollName, const std::string& aPosHitCollName, double aEnergy, double aEtaMax, double aPhiMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi,
    const std::string& aCellCollName,  const std::string& aBitfield, const std::string& aLayerField, int aLayerFirst, int aLayerLast, double aFirstLayerSF,
    double aP0p0, double aP0p1, double aP1p0, double aP1p1);
  ~SingleParticleRecoMonitors();

  void Initialize_histos();

  // energy
  TH1F* hEnTotal;
  TH1F* hEn;
  TH2F* hEnFncPhi;
  // energy, correction for material in front
  TH1F* hEnCorr;
  TH1F* hEnFirstLayer;
  TH1F* hEnUpstream;
  // space
  TH1F* hEta;
  TH1F* hPhi;
  TH2F* hPhiFncPhi;
  TH2F* hEtaFncEta;
  // number of clusters
  TH1F* hNo;
  TH2F* hNoFncPhi;
  TH2F* hNoFncEta;
  TH1F* hEnMoreClu;
  TH1F* hEnDiffMoreClu;
  TH1F* hEtaMoreClu;
  TH1F* hEtaDiffMoreClu;
  TH1F* hPhiMoreClu;
  TH1F* hPhiDiffMoreClu;
  TH1F* hRDiffMoreClu;


 private:
  virtual void processEvent(podio::EventStore& store1, podio::EventStore& store2,int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  std::string m_clusterCollName;
  std::string m_particleCollName;
  std::string m_readout;
  double m_energy;
  double m_etaMax;
  double m_phiMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;
  bool m_ifCorrectForUpstream;
  std::string m_cellCollName;
  Decoder m_decoder;
  std::string m_layerFieldName;
  int m_firstLayerFirstId;
  int m_firstLayerLastId;
  double m_firstLayerSF;
  double m_P0p0;
  double m_P0p1;
  double m_P1p0;
  double m_P1p1;

};

#endif /* SINGLEPARTICLERECOMONITORS_H */
