#include "CaloAnalysis_more.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
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


CaloAnalysis_more::CaloAnalysis_more(const double sf, const double ENE, const TString particle, const int nevents) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  PARTICLE=particle;
  ENERGY = ENE;
  //If nevents<0 => Using all events in the sample
  NEVENTS = nevents;

  //Histograms initialization
  histClass = new HistogramClass_more(SF, ENERGY, PARTICLE);
  histClass->Initialize_histos();
}


CaloAnalysis_more::~CaloAnalysis_more() {

  histClass->Delete_histos();
  delete histClass;

}

  

void CaloAnalysis_more::loop(const std::string filename) {

  //Reset histograms
  histClass->Reset_histos();

  //Open file in the reader
  TString filename_eos;
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //filename_eos =  "root://eospublic.cern.ch//eos/fcc/users/n/novaj/June10_ecalShifted/"+filename;
    reader.openFile(filename);
    std::cout << "CaloAnalysis_more opening file " << filename << std::endl;
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  store.setReader(&reader);

  bool verbose = true;

  //Loop over events
  //If nevents<0 => Using all events in the sample
  unsigned nEvents = 0;
  if (NEVENTS<0) {
    nEvents = reader.getEntries();
  }
  else {
    nEvents = NEVENTS;
  }
  std::cout << "Number of events: " << nEvents << std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%100==0) std::cout<<"reading event "<<i<<std::endl;
    if(i>11) verbose = false;

    processEvent(store, verbose, reader);

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "Total energy: " << histClass->h_cellEnergy->GetMean() << std::endl;
  std::cout << "End of loop" << std::endl;

  return;
}


void CaloAnalysis_more::processEvent(podio::EventStore& store, bool verbose,
				podio::ROOTReader& reader) {


  //Get the collections
  const fcc::MCParticleCollection*  colMCParticles(nullptr);
  const fcc::CaloClusterCollection*     colECalCluster(nullptr);
 
  bool colMCParticlesOK = store.get("GenParticles", colMCParticles);
 
  bool colECalClusterOK     = store.get("ECalClusters" , colECalCluster);

  double ptGen, pGen, EGen;
  double phiGen, etaGen;
  phiGen=-100;
  //MCParticle and Vertices collection 
  if (colMCParticlesOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
    }
    //Loop through the collection   
    for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
      ptGen =  sqrt( pow(iparticle->Core().P4.Px,2)+
		     pow(iparticle->Core().P4.Py,2) );
      pGen =  sqrt( pow(iparticle->Core().P4.Px,2)+
		    pow(iparticle->Core().P4.Py,2)+
		    pow(iparticle->Core().P4.Pz,2) );
      EGen =  sqrt( pow(iparticle->Core().P4.Px,2)+
		    pow(iparticle->Core().P4.Py,2)+
		    pow(iparticle->Core().P4.Pz,2)+
		    pow(iparticle->Core().P4.Mass,2) );
      phiGen=atan2( iparticle->Core().P4.Py, iparticle->Core().P4.Px );
      etaGen=0.5*log((EGen+iparticle->Core().P4.Pz)/(EGen-iparticle->Core().P4.Pz));

      //Fill histograms
      histClass->h_ptGen->Fill(ptGen);
      histClass->h_pGen->Fill(pGen);
      histClass->h_EGen->Fill(EGen);
      histClass->h_phiGen->Fill(phiGen);
      histClass->h_etaGen->Fill(etaGen);

    } 
  }
  else {
    if (verbose) {
      std::cout << "No MCTruth info available" << std::endl;
    }
  }

  //Total hit energy per event
  SumE_hit_ecal = 0.;  
  //Cluster collection
  if (colECalClusterOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalCluster:     " << colECalCluster->size()    << std::endl;;
    }
    //Loop through the collection
    for (auto& iecl=colECalCluster->begin(); iecl!=colECalCluster->end(); ++iecl)         {
          //if (verbose) std::cout << "ECal cluster energy " << iecluster->Core().Energy << std::endl;
          SumE_hit_ecal += iecl->Core().Energy;
	}

    if (verbose) std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalCluster->size() << std::endl;

    //Fill histograms
    histClass->h_hitEnergy->Fill(SumE_hit_ecal/GeV);
    histClass->h_cellEnergy->Fill(SumE_hit_ecal*SF/GeV);

  }
  else {
    if (verbose) {
      std::cout << "No CaloCluster Collection!!!!!" << std::endl;
    }
  }
 
}

