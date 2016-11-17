#include "CaloAnalysis_simple.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
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

CaloAnalysis_simple::CaloAnalysis_simple(const double aSf, const double aEnergy):
  m_sf(aSf), m_energy(aEnergy), m_histograms(aEnergy) {
  TH1::AddDirectory(kFALSE);
  m_histograms.Initialize_histos();
}

CaloAnalysis_simple::~CaloAnalysis_simple() {
  m_histograms.Delete_histos();
}

void CaloAnalysis_simple::loop(const std::string& aFilename) {
  //Reset histograms
  m_histograms.Reset_histos();

  //Open file in the reader
  TString filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    reader.openFile(aFilename);
    std::cout << "CaloAnalysis_simple opening file " << aFilename << std::endl;
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
    if(i%1000==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) verbose = false;

    processEvent(store, verbose, reader);

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "Total energy: " << m_histograms.h_cellEnergy->GetMean() << std::endl;
  std::cout << "End of loop" << std::endl;

  return;
}

void CaloAnalysis_simple::processEvent(podio::EventStore& store, bool verbose,
  podio::ROOTReader& reader) {
  //Get the collections
  const fcc::MCParticleCollection*  colMCParticles(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits(nullptr);

  bool colMCParticlesOK = store.get("GenParticles", colMCParticles);
  bool colECalPositionedHitsOK     = store.get("ECalPositionedHits" , colECalPositionedHits);

  //Total hit energy per event
  double SumE_hit_ecal = 0.;

  //PositionedHits collection
  if (colECalPositionedHitsOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalPositionedHits:     " << colECalPositionedHits->size()    << std::endl;
    }
    //Loop through the collection
    for (auto& iecl=colECalPositionedHits->begin(); iecl!=colECalPositionedHits->end(); ++iecl) {
      //if (verbose) std::cout << "ECal hit energy " << iehit->core().energy << std::endl;
      SumE_hit_ecal += iecl->core().energy;
    }

    if (verbose) std::cout << "Total hit energy (GeV): " << SumE_hit_ecal << " total cell energy (GeV): " << SumE_hit_ecal*m_sf << " hit collection size: " << colECalPositionedHits->size() << std::endl;

    //Fill histograms
    m_histograms.h_hitEnergy->Fill(SumE_hit_ecal);
    m_histograms.h_cellEnergy->Fill(SumE_hit_ecal*m_sf);
  }
  else {
    if (verbose) {
      std::cout << "No CaloPositionedHits Collection!!!!!" << std::endl;
    }
  }

  //MCParticle and Vertices collection
  if (colMCParticlesOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
    }
    //Loop through the collection
    for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
      //Fill histogram
      m_histograms.h_ptGen->Fill( sqrt( pow(iparticle->core().p4.px,2)+
          pow(iparticle->core().p4.py,2) ) );
    }
  }
  else {
    if (verbose) {
      std::cout << "No MCTruth info available" << std::endl;
    }
  }

}
