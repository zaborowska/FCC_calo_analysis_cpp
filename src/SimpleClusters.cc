#include "SimpleClusters.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloClusterCollection.h"

// ROOT
#include "TH1F.h"
#include "TVector3.h"

// STL
#include <iostream>

SimpleClusters::SimpleClusters(double aEnergy):
  m_energy(aEnergy), hClustersEnergy(nullptr), hFirstClusterEnergy(nullptr) {
  Initialize_histos();
}
SimpleClusters::~SimpleClusters(){}


void SimpleClusters::Initialize_histos() {
 
  hClustersEnergy = new TH1F("hClustersEnergy","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(hClustersEnergy);

  hFirstClusterEnergy = new TH1F("energy","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(hFirstClusterEnergy);

}

void SimpleClusters::processEvent(podio::EventStore& store, int aEventId, bool verbose) {
  //Get the collections
  const fcc::CaloClusterCollection*     clusters(nullptr);

  bool testCluster = store.get("CombinedClusters" , clusters);

  double energyMin = 5.;
  double etaMin = -0.01;
  double etaMax = 0.5;

  //PositionedHits collection
  if (testCluster) {
    //Loop through the collection
    double maxEnergy = 0.;
    TVector3 position;
    for (auto& iclu=clusters->begin(); iclu!=clusters->end(); ++iclu) {
      position.SetXYZ(iclu->position().x,iclu->position().y,iclu->position().z);
      double eta = position.Eta();
      if (fabs(eta>etaMin) && fabs(eta<etaMax)) {
	hClustersEnergy->Fill(iclu->core().energy);
	if(maxEnergy < iclu->core().energy) {
	  maxEnergy = iclu->core().energy;
	}
      }
    }
    if (maxEnergy>energyMin) {
      hFirstClusterEnergy->Fill(maxEnergy);
    }
  }
  else {
    if (verbose) {
      std::cout << "No clusters collection!!!!!" << std::endl;
    }
  }

}

void SimpleClusters::finishLoop(int aNumEvents, bool aVerbose) {
  std::cout << "Mean cluster energy: " << hClustersEnergy->GetMean() << std::endl;
  std::cout << "Mean of most energetic cluster energy: " << hFirstClusterEnergy->GetMean() << std::endl;
}
