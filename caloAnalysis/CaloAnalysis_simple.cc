#include "CaloAnalysis_simple.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
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
#include <bitset>


CaloAnalysis_simple::CaloAnalysis_simple(const double sf, const double ENE, const TString particle) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  PARTICLE=particle;
  ENERGY = ENE;

  //Histograms initialization
  histClass = new HistogramClass(SF, ENERGY, PARTICLE);
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
  const fcc::CaloClusterCollection*     colECalCluster(nullptr);
 
  bool colMCParticlesOK = store.get("GenParticles", colMCParticles);
  bool colECalClusterOK     = store.get("ECalClusters" , colECalCluster);
 
  //Total hit energy per event
  SumE_hit_ecal = 0.;
  
  //Cluster collection
  if (colECalClusterOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalClusters:     " << colECalCluster->size()    << std::endl;;
    }
    //Loop through the collection
    for (auto& iecl=colECalCluster->begin(); iecl!=colECalCluster->end(); ++iecl) 
        {
          //if (verbose) std::cout << "ECal hit energy " << iehit->Core().Energy << std::endl;
          SumE_hit_ecal += iecl->Core().Energy;
	}

    if (verbose) std::cout << "Total hit energy (GeV): " << SumE_hit_ecal/GeV << " total cell energy (GeV): " << SumE_hit_ecal*SF/GeV << " hit collection size: " << colECalCluster->size() << std::endl;

    //Fill histograms
    histClass->h_hitEnergy->Fill(SumE_hit_ecal/GeV);
    histClass->h_cellEnergy->Fill(SumE_hit_ecal*SF/GeV);

  }
  else {
    if (verbose) {
      std::cout << "No CaloCluster Collection!!!!!" << std::endl;
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
      histClass->h_ptGen->Fill( sqrt( pow(iparticle->Core().P4.Px,2)+
				      pow(iparticle->Core().P4.Py,2) ) );
      //      std::cout << std::atan2(iparticle.StartVertex.Position.Y, iparticle.StartVertex.Position.X) << std::endl;
    }
    
  }
  else {
    if (verbose) {
      std::cout << "No MCTruth info available" << std::endl;
    }
  }

}
