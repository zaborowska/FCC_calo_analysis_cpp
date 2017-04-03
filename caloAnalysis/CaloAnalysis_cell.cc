#include "CaloAnalysis_cell.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloHitCollection.h"
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
#include <bitset>


CaloAnalysis_cell::CaloAnalysis_cell(const double sf, const double ENE) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  ENERGY = ENE;

  //Histograms initialization
  histClass = new HistogramClass_cell(ENERGY);
  histClass->Initialize_histos();
}


CaloAnalysis_cell::~CaloAnalysis_cell() {

  histClass->Delete_histos();
  delete histClass;

}

  

void CaloAnalysis_cell::loop(const std::string filename) {

  //Reset histograms
  histClass->Reset_histos();

  //Open file in the reader
  TString filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //filename_eos =  "root://eospublic.cern.ch//eos/fcc/users/n/novaj/June10_ecalShifted/"+filename;
    reader.openFile(filename);
    std::cout << "CaloAnalysis_cell opening file " << filename << std::endl;
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
    //std::cout<<"reading event "<<i<<std::endl;
    if(i%1000==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) verbose = false;

    processEvent(store, verbose, reader);

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "Total energy: " << histClass->h_cellEnergy->GetMean() << " check " <<  histClass->h_cellEnergy_check->GetMean() << std::endl;
  std::cout << "End of loop" << std::endl;

  histClass->h_ene_r->Scale(1./(float)nEvents);
  histClass->h_ene_phi->Scale(1./(float)nEvents);
  histClass->h_ene_eta->Scale(1./(float)nEvents);

  histClass->h_ene_r_check->Scale(1./(float)nEvents);
  histClass->h_ene_phi_check->Scale(1./(float)nEvents);
  histClass->h_ene_eta_check->Scale(1./(float)nEvents);

  std::cout << "Integral phi " << histClass->h_ene_phi->Integral() << " check " << histClass->h_ene_phi_check->Integral() << std::endl;

  return;
}


void CaloAnalysis_cell::processEvent(podio::EventStore& store, bool verbose,
				     podio::ROOTReader& reader) {

  //Get the collections
  const fcc::CaloHitCollection*     colECalCell(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits_new(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits_old(nullptr);
 
  bool colECalCellOK     = store.get("caloCells" , colECalCell);
  bool colECalPositionedHits_newOK     = store.get("caloCellsPositions" , colECalPositionedHits_new);
  bool colECalPositionedHits_oldOK     = store.get("ECalPositionedHits" , colECalPositionedHits_old);

  //Total hit energy per event
  SumE_cell = 0.;
  
  //Cell collection
  if (colECalCellOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #caloCells:     " << colECalCell->size()    << std::endl;;
    }
    //Loop through the collection
    for (auto& iecl=colECalCell->begin(); iecl!=colECalCell->end(); ++iecl) 
      {
	//if (verbose) std::cout << "ECal cell energy " << iecl->core().energy << std::endl;
	SumE_cell += iecl->core().energy;
	histClass->h_cellId->Fill(iecl->core().cellId);
      }
    //Fill histograms
    //histClass->h_cellEnergy->Fill(SumE_cell);
    
  }
  else {
    if (verbose) {
      std::cout << "No CaloHit Collection!!!!!" << std::endl;
    }
  }

  if (verbose) std::cout << "Total energy 1: " << SumE_cell << std::endl;


  SumE_cell = 0.;
  //Cell collection
  if (colECalPositionedHits_newOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #newCaloPositionedHits:     " << colECalPositionedHits_new->size()    << std::endl;;
    }
    for (auto& iecl=colECalPositionedHits_new->begin(); iecl!=colECalPositionedHits_new->end(); ++iecl) 
      {

	  //if (verbose) std::cout << "ECal cell energy " << iecl->core().energy << std::endl;
	  SumE_cell += iecl->core().energy;
	  //if (iecl->core().energy>0.005) {
	  double r = sqrt(pow(iecl->position().x,2)+pow(iecl->position().y,2));
	  //std::cout << " r " << r << std::endl;
	//if (verbose) std::cout << " x " << iecl->position().x << " y " << iecl->position().y << std::endl;
	    TVector3 vec(iecl->position().x,iecl->position().y,iecl->position().z);
	    double phi = atan2( iecl->position().y, iecl->position().x );
	    double eta = vec.Eta();
	    if (verbose && iecl->core().energy>10.) {
	      std::cout << " eta " << eta << " phi " << phi << " energy " << iecl->core().energy << std::endl;
	    }
	    histClass->h_ene_r->Fill(r,iecl->core().energy);
	    histClass->h_ene_phi->Fill(phi,iecl->core().energy);
	    histClass->h_ene_eta->Fill(eta,iecl->core().energy);
	    //	  }
      }
  
    if (verbose) std::cout << "Total energy 2: " << SumE_cell << std::endl;
    //Fill histograms
    histClass->h_cellEnergy->Fill(SumE_cell);
  }
  else {
    if (verbose) {
      std::cout << "No colECalPositionedHits_new Collection!!!!!" << std::endl;
    }
  }


  double SumE_cell_check = 0.;
  double SF = 5.4;
  //Cell collection
  if (colECalPositionedHits_oldOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #oldCaloPositionedHitss:     " << colECalPositionedHits_old->size()    << std::endl;;
    }
    for (auto& iecl=colECalPositionedHits_old->begin(); iecl!=colECalPositionedHits_old->end(); ++iecl) 
      {
	SumE_cell_check += iecl->core().energy;
	double r = sqrt(pow(iecl->position().x,2)+pow(iecl->position().y,2));
	TVector3 vec(iecl->position().x,iecl->position().y,iecl->position().z);
	double phi = atan2( iecl->position().y, iecl->position().x );
	double eta = vec.Eta();
	//if (verbose && iecl->core().energy>2.) std::cout << " eta " << eta << " phi " << phi << " energy " << iecl->core().energy << std::endl;
	histClass->h_ene_r_check->Fill(r,iecl->core().energy*SF);
	histClass->h_ene_phi_check->Fill(phi,iecl->core().energy*SF);
	histClass->h_ene_eta_check->Fill(eta,iecl->core().energy*SF);
      }
    if (verbose) std::cout << "Total energy 3: " << SumE_cell_check*SF << std::endl;
    histClass->h_cellEnergy_check->Fill(SumE_cell_check*SF);
  }
  else {
    if (verbose) {
      std::cout << "No colECalPositionedHits_old Collection!!!!!" << std::endl;
    }
  }
 
}
