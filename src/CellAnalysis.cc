#include "CellAnalysis.h"

// FCC-EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// PODIO
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// ROOT
#include "TVector3.h"

// STL
#include <iostream>

CellAnalysis::CellAnalysis(double aEnergy, double aSf) : m_energy(aEnergy), m_sf(aSf), SumE_cell(0), h_cellEnergy(nullptr) {
  Initialize_histos();
}

CellAnalysis::~CellAnalysis() {}


void CellAnalysis::Initialize_histos() {
  h_cellEnergy = new TH1F("h_cellEnergy","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(h_cellEnergy);
  h_cellEnergy_check = new TH1F("h_cellEnergy_check","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(h_cellEnergy_check);

  h_cellId = new TH1F("h_cellId","", 1000, 0,5000e6);
  m_histograms.push_back(h_cellId);

  h_ene_eta = new TH1F("h_ene_eta","", 400, -2.0,2.0);
  m_histograms.push_back(h_ene_eta);

  h_ene_phi = new TH1F("h_ene_phi","", 628, -3.1416 ,3.1416);
  m_histograms.push_back(h_ene_phi);

  h_ene_r = new TH1F("h_ene_r","", 10, 2700.,3400.);
  m_histograms.push_back(h_ene_r);

  h_ene_eta_check = new TH1F("h_ene_eta_check","", 400, -2.0,2.0);
  m_histograms.push_back(h_ene_eta_check);

  h_ene_phi_check = new TH1F("h_ene_phi_check","", 628, -3.1416 ,3.1416);
  m_histograms.push_back(h_ene_phi_check);

  h_ene_r_check = new TH1F("h_ene_r_check","", 10, 2700.,3400.);
  m_histograms.push_back(h_ene_r_check);
}

void CellAnalysis::processEvent(podio::EventStore& aStore, int aEventId, bool verbose) {
  //Get the collections
  const fcc::CaloHitCollection*     colECalCell(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits_new(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits_old(nullptr);

  bool colECalCellOK     = aStore.get("caloCells" , colECalCell);
  bool colECalPositionedHits_newOK     = aStore.get("caloCellsPositions" , colECalPositionedHits_new);
  bool colECalPositionedHits_oldOK     = aStore.get("ECalPositionedHits" , colECalPositionedHits_old);

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
	h_cellId->Fill(iecl->core().cellId);
      }
    //Fill histograms
    //h_cellEnergy->Fill(SumE_cell);

  }
  else {
    if (verbose) {
      std::cout << "No CaloHit Collection!!!!!" << std::endl;
    }
  }

  std::cout << "Total energy 1: " << SumE_cell << std::endl;


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
	//if (verbose) std::cout << " x " << iecl->position().x << " y " << iecl->position().y << std::endl;
	    TVector3 vec(iecl->position().x,iecl->position().y,iecl->position().z);
	    double phi = atan2( iecl->position().y, iecl->position().x );
	    double eta = vec.Eta();
	  //if (verbose && iecl->core().energy>2.) std::cout << " eta " << eta << " phi " << phi << " energy " << iecl->core().energy << std::endl;
	    h_ene_r->Fill(r,iecl->core().energy);
	    h_ene_phi->Fill(phi,iecl->core().energy);
	    h_ene_eta->Fill(eta,iecl->core().energy);
	    //	  }
      }

    std::cout << "Total energy 2: " << SumE_cell << std::endl;
    //Fill histograms
    h_cellEnergy->Fill(SumE_cell);
  }
  else {
    if (verbose) {
      std::cout << "No colECalPositionedHits_new Collection!!!!!" << std::endl;
    }
  }


  double SumE_cell_check = 0.;
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
	h_ene_r_check->Fill(r,iecl->core().energy*m_sf);
	h_ene_phi_check->Fill(phi,iecl->core().energy*m_sf);
	h_ene_eta_check->Fill(eta,iecl->core().energy*m_sf);
      }
    std::cout << "Total energy 3: " << SumE_cell_check*m_sf << std::endl;
    h_cellEnergy_check->Fill(SumE_cell_check*m_sf);
  }
  else {
    if (verbose) {
      std::cout << "No colECalPositionedHits_old Collection!!!!!" << std::endl;
    }
  }

}

void CellAnalysis::finishLoop(int aNumEvents, bool aVerbose) {
  std::cout << "Total energy: " << h_cellEnergy->GetMean() << " check " <<  h_cellEnergy_check->GetMean() << std::endl;

  h_ene_r->Scale(1./(float)aNumEvents);
  h_ene_phi->Scale(1./(float)aNumEvents);
  h_ene_eta->Scale(1./(float)aNumEvents);

  h_ene_r_check->Scale(1./(float)aNumEvents);
  h_ene_phi_check->Scale(1./(float)aNumEvents);
  h_ene_eta_check->Scale(1./(float)aNumEvents);

  std::cout << "Integral phi " << h_ene_phi->Integral() << " check " << h_ene_phi_check->Integral() << std::endl;
}
