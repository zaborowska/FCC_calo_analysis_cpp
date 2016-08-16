#include "CaloAnalysis_cell.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloHitCollection.h"

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


void CaloAnalysis_cell::processEvent(podio::EventStore& store, bool verbose,
				podio::ROOTReader& reader) {

  //Get the collections
  const fcc::CaloHitCollection*     colECalCell(nullptr);
 
  bool colECalCellOK     = store.get("caloCells" , colECalCell);

  std::vector<unsigned> cellID;
  bool find_id = false;
 
  //Total hit energy per event
  SumE_cell = 0.;
  
  //Hit collection
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
	  find_id = false;
	  for (auto iter_id = cellID.begin(); iter_id!=cellID.end(); iter_id++) {
	    if (iecl->Core().Cellid == (*iter_id)) {
	      find_id = true;
	      break;
	    }
	  }
	  if (!find_id) {
	    cellID.push_back(iecl->Core().Cellid);
	  }
	}

    if (verbose) {
      std::cout << "Total energy in ECAL (EMSCALE, GeV): " << SumE_cell/GeV << " hit collection size: " << colECalCell->size() << std::endl;
      std::cout << "Number of different cellIDs: " << cellID.size() << std::endl;
    }

    //Fill histograms
    histClass->h_cellEnergy->Fill(SumE_cell/GeV);

  }
  else {
    if (verbose) {
      std::cout << "No CaloHit Collection!!!!!" << std::endl;
    }
  }


}
