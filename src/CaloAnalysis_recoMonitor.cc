#include "CaloAnalysis_recoMonitor.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/CaloClusterCollection.h"

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

CaloAnalysis_recoMonitor::CaloAnalysis_recoMonitor(const std::string& aClusterCollName, const std::string& aParticleCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_clusterCollName(aClusterCollName), m_particleCollName(aParticleCollName), m_energy(aEnergy), m_histograms(aEnergy, aEtaMax, aNoEta, aNoPhi, aDEta, aDPhi) {
  TH1::AddDirectory(kFALSE);
  m_histograms.Initialize_histos();
}

CaloAnalysis_recoMonitor::~CaloAnalysis_recoMonitor() {
  m_histograms.Delete_histos();
}

void CaloAnalysis_recoMonitor::loop(const std::string& aFilenameSim, const std::string& aFilenameRec, bool aVerbose) {
  //Reset histograms
  m_histograms.Reset_histos();

  //Open file in the reader - with particles
  auto readerSim = podio::ROOTReader();
  auto storeSim = podio::EventStore();
  try {
    readerSim.openFile(aFilenameSim);
    std::cout << "CaloAnalysis_recoMonitor opening file with simulation output " << aFilenameSim << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  storeSim.setReader(&readerSim);
  //Open file in the reader - with reconstructed clusters
  auto readerRec = podio::ROOTReader();
  auto storeRec = podio::EventStore();
  try {
    readerRec.openFile(aFilenameRec);
    std::cout << "CaloAnalysis_recoMonitor opening file with reconstruction output " << aFilenameRec << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  storeRec.setReader(&readerRec);

  //Loop over all events
  unsigned nEvents = readerRec.getEntries();
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%100 == 0) {
      std::cout<<"processing event "<<i<<std::endl;
    }
    processEvent(storeSim, readerSim, storeRec, readerRec, aVerbose);
    storeSim.clear();
    storeRec.clear();
    readerSim.endOfEvent();
    readerRec.endOfEvent();
  }

  std::cout << "End of loop" << std::endl;

  return;
}

void CaloAnalysis_recoMonitor::processEvent(podio::EventStore& aStoreSim, podio::ROOTReader& aReaderSim,
  podio::EventStore& aStoreRec, podio::ROOTReader& aReaderRec, bool aVerbose) {
  // Get the collections
  const fcc::CaloClusterCollection* clusters(nullptr);
  const fcc::MCParticleCollection* particles(nullptr);

  bool testParticles = aStoreSim.get(m_particleCollName, particles);
  bool testClusters = aStoreRec.get(m_clusterCollName, clusters);

  TVector3 momentum;

  // Get generated particles - assuming single particle events
  if (testParticles) {
    if (particles->size() > 1) {
      std::cout << "This is not a single particle event! Number of particles: " << particles->size() << std::endl;
    }
    //Loop through the collection
    for (const auto ipart = particles->begin(); ipart != particles->end(); ++ipart) {
      if (aVerbose) {
        std::cout << "Particle at " << ipart->core().vertex.x
                  << " , " <<  ipart->core().vertex.y
                  << " , " <<  ipart->core().vertex.z
                  << "  with momentum " << ipart->core().p4.px
                  << " , " <<  ipart->core().p4.py
                  << " , " <<  ipart->core().p4.pz
                  << "  and mass " <<  ipart->core().p4.mass << " GeV" << std::endl;
      }
      momentum = TVector3(ipart->core().p4.px, ipart->core().p4.py, ipart->core().p4.pz);
    }
  } else {
    std::cout << "No MC Particle Collection in the event." << std::endl;
    return;
  }

  // Get clusters reconstructed in an event
  if (testClusters) {
    if (aVerbose) {
      std::cout << "Number of clusters: " << clusters->size() << std::endl;
    }
    double maxEnergy = 0;
    double phiAtMaxEnergy = 0;
    double etaAtMaxEnergy = 0;
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
      m_histograms.hEta->Fill(eta-momentum.Eta(), iclu->core().energy);
      m_histograms.hPhi->Fill(phi-momentum.Phi(), iclu->core().energy);
      m_histograms.hEtaFncEta->Fill(momentum.Eta(), eta-momentum.Eta(), iclu->core().energy);
      m_histograms.hPhiFncPhi->Fill(momentum.Phi(), phi-momentum.Phi(), iclu->core().energy);
      m_histograms.hEn->Fill(iclu->core().energy);
      m_histograms.hEnFncPhi->Fill(momentum.Phi(), iclu->core().energy);
      if(maxEnergy < iclu->core().energy) {
        maxEnergy = iclu->core().energy;
        phiAtMaxEnergy = phi;
        etaAtMaxEnergy = eta;
      }
    }
    m_histograms.hNoFncPhi->Fill(phiAtMaxEnergy, clusters->size());
    m_histograms.hNo->Fill(clusters->size());
    if(clusters->size() > 1) {
      for (const auto iclu = clusters->begin(); iclu != clusters->end(); ++iclu) {
        if(iclu->core().energy < maxEnergy) {
          TVector3 pos (iclu->core().position.x, iclu->core().position.y, iclu->core().position.z);
          float phi = pos.Phi();
          float eta = pos.Eta();
          m_histograms.hEnDiffMoreClu->Fill( (maxEnergy - iclu->core().energy)/m_energy );
          m_histograms.hEtaDiffMoreClu->Fill( etaAtMaxEnergy - eta );
          m_histograms.hPhiDiffMoreClu->Fill( phiAtMaxEnergy - phi );
          m_histograms.hRDiffMoreClu->Fill( sqrt(pow(phiAtMaxEnergy,2)+pow(etaAtMaxEnergy,2))
            - sqrt(pow(phi,2)+pow(eta,2)) );
        }
      }
    }
  } else {
    std::cout << "No Cluster Collection in the event." << std::endl;
  }
}
