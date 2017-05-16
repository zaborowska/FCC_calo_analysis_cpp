#ifndef PILEUPNOISE_H
#define PILEUPNOISE_H

#include "BaseAnalysis.h"
#include "Decoder.h"

#include "TObject.h"
#include "TH2F.h"

namespace podio {
  class EventStore;
  class ROOTReader;
}

class PileupNoise: public BaseAnalysis {

 public:
  PileupNoise(const std::string& aCellCollName, 
	      double aEnergy, double aEtaMax, double aPhiMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi,
	      const std::string& aBitfield, const std::string& aLayerField, const int aNLayers);
  ~PileupNoise();

  void Initialize_histos();

  // energy in cells at EM scale
  TH1F* hEnCell;
  // energy in cells within some region                                                                                                                 
  TH1F* hEnCellTest;
  // energy vs |eta|
  std::vector<TH2F*> hEnFcnAbsEta;

 private:
  virtual void processEvent(podio::EventStore& store,int aEventId, bool verbose) final;
  virtual void finishLoop(int aNumEvents, bool aVerbose) final;
  std::string m_readout;
  double m_energy;
  double m_etaMax;
  double m_phiMax;
  int m_noEta;
  int m_noPhi;
  double m_dEta;
  double m_dPhi;
  std::string m_cellCollName;
  Decoder m_decoder;
  std::string m_layerFieldName;
  const int m_nLayers;
  
};

#endif /* PILEUPNOISE_H */
