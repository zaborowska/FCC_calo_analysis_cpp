#include "CaloAnalysis_simple.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/EventInfoCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// Utility functions
//#include "utilities/VectorUtils.h"
//#include "utilities/ParticleUtils.h"

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


CaloAnalysis_simple::CaloAnalysis_simple(const double sf, const double ENE, const std::string particle) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  PARTICLE=particle;
  ENERGY = ENE;

  h_hitEnergy = new TH1F("h_hitEnergy","h_hitEnergy", 200, 0, ENERGY);
  
  if (PARTICLE=="e") {
    h_cellEnergy = new TH1F("h_cellenergy","h_cellEnergy", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  }
  else {
    if (PARTICLE=="mu") h_cellEnergy = new TH1F("h_cellEnergy","h_cellEnergy", 1000, 0, ENERGY-0.8*ENERGY);
    else std::cout << "WARNING!!! Undefined particle type!!!" <<std::endl;
  }

  h_ptGen = new TH1F("h_ptGen","h_ptGen", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  
}


CaloAnalysis_simple::~CaloAnalysis_simple() {

  delete h_hitEnergy;
  delete h_cellEnergy;
  delete h_ptGen;
 
}

  

void CaloAnalysis_simple::loop(const std::string filename) {

  h_hitEnergy->Reset();
  h_cellEnergy->Reset();
  h_ptGen->Reset();

  h_hitEnergy->Sumw2();
  h_cellEnergy->Sumw2();
  h_ptGen->Sumw2();
  

  double truncation = 0.01;
  double mean = 0.0;
  double mean_err = 0.0;

  std::string filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //reader.openFile(filename);
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

  //unsigned nEvents = 5;
  unsigned nEvents = reader.getEntries();
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) verbose = false;

    processEvent(store, verbose, reader);

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "End of loop" << std::endl;

  return;
}


void CaloAnalysis_simple::processEvent(podio::EventStore& store, bool verbose,
				podio::ROOTReader& reader) {


  // read event information
  const fcc::EventInfoCollection* evinfocoll(nullptr);
  bool evinfo_available = store.get("EventInfo", evinfocoll);
  if(evinfo_available) {
    auto evinfo = evinfocoll->at(0);

   if(verbose)
      std::cout << "event number " << evinfo.Number() << std::endl;
  }

  const fcc::MCParticleCollection*  colMCParticles(nullptr);
  const fcc::GenVertexCollection*  colGenVertex(nullptr);
  const fcc::CaloClusterCollection* colECalCluster(nullptr);
  const fcc::CaloHitCollection*     colECalHit(nullptr);
 
  bool colMCParticlesOK = store.get("GenParticles", colMCParticles);
  bool colGenVertexOK =store.get("GenVertices", colGenVertex);
  
  bool colECalClusterOK = store.get("ECalClusters" , colECalCluster);
  bool colECalHitOK     = store.get("ECalHits" , colECalHit);

  SumE_hit_ecal = 0.;
  
  if (colECalClusterOK && colECalHitOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalClusters:     " << colECalCluster->size()    << std::endl;;
    }
    //  std::cout << std::endl;
    //  std::cout << "ECalClusters: " << std::endl;
     
    auto& iehit=colECalHit->begin();
    for (auto& iecluster=colECalCluster->begin(); iecluster!=colECalCluster->end(); ++iecluster) 
        {
          //if (verbose) std::cout << "ECal cluster energy " << iecluster->Core().Energy << std::endl;
          SumE_hit_ecal += iecluster->Core().Energy;
	}

    if (verbose) std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalHit->size() << std::endl;

    h_hitEnergy->Fill(SumE_hit_ecal/GeV);
    h_cellEnergy->Fill(SumE_hit_ecal*SF/GeV);

  }
 
 
 if (colMCParticlesOK && colGenVertexOK) {
   if (verbose) {
     std::cout << " Collections: "          << std::endl;
     std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
     std::cout << " -> #GenVertices:     " << colGenVertex->size()    << std::endl;
    }
   
   for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
     h_ptGen->Fill( sqrt( pow(iparticle->Core().P4.Px,2)+
			  pow(iparticle->Core().P4.Py,2) ) );
   }
   
 }
 else {
   if (verbose) {
     std::cout << "No MCTruth info available" << std::endl;
   }
 }

}
