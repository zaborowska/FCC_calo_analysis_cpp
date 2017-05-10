#include "SingleParticleRecoMonitors.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

#include "TVector3.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <cmath>

SingleParticleRecoMonitors::SingleParticleRecoMonitors(const std::string& aClusterCollName, const std::string& aParticleCollName,
  double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi):
  m_clusterCollName(aClusterCollName), m_particleCollName(aParticleCollName), m_energy(aEnergy), m_etaMax(aEtaMax),
  m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi), m_ifCorrectForUpstream(false),
  m_cellCollName(""), m_decoder("") , m_layerFieldName(""), m_firstLayerFirstId(0), m_firstLayerLastId(0), m_firstLayerSF(0),
  m_P0p0(0), m_P0p1(0), m_P1p0(0), m_P1p1(0){
  Initialize_histos();
}

SingleParticleRecoMonitors::SingleParticleRecoMonitors(const std::string& aClusterCollName, const std::string& aParticleCollName,
  double aEnergy, double aEtaMax, int aNoEta, int aNoPhi, double aDEta, double aDPhi,
  const std::string& aCellCollName, const std::string& aBitfield, const std::string& aLayerField, int aLayerFirst, int aLayerLast, double aFirstLayerSF,
  double aP0p0, double aP0p1, double aP1p0, double aP1p1):
  m_clusterCollName(aClusterCollName), m_particleCollName(aParticleCollName), m_energy(aEnergy), m_etaMax(aEtaMax),
  m_noEta(aNoEta), m_noPhi(aNoPhi), m_dEta(aDEta), m_dPhi(aDPhi),
  m_ifCorrectForUpstream(true), m_cellCollName(aCellCollName),  m_decoder(aBitfield), m_layerFieldName(aLayerField), m_firstLayerFirstId(aLayerFirst), m_firstLayerLastId(aLayerLast), m_firstLayerSF(aFirstLayerSF),
  m_P0p0(aP0p0), m_P0p1(aP0p1), m_P1p0(aP1p0), m_P1p1(aP1p1) {
  Initialize_histos();
}

SingleParticleRecoMonitors::~SingleParticleRecoMonitors(){}


void SingleParticleRecoMonitors::Initialize_histos() {

  hEnTotal = new TH1F("energyTotal",
    ("Energy of all clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    99,0.,1.5*m_energy);
  hEn = new TH1F("energy",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    99,0.,1.5*m_energy);
  hEnCorr = new TH1F("energyCorrected",
    ("Energy of clusters corrected for upstrem energy (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    99,0.,1.5*m_energy);
  hEnFirstLayer = new TH1F("energyFirstLayer",
    ("Energy of cells within cluster in the first layer (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    999,0.,0.2*m_energy);
  hEnUpstream = new TH1F("energyUpstream",
    ("Upstream energy (e^{-}, "+std::to_string(int(m_energy))+" GeV);energy (GeV);fraction of events").c_str(),
    999,0.,0.2*m_energy);
  hEnFncPhi = new TH2F("energy_phi",
    ("Energy of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;energy (GeV)").c_str(),
    m_noPhi,-M_PI,M_PI,
    99,0.5*m_energy,1.5*m_energy);
  hEta = new TH1F("eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;fraction of events").c_str()
    ,101,-10*m_dEta,10*m_dEta);
  hPhi = new TH1F("phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;fraction of events").c_str(),
    909,-100*m_dPhi,100*m_dPhi);
  hPhiFncPhi = new TH2F("phi_phi",
    ("#Delta #varphi (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;#Delta#varphi").c_str(),
    m_noPhi,-M_PI,M_PI,
    909,-100*m_dPhi,100*m_dPhi);
  hEtaFncEta = new TH2F("eta_eta",
    ("#Delta #eta (e^{-}, "+std::to_string(int(m_energy))+" GeV);#eta;#Delta#eta").c_str(),
    m_noEta,-1.8,1.8,
    101,-10*m_dEta,10*m_dEta);
  hNo = new TH1F("clusters",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);fraction of events;number of clusters per event").c_str(),
    7,-0.5,6.5);
  hNoFncPhi = new TH2F("clusters_phi",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;number of clusters per event").c_str(),
    m_noPhi,-M_PI,M_PI,
    7,-0.5,7.5);
  hNoFncEta = new TH2F("clusters_eta",
    ("Number of clusters (e^{-}, "+std::to_string(int(m_energy))+" GeV);#eta;number of clusters per event").c_str(),
    m_noEta,-m_etaMax,m_etaMax,
    7,-0.5,7.5);
  hEnMoreClu = new TH1F("energy_duplicates",
    ("Energy of cluster duplicates (e^{-}, "+std::to_string(int(m_energy))+" GeV);E (GeV);number of clusters").c_str(),
    99,0.,1.5*m_energy);
  hEnDiffMoreClu = new TH1F("energy_diff",
    ("#DeltaE/E for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta E / E;number of clusters").c_str(),
    101,0,1);
  hEtaMoreClu = new TH1F("eta_duplicates",
    ("#eta of cluster duplicates (e^{-}, "+std::to_string(int(m_energy))+" GeV);#eta;number of clusters").c_str(),
    m_noEta,-m_etaMax, m_etaMax);
  hEtaDiffMoreClu = new TH1F("eta_diff",
    ("#Delta#eta for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#eta;number of clusters").c_str(),
    101,-10*m_dEta,10*m_dEta);
  hPhiMoreClu = new TH1F("phi_duplicates",
    ("#varphi fof cluster duplicates (e^{-}, "+std::to_string(int(m_energy))+" GeV);#varphi;number of clusters").c_str(),
    m_noPhi,-M_PI,M_PI);
  hPhiDiffMoreClu = new TH1F("phi_diff",
    ("#Delta#varphi for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta#varphi;number of clusters").c_str(),
    101,-2.1*M_PI,2.1*M_PI);
  hRDiffMoreClu = new TH1F("R_diff",
    ("#Delta R for events with more than 1 cluster (e^{-}, "+std::to_string(int(m_energy))+" GeV);#Delta R;number of clusters").c_str(),
    101,-200*m_dPhi,200*m_dPhi);

  m_histograms.push_back(hEnTotal);
  m_histograms.push_back(hEn);
  m_histograms.push_back(hEnCorr);
  m_histograms.push_back(hEnFirstLayer);
  m_histograms.push_back(hEnUpstream);
  m_histograms.push_back(hEnFncPhi);
  m_histograms.push_back(hEta);
  m_histograms.push_back(hPhi);
  m_histograms.push_back(hEtaFncEta);
  m_histograms.push_back(hPhiFncPhi);
  m_histograms.push_back(hNo);
  m_histograms.push_back(hNoFncPhi);
  m_histograms.push_back(hNoFncEta);
  m_histograms.push_back(hEnMoreClu);
  m_histograms.push_back(hEnDiffMoreClu);
  m_histograms.push_back(hEtaMoreClu);
  m_histograms.push_back(hEtaDiffMoreClu);
  m_histograms.push_back(hPhiMoreClu);
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

  double EfirstLayer = 0.;
  if( m_ifCorrectForUpstream ) {
    // get cells to calculate energy deposited in first layer
    const fcc::CaloHitCollection* cells(nullptr);
    bool testCells = aStoreRec.get(m_cellCollName, cells);
    if (testCells) {
      if (aVerbose) {
        std::cout << "Number of cells: " << cells->size() << std::endl;
      }
      uint verb=0;
      for (const auto icell = cells->begin(); icell != cells->end(); ++icell) {
        int layerId = m_decoder.value(m_layerFieldName,icell->core().cellId);
        int etaId = m_decoder.value("eta",icell->core().cellId);
        int phiId = m_decoder.value("phi",icell->core().cellId);
        if( layerId >= m_firstLayerFirstId && layerId <= m_firstLayerLastId ) {
          // TODO  make additional check on eta & phi position: within window
          EfirstLayer += icell->core().energy;
        }
      }
      // if cells were already calibrated to EM scale, scale them back
      EfirstLayer *= m_firstLayerSF;
    } else {
      std::cout << "No Cell Collection in the event." << std::endl;
      return;
    }
  }

  // Get clusters reconstructed in an event
  if (testClusters) {
    if (aVerbose) {
      std::cout << "Number of clusters: " << clusters->size() << std::endl;
    }
    double sumEnergy = 0;
    double maxEnergy = 0;
    double phiAtMaxEnergy = 0;
    double etaAtMaxEnergy = 0;
    //Loop through the collection, find the cluster with highest energy
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
      sumEnergy += iclu->core().energy;
      if(maxEnergy < iclu->core().energy) {
        maxEnergy = iclu->core().energy;
        phiAtMaxEnergy = phi;
        etaAtMaxEnergy = eta;
      }
    }
    // fill histograms for all clusters in the event
    hEnTotal->Fill(sumEnergy);
    hNoFncPhi->Fill(phiAtMaxEnergy, clusters->size());
    hNoFncEta->Fill(etaAtMaxEnergy, clusters->size());
    hNo->Fill(clusters->size());
    // distinguish between cluster = reconstructed particle and duplicates
    for (const auto iclu = clusters->begin(); iclu != clusters->end(); ++iclu) {
      // duplicates
      if(iclu->core().energy < maxEnergy) {
        TVector3 pos (iclu->core().position.x, iclu->core().position.y, iclu->core().position.z);
        float phi = pos.Phi();
        float eta = pos.Eta();
        hEnDiffMoreClu->Fill( (maxEnergy - iclu->core().energy)/m_energy );
        hEnMoreClu->Fill( iclu->core().energy );
        hEtaMoreClu->Fill( eta );
        hEtaDiffMoreClu->Fill( etaAtMaxEnergy - eta );
        hPhiMoreClu->Fill( phi );
        hPhiDiffMoreClu->Fill( phiAtMaxEnergy - phi );
        hRDiffMoreClu->Fill( sqrt(pow(phiAtMaxEnergy,2)+pow(etaAtMaxEnergy,2))
          - sqrt(pow(phi,2)+pow(eta,2)) );
      } else {
        // reconstructed particle
        hEta->Fill(etaAtMaxEnergy-momentum.Eta());
        hPhi->Fill(phiAtMaxEnergy-momentum.Phi());
        hEtaFncEta->Fill(momentum.Eta(), etaAtMaxEnergy-momentum.Eta());
        hPhiFncPhi->Fill(momentum.Phi(), phiAtMaxEnergy-momentum.Phi());
        hEn->Fill(maxEnergy);
        hEnFncPhi->Fill(momentum.Phi(), maxEnergy);
        if( m_ifCorrectForUpstream ) {
          // correct for energy upstream (lost in tracker, cryostat...)
          // calculate parameters based on reconstructed energy
          double EupstreamP0 = m_P0p0 + m_P0p1 * maxEnergy;
          double EupstreamP1 = m_P0p0 + m_P0p1 / sqrt( maxEnergy );
          double Eupstream = EupstreamP0 + EupstreamP1 * EfirstLayer;
          hEnCorr->Fill(maxEnergy + Eupstream);
          hEnFirstLayer->Fill(EfirstLayer);
          hEnUpstream->Fill(Eupstream);
        }
      }
    }
  } else {
    std::cout << "No Cluster Collection in the event." << std::endl;
    return;
  }
}

void SingleParticleRecoMonitors::finishLoop(int aNumEvents, bool aVerbose) {
  int numClusters = hEn->GetEntries();
  hEnTotal->Scale(1./aNumEvents);
  hEn->Scale(1./numClusters);
  hEnCorr->Scale(1./numClusters);
  hEnFirstLayer->Scale(1./numClusters);
  hEnUpstream->Scale(1./numClusters);
  hEnFncPhi->Scale(1./numClusters);
  hEta->Scale(1./numClusters);
  hPhi->Scale(1./numClusters);
  hPhiFncPhi->Scale(1./numClusters);
  hEtaFncEta->Scale(1./numClusters);
  hNo->Scale(1./aNumEvents);
  hNoFncPhi->Scale(1./aNumEvents);
  hNoFncEta->Scale(1./aNumEvents);
}
