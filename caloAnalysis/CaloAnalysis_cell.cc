#include "CaloAnalysis_cell.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloHitCollection.h"
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
#include "TVector3.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>


CaloAnalysis_cell::CaloAnalysis_cell(const double sf, const double ENE, const TString particle) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  PARTICLE=particle;
  ENERGY = ENE;

  //Histograms initialization
  histClass = new HistogramClass_cell(SF, ENERGY, PARTICLE);
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
  const fcc::CaloClusterCollection*     colECalCluster_new(nullptr);
  const fcc::CaloClusterCollection*     colECalCluster_old(nullptr);
 
  bool colECalCellOK     = store.get("caloCells" , colECalCell);
  bool colECalCluster_newOK     = store.get("newCaloClusters" , colECalCluster_new);
  bool colECalCluster_oldOK     = store.get("ECalClusters" , colECalCluster_old);

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
	//if (verbose) std::cout << "ECal cell energy " << iecl->Core().Energy << std::endl;
	SumE_cell += iecl->Core().Energy;
	histClass->h_cellId->Fill(iecl->Core().Cellid);
      }
    //Fill histograms
    //histClass->h_cellEnergy->Fill(SumE_cell/GeV);
    
  }
  else {
    if (verbose) {
      std::cout << "No CaloHit Collection!!!!!" << std::endl;
    }
  }

  std::cout << "Total energy 1: " << SumE_cell/GeV << std::endl;


  SumE_cell = 0.;
  //Cell collection
  if (colECalCluster_newOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #newCaloClusters:     " << colECalCluster_new->size()    << std::endl;;
    }
    for (auto& iecl=colECalCluster_new->begin(); iecl!=colECalCluster_new->end(); ++iecl) 
      {

	  //if (verbose) std::cout << "ECal cell energy " << iecl->Core().Energy << std::endl;
	  SumE_cell += iecl->Core().Energy;
	  if (iecl->Core().Energy>0.0001) {
	    double r = sqrt(pow(iecl->Core().position.X,2)+pow(iecl->Core().position.Y,2));
	//if (verbose) std::cout << " x " << iecl->Core().position.X << " y " << iecl->Core().position.Y << std::endl;
	    TVector3 vec(iecl->Core().position.X,iecl->Core().position.Y,iecl->Core().position.Z);
	    double phi = atan2( iecl->Core().position.Y, iecl->Core().position.X );
	    double eta = vec.Eta();
	  //if (verbose && iecl->Core().Energy>2.) std::cout << " eta " << eta << " phi " << phi << " energy " << iecl->Core().Energy << std::endl;
	    histClass->h_ene_r->Fill(r,iecl->Core().Energy);
	    histClass->h_ene_phi->Fill(phi,iecl->Core().Energy);
	    histClass->h_ene_eta->Fill(eta,iecl->Core().Energy);
	  }
      }
  
    std::cout << "Total energy 2: " << SumE_cell/GeV << std::endl;
    //Fill histograms
    histClass->h_cellEnergy->Fill(SumE_cell/GeV);
  }
  else {
    if (verbose) {
      std::cout << "No colECalCluster_new Collection!!!!!" << std::endl;
    }
  }


  double SumE_cell_check = 0.;
  double SF = 5.4;
  //Cell collection
  double n_noDouble = 0;
  int n_clusters = 0;
  if (colECalCluster_oldOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #oldCaloClusters:     " << colECalCluster_old->size()    << std::endl;;
    }
    n_noDouble = (float)colECalCluster_old->size()/2.;
    for (auto& iecl=colECalCluster_old->begin(); iecl!=colECalCluster_old->end(); ++iecl) 
      {
	n_clusters += 1;
	if (n_clusters>n_noDouble) break;
	SumE_cell_check += iecl->Core().Energy;
	double r = sqrt(pow(iecl->Core().position.X,2)+pow(iecl->Core().position.Y,2));
	TVector3 vec(iecl->Core().position.X,iecl->Core().position.Y,iecl->Core().position.Z);
	double phi = atan2( iecl->Core().position.Y, iecl->Core().position.X );
	double eta = vec.Eta();


	//if (verbose && iecl->Core().Energy>2.) std::cout << " eta " << eta << " phi " << phi << " energy " << iecl->Core().Energy << std::endl;

	histClass->h_ene_r_check->Fill(r,iecl->Core().Energy*SF);
	histClass->h_ene_phi_check->Fill(phi,iecl->Core().Energy*SF);
	histClass->h_ene_eta_check->Fill(eta,iecl->Core().Energy*SF);
      }
    std::cout << "Total energy 3: " << SumE_cell_check*SF/GeV << std::endl;
    histClass->h_cellEnergy_check->Fill(SumE_cell_check*SF/GeV);
  }
  else {
    if (verbose) {
      std::cout << "No colECalCluster_old Collection!!!!!" << std::endl;
    }
  }
 

}
