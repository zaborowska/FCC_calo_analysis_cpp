#include "CaloAnalysis_recoExample.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloClusterCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// ROOT
#include "TObject.h"
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TString.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"
#include "TLorentzVector.h"

// STL
#include <vector>
#include <iostream>

CaloAnalysis_recoExample::CaloAnalysis_recoExample(const std::string& aCluserCollName, const std::string& aPosHitCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_clusterCollName(aCluserCollName), m_posHitCollName(aPosHitCollName), m_energy(aEnergy), m_histograms(aEnergy, aEtaMax, aNoEta, aNoPhi, aDEta, aDPhi) {
  TH1::AddDirectory(kFALSE);
  m_histograms.Initialize_histos();
}

CaloAnalysis_recoExample::~CaloAnalysis_recoExample() {
  m_histograms.Delete_histos();
}

void CaloAnalysis_recoExample::analyseEvent(const std::string& aFilename, int aNumEvent) {
  //Reset histograms
  m_histograms.Reset_histos();

  //Open file in the reader
  TString filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    reader.openFile(aFilename);
    std::cout << "CaloAnalysis_recoExample opening file " << aFilename << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  store.setReader(&reader);

  bool verbose = true;

  //Loop over all events
  unsigned nEvents = reader.getEntries();
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i == aNumEvent) {
      std::cout<<"processing event "<<i<<std::endl;
      processEvent(store, verbose, reader);
    }
    store.clear();
    reader.endOfEvent();
  }

  std::cout << "End of loop" << std::endl;

  return;
}

void CaloAnalysis_recoExample::processEvent(podio::EventStore& store, bool verbose,
  podio::ROOTReader& reader) {
  // Get the collections
  const fcc::CaloClusterCollection* clusters(nullptr);
  const fcc::PositionedCaloHitCollection* cells(nullptr);

  bool testClusters = store.get(m_clusterCollName, clusters);
  bool testCells = store.get(m_posHitCollName, cells);

  // cells associated to the reconstructed cluster
  std::vector<long long> cellIds;

  // Get clusters reconstructed in an event
  if (testClusters) {
    if (verbose) {
      std::cout << "Number of clusters: " << clusters->size() << std::endl;
    }
    //Loop through the collection
    for (const auto iclu = clusters->begin(); iclu != clusters->end(); ++iclu) {
      if (verbose) {
        std::cout << "Cluster reconstructed at " << iclu->core().position.x
                  << " , " <<  iclu->core().position.y
                  << " , " <<  iclu->core().position.z
                  << "  with energy " <<  iclu->core().energy << " GeV" << std::endl;
      }
      TVector3 pos (iclu->core().position.x, iclu->core().position.y, iclu->core().position.z);
      float phi = pos.Phi();
      float eta = pos.Eta();
      m_histograms.hClusterEnergy->Fill(eta, phi, iclu->core().energy);
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
    if (verbose) {
      std::cout << "Number of cells: " << cells->size() << std::endl;
    }
    //Loop through the collection
    for (const auto icell = cells->begin(); icell != cells->end(); ++icell) {
      if (verbose) {
        std::cout << "Cell at " << icell->position().x
                  << " , " <<  icell->position().y
                  << " , " <<  icell->position().z
                  << "  with energy " <<  icell->core().energy << " GeV" << std::endl;
      }
      TVector3 position(icell->position().x, icell->position().y, icell->position().z);
      float  phiCell = position.Phi();
      float  etaCell = position.Eta();
      m_histograms.hAllCellEnergy->Fill(etaCell, phiCell, icell->core().energy);
      if(std::find(cellIds.begin(), cellIds.end(), icell->core().cellId) != cellIds.end()) {
        m_histograms.hClusterCellEnergy->Fill(etaCell, phiCell, icell->core().energy);
      }
    }
  } else {
    std::cout << "No Cluster Collection in the event." << std::endl;
  }
}
