#include "PileupNoise.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

#include "TVector3.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <cmath>

PileupNoise::PileupNoise(const std::string& aCellCollName, 
			   double aEnergy, double aEtaMax, double aPhiMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi, const std::string& aBitfield, const std::string& aLayerField, const int aNLayers):
  m_etaMax(aEtaMax), m_phiMax(aPhiMax),
  m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi),
  m_cellCollName(aCellCollName),  m_decoder(aBitfield), m_layerFieldName(aLayerField), m_nLayers(aNLayers)
 {
  Initialize_histos();
}


PileupNoise::~PileupNoise(){}


void PileupNoise::Initialize_histos() {

  hEnCell = new TH1F("cellEnergy", "Cell Energy at EM scale",160,-1.,15.);
  hEnCellTest = new TH1F("cellEnergyTest", "Cell Energy at EM scale - eta<0.1, first layer",160,-1.,15.);
  TH2F *histo[m_nLayers];
  for (int i = 0; i<m_nLayers; i++) {
     histo[i] = new TH2F(("EnergyVsAbsEta_"+std::to_string(i)).c_str(), ("Cell Energy vs abs(eta) layer "+std::to_string(i)).c_str(),20,0.,2.0,5000,-1.,15.);
     hEnFcnAbsEta.push_back(histo[i]);
     m_histograms.push_back(histo[i]);
  }

  m_histograms.push_back(hEnCell);
  m_histograms.push_back(hEnCellTest);
 
}

void PileupNoise::processEvent(podio::EventStore& aStore, int aEventId, bool aVerbose) {
   // Get the collections
   // get cells to calculate energy deposited in first layer
   const fcc::CaloHitCollection* cells(nullptr);
   bool testCells = aStore.get(m_cellCollName, cells);
   if (testCells) {
     if (aVerbose) {
       std::cout << "Number of cells: " << cells->size() << std::endl;
     }
     for (const auto icell = cells->begin(); icell != cells->end(); ++icell) {
       int layerId = m_decoder.value(m_layerFieldName,icell->core().cellId);
       int etaId = m_decoder.value("eta",icell->core().cellId);
       double etaPos = m_decoder.segmentationPosition(etaId, m_dEta, -fabs(m_etaMax));
       //std::cout << layerId << " " << etaPos << std::endl;
       if ((layerId==0)&&(fabs(etaPos)<=0.1)) {
         hEnCellTest->Fill(icell->core().energy);
       }
       hEnFcnAbsEta[layerId]->Fill(fabs(etaPos), icell->core().energy);
       hEnCell->Fill(icell->core().energy);
     }  
   } else {
     std::cout << "No Cell Collection in the event." << std::endl;
     return;
   }
}

void PileupNoise::finishLoop(int aNumEvents, bool aVerbose) {
  /*
  hEnCell->Scale(1./aNumEvents);
  for (int i = 0; i<m_nLayers; i++) {
    hEnFcnAbsEta[i]->Scale(1./aNumEvents);
  }
  */
}
