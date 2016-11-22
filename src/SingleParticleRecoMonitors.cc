#include "SingleParticleRecoMonitors.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/CaloClusterCollection.h"

#include "TVector3.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <cmath>

SingleParticleRecoMonitors::SingleParticleRecoMonitors(const std::string& aClusterCollName, const std::string& aParticleCollName, double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_clusterCollName(aClusterCollName), m_particleCollName(aParticleCollName), m_energy(aEnergy), m_etaMax(aEtaMax), m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi) {
  Initialize_histos();
}

SingleParticleRecoMonitors::~SingleParticleRecoMonitors(){}


void SingleParticleRecoMonitors::Initialize_histos() {

  hEn = new TH1F("energy",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    101,0.8*m_energy,1.2*m_energy);
  hEnFncPhi = new TH2F("energy_phi",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;energy (GeV)").c_str(),
    m_noPhi,-M_PI,M_PI,
    11,0.8*m_energy,1.2*m_energy);
  hEta = new TH1F("eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;fraction of events").c_str()
    ,101,-10*m_dEta,10*m_dEta);
  hPhi = new TH1F("phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;fraction of events").c_str(),
    101,-10*m_dPhi,10*m_dPhi);
  hPhiFncPhi = new TH2F("phi_phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;#Delta#varphi").c_str(),
    m_noPhi,-M_PI,M_PI,
    101,-10*m_dPhi,10*m_dPhi);
  hEtaFncEta = new TH2F("eta_eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#eta;#Delta#eta").c_str(),
    m_noEta,-1.8,1.8,
    101,-10*m_dEta,10*m_dEta);
  hNo = new TH1F("clusters",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);fraction of events;number of clusters per event").c_str(),
    5,-0.5,4.5);
  hNoFncPhi = new TH2F("clusters_phi",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;number of clusters per event").c_str(),
    m_noPhi,-M_PI,M_PI,
    5,-0.5,4.5);
  hEnDiffMoreClu = new TH1F("energy_diff",
    ("#DeltaE/E for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta E / E;number of events").c_str(),
    101,0,1);
  hEtaDiffMoreClu = new TH1F("eta_diff",
    ("#Delta#eta for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;number of events").c_str(),
    101,-10*m_dEta,10*m_dEta);
  hPhiDiffMoreClu = new TH1F("phi_diff",
    ("#Delta#varphi for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;number of events").c_str(),
    101,-2.1*M_PI,2.1*M_PI);
  hRDiffMoreClu = new TH1F("R_diff",
    ("#Delta R for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta R;number of events").c_str(),
    101,-50*m_dPhi,50*m_dPhi);

  m_histograms.push_back(hEn);
  m_histograms.push_back(hEnFncPhi);
  m_histograms.push_back(hEta);
  m_histograms.push_back(hPhi);
  m_histograms.push_back(hEtaFncEta);
  m_histograms.push_back(hPhiFncPhi);
  m_histograms.push_back(hNo);
  m_histograms.push_back(hNoFncPhi);
  m_histograms.push_back(hEnDiffMoreClu);
  m_histograms.push_back(hEtaDiffMoreClu);
  m_histograms.push_back(hPhiDiffMoreClu);
  m_histograms.push_back(hRDiffMoreClu);
}

void SingleParticleRecoMonitors::processEvent(podio::EventStore& aStoreSim, podio::EventStore& aStoreRec, int aEventId, bool aVerbose) {
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
      hEta->Fill(eta-momentum.Eta(), iclu->core().energy);
      hPhi->Fill(phi-momentum.Phi(), iclu->core().energy);
      hEtaFncEta->Fill(momentum.Eta(), eta-momentum.Eta(), iclu->core().energy);
      hPhiFncPhi->Fill(momentum.Phi(), phi-momentum.Phi(), iclu->core().energy);
      hEn->Fill(iclu->core().energy);
      hEnFncPhi->Fill(momentum.Phi(), iclu->core().energy);
      if(maxEnergy < iclu->core().energy) {
        maxEnergy = iclu->core().energy;
        phiAtMaxEnergy = phi;
        etaAtMaxEnergy = eta;
      }
    }
    hNoFncPhi->Fill(phiAtMaxEnergy, clusters->size());
    hNo->Fill(clusters->size());
    if(clusters->size() > 1) {
      for (const auto iclu = clusters->begin(); iclu != clusters->end(); ++iclu) {
        if(iclu->core().energy < maxEnergy) {
          TVector3 pos (iclu->core().position.x, iclu->core().position.y, iclu->core().position.z);
          float phi = pos.Phi();
          float eta = pos.Eta();
          hEnDiffMoreClu->Fill( (maxEnergy - iclu->core().energy)/m_energy );
          hEtaDiffMoreClu->Fill( etaAtMaxEnergy - eta );
          hPhiDiffMoreClu->Fill( phiAtMaxEnergy - phi );
          hRDiffMoreClu->Fill( sqrt(pow(phiAtMaxEnergy,2)+pow(etaAtMaxEnergy,2))
            - sqrt(pow(phi,2)+pow(eta,2)) );
        }
      }
    }
  } else {
    std::cout << "No Cluster Collection in the event." << std::endl;
  }
}

void SingleParticleRecoMonitors::finishLoop(int aNumEvents, bool aVerbose) {}
