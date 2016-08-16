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
#include "TGraphErrors.h"
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

  n_layers = histClass->h_longProfile->GetNbinsX();
  layer_thickness = (rmax_calo-rmin_calo)/(double)n_layers;
  n_slices_phi = histClass->h_radialProfile->GetNbinsX();
  phi_slice_diff = (1.0)/(double)n_slices_phi;

  if (n_layers>NLAYERS_MAX) {
    n_layers = NLAYERS_MAX;
    layer_thickness = (rmax_calo-rmin_calo)/NLAYERS_MAX;
    std::cout << "WARNING: Too many layers required!! Maximum set to " << NLAYERS_MAX << std::endl;
  }
  if (n_slices_phi>NLAYERS_MAX) {
    n_slices_phi = NLAYERS_MAX;
    phi_slice_diff =  (2*TMath::Pi())/NLAYERS_MAX;
    std::cout << "WARNING: Too many slices in phi required!! Maximum set to " << NLAYERS_MAX << std::endl;
  }

  std::cout << "layer thickness " << layer_thickness <<  " n layers: " << n_layers << std::endl;
  std::cout << "phi slice diff " << phi_slice_diff <<  " phi slices: " << n_slices_phi << std::endl;

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
    //  std::cout <<"test " << histClass->h_longProfile_test->GetBinContent(3)*1./(double)(i+1)*SF/GeV << std::endl;

    store.clear();
    reader.endOfEvent();
  }

  std::cout << "Total hit energy in Ecal: " << histClass->h_totalHitEnergy->GetMean() << std::endl;
  std::cout << "Total energy in Ecal: " << histClass->h_totalCellEnergy->GetMean()<< std::endl;
  std::cout << "End of loop" << std::endl;

  double mean_layer_e[NLAYERS_MAX], mean_layer_e_err[NLAYERS_MAX];
  double mean_layer_rho[NLAYERS_MAX], mean_layer_rho_err[NLAYERS_MAX];


  double mean_phi_e[NLAYERS_MAX], mean_phi_e_err[NLAYERS_MAX];
  double mean_phi_phi[NLAYERS_MAX], mean_phi_phi_err[NLAYERS_MAX];

  for (unsigned int ii = 0; ii<n_slices_phi; ii++) {
    mean_phi_e[ii] = histClass->h_slicePhi_e[ii]->GetMean();
    mean_phi_e_err[ii] = histClass->h_slicePhi_e[ii]->GetMeanError();
    histClass->h_radialProfile->SetBinContent(ii+1, mean_phi_e[ii]);
    histClass->h_radialProfile->SetBinError(ii+1, mean_phi_e_err[ii]);
    mean_phi_phi[ii]=histClass->h_slicePhi_phi[ii]->GetMean();
    mean_phi_phi_err[ii]=histClass->h_slicePhi_phi[ii]->GetMeanError();
    //std::cout << "rho " << mean_layer_rho[ii] << " e " << mean_layer_e[ii] << std::endl;
  }

  for (unsigned int ii = 0; ii<n_layers; ii++) {
    mean_layer_e[ii] = histClass->h_layer_e[ii]->GetMean();
    mean_layer_e_err[ii] = histClass->h_layer_e[ii]->GetMeanError();
    histClass->h_longProfile->SetBinContent(ii+1, mean_layer_e[ii]);
    histClass->h_longProfile->SetBinError(ii+1, mean_layer_e_err[ii]);
    mean_layer_rho[ii]=histClass->h_layer_rho[ii]->GetMean();
    mean_layer_rho_err[ii]=histClass->h_layer_rho[ii]->GetMeanError();
    //std::cout << "rho " << mean_layer_rho[ii] << " e " << mean_layer_e[ii] << std::endl;
  }


  //delete histClass->g_longProfile;
  histClass->g_longProfile = new TGraphErrors(n_layers,mean_layer_rho, mean_layer_e, mean_layer_rho_err,mean_layer_e_err);
  histClass->g_longProfile->SetMarkerStyle(25);
  histClass->g_longProfile->SetMarkerSize(0.5);
  histClass->g_longProfile->GetXaxis()->SetRangeUser(rmin_calo, rmax_calo);

  double graph_e, graph_rho;
  for (unsigned int ii = 0; ii<n_layers; ii++) {
    histClass->g_longProfile->GetPoint(ii, graph_rho, graph_e);
    std::cout << "e " << histClass->h_longProfile->GetBinContent(ii+1) << " " << graph_e << " rho " << histClass->h_layer_rho[ii]->GetMean() << " phiDiff " << histClass->h_layer_phi[ii]->GetMean() << " rms " << histClass->h_layer_phi[ii]->GetRMS() << std::endl;
  }

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
      ptGen=sqrt( pow(iparticle->Core().P4.Px,2)+
		  pow(iparticle->Core().P4.Py,2) );
      pGen=sqrt( pow(iparticle->Core().P4.Px,2)+
		 pow(iparticle->Core().P4.Py,2)+
		 pow(iparticle->Core().P4.Pz,2) );
      EGen=sqrt( pow(iparticle->Core().P4.Px,2)+
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
  for (unsigned int i = 0; i<n_layers; i++) {
    SumE_layer[i] = 0.;
  }
  double eHit = 0;
  double phiHit=-100;
  //Cluster collection
  if (colECalClusterOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalCluster:     " << colECalCluster->size()    << std::endl;;
    }
    //Loop through the collection
    for (auto& iecl=colECalCluster->begin(); iecl!=colECalCluster->end(); ++iecl) {
      //if (verbose) std::cout << "ECal cluster energy " << iecluster->Core().Energy << std::endl;
      eHit=iecl->Core().Energy;
      SumE_hit_ecal += eHit;
      phiHit=atan2(iecl->Core().position.Y, iecl->Core().position.X);
      //Fill histograms for each hit
      histClass->h_hitEnergy->Fill(eHit);
      if (eHit>eHitMin) {
	double phiDiff = phiHit-phiGen;
	if (TMath::Abs(phiDiff)>TMath::Pi()) {
	  int sign = -1;
	  if (phiDiff<-TMath::Pi()) {
	    sign = 1;
	  }
	  phiDiff += sign*2*TMath::Pi();
	  std::cout << phiHit << " " << phiGen << " " << phiDiff << std::endl;
	}
	histClass->h_phiDiff->Fill(phiDiff);
	
	double slice_phi_helper = TMath::Abs(phiDiff)/phi_slice_diff;
	double sign = phiDiff/TMath::Abs(phiDiff);
	int slice_phi_i = n_slices_phi/2+sign*(int)slice_phi_helper;
	if (slice_phi_i>=n_slices_phi) {
	  //std::cout << phiDiff << " " <<phi_slice_diff<<" "<< slice_phi_helper << " " <<slice_phi_i << std::endl;
	  if (sign>0) slice_phi_i = n_slices_phi-1;
	  else slice_phi_i = 1;
	}
	histClass->h_slicePhi_e[slice_phi_i]->Fill(eHit*SF/GeV);
	histClass->h_slicePhi_phi[slice_phi_i]->Fill(phiDiff);
	
      }

      double rhoHit = std::sqrt(pow(iecl->Core().position.X,2) +pow(iecl->Core().position.Y,2));
      //floor: Rounds x downward, returning the largest integral value that is not greater than x
      unsigned int layer_i = std::floor((rhoHit-rmin_calo)/layer_thickness);

      //std::cout << "rho " << rho << " layer " << layer_i << std::endl;
      SumE_layer[layer_i] += eHit;
      histClass->h_layer_e[layer_i]->Fill(eHit*SF/GeV);
      histClass->h_layer_rho[layer_i]->Fill(rhoHit,eHit);
      histClass->h_layer_phi[layer_i]->Fill(phiHit-phiGen,eHit);
      
    }

    if (verbose) std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalCluster->size() << std::endl;

    //Fill histograms per event
    histClass->h_totalHitEnergy->Fill(SumE_hit_ecal/GeV);
    histClass->h_totalCellEnergy->Fill(SumE_hit_ecal*SF/GeV);
    /*
    for (unsigned int ii = 0; ii<n_layers; ii++) {
      histClass->h_layer_e[ii]->Fill(SumE_layer[ii]*SF/GeV);
    }
    */


  }
  else {
    if (verbose) {
      std::cout << "No CaloCluster Collection!!!!!" << std::endl;
    }
  }
 
}

