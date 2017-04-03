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
#include "TVector3.h"

// STL
#include <vector>
#include <iostream>

CaloAnalysis_simple::CaloAnalysis_simple(const double sf, const double ENE) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  ENERGY = ENE;

  //Histograms initialization
  histClass = new HistogramClass(ENERGY);
  histClass->Initialize_histos();
}


CaloAnalysis_simple::~CaloAnalysis_simple() {

  histClass->Delete_histos();
  delete histClass;

}

  

void CaloAnalysis_simple::loop(const std::string filename) {

  //Reset histograms
  histClass->Reset_histos();

  //Open file in the reader
  TString filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //filename_eos =  "root://eospublic.cern.ch//eos/fcc/users/n/novaj/June10_ecalShifted/"+filename;
    reader.openFile(filename);
    std::cout << "CaloAnalysis_simple opening file " << filename << std::endl;
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

  std::cout << "Total energy: " << histClass->h_cellEnergy->GetMean() << std::endl;
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


  double phiGen, etaGen;
  //MCParticle and Vertices collection 
  if (colMCParticlesOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
    } 
    //Loop through the collection   
    for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
      //Fill histogram
      TVector3 pGen(iparticle->core().p4.px,iparticle->core().p4.py,iparticle->core().p4.pz);
      //      std::cout << "phiGen " << std::atan2(iparticle.startVertex.position.y, iparticle.startVertex.position.x) << std::endl;
      phiGen = pGen.Phi();
      etaGen = pGen.Eta();
      if (verbose) {
	std::cout << "etaGen " << etaGen << " phiGen " << phiGen << std::endl;
      }
      histClass->h_ptGen->Fill( pGen.Pt() );
      histClass->h_phiGen->Fill( pGen.Phi() );
      histClass->h_etaGen->Fill( pGen.Eta() );
    }
  }
  else {
    if (verbose) {
      std::cout << "No MCTruth info available" << std::endl;
    }
  }
 
  //Total hit energy per event
  SumE_hit_ecal = 0.;
  histClass->h_phiHit->Reset();
  histClass->h_etaHit->Reset();
  //PositionedHits collection
  if (colECalPositionedHitsOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalPositionedHits:     " << colECalPositionedHits->size()    << std::endl;;
    }
    //Loop through the collection
    for (auto& iecl=colECalPositionedHits->begin(); iecl!=colECalPositionedHits->end(); ++iecl) 
        {
          //if (verbose) std::cout << "ECal hit energy " << iehit->core().energy << std::endl;
          SumE_hit_ecal += iecl->core().energy;

	  TVector3 pHit(iecl->position().x,iecl->position().y,iecl->position().z);
	  histClass->h_phiHit->Fill(pHit.Phi(),iecl->core().energy*SF);
	  histClass->h_etaHit->Fill(pHit.Eta(),iecl->core().energy*SF);
	}

    if (verbose) std::cout << "Total hit energy (GeV): " << SumE_hit_ecal << " total cell energy (GeV): " << SumE_hit_ecal*SF << " hit collection size: " << colECalPositionedHits->size() << std::endl;

    //Fill histograms
    histClass->h_hitEnergy->Fill(SumE_hit_ecal);
    histClass->h_cellEnergy->Fill(SumE_hit_ecal*SF);

    double phiHit = histClass->h_phiHit->GetMean();
    double etaHit = histClass->h_etaHit->GetMean();
    if (verbose) {
      std::cout << "etaHit " << etaHit << " phiHit " << phiHit << std::endl;
    }
    histClass->h_deltaPhi->Fill(phiHit-phiGen);
    histClass->h_deltaEta->Fill(etaHit-etaGen);
  }
  else {
    if (verbose) {
      std::cout << "No CaloPositionedHits Collection!!!!!" << std::endl;
    }
  }
}
