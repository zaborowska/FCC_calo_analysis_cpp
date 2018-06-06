#include "ReconstructionExampleWithCells.h"

#include "Decoder.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

#include "TVector3.h"

// STL
#include <vector>
#include <iostream>
#include <cmath>

ReconstructionExampleWithCells::ReconstructionExampleWithCells(const std::string& aCluserCollName, const std::string& aPosHitCollName, int aEventToAnalyse, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, int aNoLayers, double aDEta, double aDPhi, std::string aEncoder, double aOffsetEta, double aOffsetPhi):
  m_clusterCollName(aCluserCollName), m_posHitCollName(aPosHitCollName), m_eventToAnalyse(aEventToAnalyse), m_energy(aEnergy), m_etaMax(aEtaMax), m_noEta(aNoEta), m_noPhi(aNoPhi), m_noLayer(aNoLayers), m_dEta(aDEta), m_dPhi(aDPhi), m_encoder(aEncoder), m_offsetEta(aOffsetEta), m_offsetPhi(aOffsetPhi) {
  Initialize_histos();
}

ReconstructionExampleWithCells::~ReconstructionExampleWithCells(){}


void ReconstructionExampleWithCells::Initialize_histos() {
  hAllCellEnergy = new TH2F("all cells","calo towers (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  hClusterEnergy = new TH2F("cluster","cluster seeds (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  hClusterCellEnergy = new TH2F("cells associated to clusters","cells in reconstructed cluster (50 GeV e^{-})",m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi);
  m_histograms.push_back(hAllCellEnergy);
  m_histograms.push_back(hClusterCellEnergy);
  m_histograms.push_back(hClusterEnergy);
  for (int iLayer = 0; iLayer < m_noLayer; iLayer++) {
    hLayerEnergy.push_back(new TH2F(("layer"+std::to_string(iLayer)).c_str(),("layer "+std::to_string(iLayer)).c_str(),m_noEta,-m_etaMax,m_etaMax,m_noPhi,-M_PI-0.5*m_dPhi,M_PI+0.5*m_dPhi));
    m_histograms.push_back(hLayerEnergy[iLayer]);
  }
  // containment
  // calculate in what steps the containment should be measured
  m_dR = std::min(m_dEta, m_dPhi);
  m_noR = 40;
 std::cout<<" dR = " << m_dR << " num of R bins = " <<  m_noR << std::endl;
 hContainment = new TH1F("containment","total deposited energy vs distance from shower axis",m_noR, -0.5 * m_dR, (m_noR+0.5) * m_dR);
  m_histograms.push_back(hContainment);
  for (int iLayer = 0; iLayer < m_noLayer; iLayer++) {
    hLayerContainment.push_back( new TH1F(("containmentLayer"+std::to_string(iLayer)).c_str(),("total deposited energy vs distance from shower axis in layer "+std::to_string(iLayer)).c_str(), m_noR, -0.5* m_dR, (m_noR+0.5) * m_dR));
    m_histograms.push_back(hLayerContainment[iLayer]);
    hLayerContainmentPercent.push_back(new TH1F(("containmentPercentLayer"+std::to_string(iLayer)).c_str(),("containment vs distance from shower axis in layer "+std::to_string(iLayer)).c_str(), m_noR, -0.5 * m_dR,  (m_noR + 0.5) * m_dR));
    m_histograms.push_back(hLayerContainmentPercent[iLayer]);
  }
  hContainmentPercent = new TH1F("containmentPercent","containment vs distance from shower axis",m_noR, -0.5 * m_dR,  (m_noR + 0.5) * m_dR);
  m_histograms.push_back(hContainmentPercent);
  hLayerContainment95 = new TH1F("containmentLayer95","distance from shower axis ensuring 95% containment for different layers",m_noLayer, -0.5, m_noLayer+0.5);
  m_histograms.push_back(hLayerContainment95);
  hLayerContainment90 = new TH1F("containmentLayer90","distance from shower axis ensuring 90% containment for different layers",m_noLayer, -0.5, m_noLayer+0.5);
  m_histograms.push_back(hLayerContainment90);
  hLayerContainment85 = new TH1F("containmentLayer85","distance from shower axis ensuring 85% containment for different layers",m_noLayer, -0.5, m_noLayer+0.5);
  m_histograms.push_back(hLayerContainment85);
}

void ReconstructionExampleWithCells::processEvent(podio::EventStore& aStore, int aEventId, bool aVerbose) {
  if (aEventId != m_eventToAnalyse && m_eventToAnalyse != -1) {
    return;
  }
  // Get the collections
  const fcc::CaloClusterCollection* clusters(nullptr);
  const fcc::CaloHitCollection* cells(nullptr);

  bool testClusters = aStore.get(m_clusterCollName, clusters);
  bool testCells = aStore.get(m_posHitCollName, cells);

  // cells associated to the reconstructed cluster
  std::vector<uint64_t> cellIds;

  float clusterPhi;
  float clusterEta;
  float clusterEnergy = 0;
  // Get clusters reconstructed in an event
  if (testClusters) {
    if (aVerbose) {
      std::cout << "Number of clusters: " << clusters->size() << std::endl;
    }
    //Loop through the collection
    for (const auto iclu = clusters->begin(); iclu != clusters->end(); ++iclu) {
      if (aVerbose) {
        std::cout << "Cluster reconstructed at " << iclu->core().position.x
                  << " , " <<  iclu->core().position.y
                  << " , " <<  iclu->core().position.z
                  << "  with energy " <<  iclu->core().energy << " GeV" << std::endl;
      }
      TVector3 pos (iclu->core().position.x, iclu->core().position.y, iclu->core().position.z);
      float phi = pos.Phi();
      float eta = pos.Eta();
      if (iclu->core().energy > clusterEnergy) {
        clusterEnergy = iclu->core().energy;
        clusterEta = eta;
        clusterPhi = phi;
      }
      hClusterEnergy->Fill(eta, phi, iclu->core().energy);
      // get cells hat are associated to that cluster
      int cellsNo = iclu->hits_size();
      for (int icell = 0; icell < cellsNo; icell++) {
        cellIds.push_back(iclu->hits(icell).core().cellId);
      }
    }
  } else {
    std::cout << "No Cluster Collection in the event." << std::endl;
  }

  // Get cells
  if (testCells) {
    if (aVerbose) {
      std::cout << "Number of cells: " << cells->size() << std::endl;
    }
    Decoder encoder(m_encoder);
    //Loop through the collection
    for (const auto icell = cells->begin(); icell != cells->end(); ++icell) {
      if (aVerbose) {
        std::cout << "Cell id: " << icell->core().cellId
                  << "  with energy " <<  icell->core().energy << " GeV" << std::endl;
      }
      encoder.setValue(icell->core().cellId);
      int phiId = encoder["phi"];
      int etaId = encoder["eta"];
      int layerId = encoder["layer"];
      float  phiCell = phiId * m_dPhi + m_offsetPhi;
      float  etaCell = etaId * m_dEta + m_offsetEta;
      hAllCellEnergy->Fill(etaCell, phiCell, icell->core().energy);
      hLayerEnergy[layerId]->Fill(etaCell, phiCell, icell->core().energy);
      float rCell = sqrt(pow(phiCell - clusterPhi, 2) + pow(etaCell - clusterEta, 2));
      hContainment->Fill(rCell, icell->core().energy);
      hLayerContainment[layerId]->Fill(rCell, icell->core().energy);
      if(std::find(cellIds.begin(), cellIds.end(), icell->core().cellId) != cellIds.end()) {
        hClusterCellEnergy->Fill(etaCell, phiCell, icell->core().energy);
      }
    }

  double accumulateEnergyInR = 0;
  std::vector<double> accumulateEnergyInRInLayer;
  accumulateEnergyInRInLayer.assign(m_noLayer, 0);
  for (int iR = 0; iR < m_noR; iR++) {
    accumulateEnergyInR += hContainment->GetBinContent(iR + 1);
    hContainmentPercent->Fill(iR * m_dR, accumulateEnergyInR / hContainment->Integral());
  }
  for (int iLayer = 0; iLayer < m_noLayer; iLayer++) {
//    std::cout << " LAYER " << iLayer<< " layer energy: " << hLayerContainment[iLayer]->Integral() << " out of total energy: " << hAllCellEnergy->Integral() << std::endl;
    for (int iR = 0; iR < m_noR; iR++) {
    accumulateEnergyInRInLayer[iLayer] += hLayerContainment[iLayer]->GetBinContent(iR + 1);
    hLayerContainmentPercent[iLayer]->Fill(iR * m_dR, accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral());
      if (accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() > 0.95) {
        if ( hLayerContainment95->GetBinContent(iLayer)==0) {
          hLayerContainment95->SetBinContent(iLayer, iR * m_dR);
          //   std::cout << "iLayer = " << iLayer << " 95% cont up to dR = " <<  m_dR * iR << " -> E (%) " << accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() << std::endl;
        }
      }
      if (accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() > 0.9) {
        if (hLayerContainment90->GetBinContent(iLayer) == 0) {
          hLayerContainment90->SetBinContent(iLayer, iR * m_dR);
          //    std::cout << "iLayer = " << iLayer << " 90% cont up to dR = " <<  m_dR * iR << " -> E (%) " << accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() << std::endl;
        }
      }
      if (accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() > 0.85) {
        if (hLayerContainment85->GetBinContent(iLayer) == 0) {
          hLayerContainment85->SetBinContent(iLayer, iR * m_dR);
          //     std::cout << "iLayer = " << iLayer << " 85% cont up to dR = " <<  m_dR * iR << " -> E (%) " << accumulateEnergyInRInLayer[iLayer] / hLayerContainment[iLayer]->Integral() << std::endl;
        }
      }
    }
  }
  } else {
    std::cout << "No Cell Collection in the event." << std::endl;
  }
}

void ReconstructionExampleWithCells::finishLoop(int aNumEvents, bool aVerbose) {
  int numClusters = hClusterEnergy->GetEntries();
  if( numClusters > 1) {
    hAllCellEnergy->Scale(1./numClusters);
    hClusterCellEnergy->Scale(1./numClusters);
    hClusterEnergy->Scale(1./numClusters);
    for (int iLayer = 0; iLayer < m_noLayer; iLayer++) {
      hLayerEnergy[iLayer]->Scale(1./numClusters);
    }
    hContainment->Scale(1./aNumEvents);
  }
  for (int iLayer = 0; iLayer < m_noLayer; iLayer++) {
    hLayerContainment[iLayer]->Scale(1./aNumEvents);
    hLayerContainmentPercent[iLayer]->Scale(1./aNumEvents);
  }
  hContainment->Scale(1./aNumEvents);
  hContainmentPercent->Scale(1./aNumEvents);
  hLayerContainment95->Scale(1./aNumEvents);
  hLayerContainment90->Scale(1./aNumEvents);
  hLayerContainment85->Scale(1./aNumEvents);
}
