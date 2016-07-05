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

  nlayers = 20;
  dr = (rmax-rmin)/(double)nlayers;
  h_longProfile = new TH1F("h_longProfile","h_longProfile",nlayers,rmin,rmax);
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    if (PARTICLE=="e") {
      h_layers[ii] = new TH1F(Form("h_%d",ii),Form("h_%d",ii),100,0,ENERGY);
    }
    else {
      if (PARTICLE=="mu") h_layers[ii] = new TH1F(Form("h_%d",ii),Form("h_%d",ii),1000, 0, ENERGY-0.8*ENERGY);
      else std::cout << "WARNING!!! Undefined particle type!!!" <<std::endl;
    }
  }

  h_ptGen = new TH1F("h_ptGen","h_ptGen", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);

}


CaloAnalysis_simple::~CaloAnalysis_simple() {
  
  delete h_hitEnergy;
  delete h_cellEnergy;
  delete h_longProfile;
  delete h_ptGen;
  
  //  delete[] h_layers;
  /*
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    delete h_layers;
  }
  */

}

  

void CaloAnalysis_simple::loop(const std::string filename) {
  h_hitEnergy->Reset();
  h_cellEnergy->Reset();
  h_longProfile->Reset();
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    h_layers[ii]->Reset();
    h_layers[ii]->Sumw2();
  }
  h_ptGen->Reset();

  h_hitEnergy->Sumw2();
  h_cellEnergy->Sumw2();
  h_longProfile->Sumw2();
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

   for (unsigned int ii = 0; ii<nlayers; ii++) {
     if (PARTICLE=="e") {
       //gaussian fit possible only with high statistics, not appropriate for layers with almost no energy
       //gaussian_fit(h_layers[ii],truncation,mean,mean_err, verbose);
       mean = h_layers[ii]->GetMean();
       mean_err = h_layers[ii]->GetMeanError();
     }
     else {
       if (PARTICLE=="mu") truncated_mean(h_layers[ii],truncation,mean,mean_err, verbose);
       else std::cout << "WARNING!!! Undefined particle type!!!" <<std::endl;

     }
     h_longProfile->SetBinContent(ii+1, mean);
     h_longProfile->SetBinError(ii+1, mean_err);
     
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
  for (unsigned int ii = 0; ii<nlayers; ii++) {
    SumE_layer[ii] = 0.;
  }

  int layer;
  double hit_energy;
  double rho;
  const double r_epsilon = 0.0001;
  
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
	  hit_energy = iecluster->Core().Energy;
          SumE_hit_ecal += hit_energy;

	  /*
	  //Extract the layer number from CellID info
          layer = iehit->Core().Cellid & 0xFFFFFFFFFFFF; //Look at lower 32 bits (each F for 4 bits)
          layer = layer & createMask(6,12); // mask other bits then the ones between 6 and 12
          layer = layer >> 6; //shift to the right by 6 to get rid of the lower 6 bits you don't care about     
          //For debugging only: bitset<16> write 16 bits in the 0s and 1s
          // std::cout << std::bitset<16>((iehit->Core().Cellid & 0xFFFFFFFFFFFF)) << " "<<   std::bitset<16> (createMask(6,12)) << " "  << std::bitset<16>(layer) << " "<<layer << std::endl;
	  */
          //In which layer is the hit?
          rho = std::sqrt(pow(iecluster->Core().position.X,2) +pow(iecluster->Core().position.Y,2));
	  //floor: Rounds x downward, returning the largest integral value that is not greater than x
	  layer = std::floor((rho-rmin)/dr);
	  //std::cout << "Rho " << rho << " layer "  << layer << " rmin_layer " << rmin+dr*layer << " rmax_layer " <<  rmin+dr*(layer+1) << std::endl;
          SumE_layer[layer] += iecluster->Core().Energy;
          ++iehit;
	}

    if (verbose) std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalHit->size() << std::endl;

    h_hitEnergy->Fill(SumE_hit_ecal/GeV);
    h_cellEnergy->Fill(SumE_hit_ecal*SF/GeV);
    for (unsigned int ii = 0; ii<nlayers; ii++) {
      h_layers[ii]->Fill(SumE_layer[ii]*SF/GeV);
    }
    
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

unsigned CaloAnalysis_simple::createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void CaloAnalysis_simple::truncated_mean(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose)
{

  int index = 0;
  double entries_truncation = (double)histo->GetEntries() - (double)histo->GetEntries()*truncation;
  for (int j = 0; j<histo->GetNbinsX(); j++) {
    if (histo->Integral(0,j)>entries_truncation) {
      index = j;
      break;
    }
  }

  if (verbose) {
    std::cout << 	 "Entries outside truncated region : " << histo->Integral(index,(histo->GetNbinsX()+1)) << std::endl;
    std::cout <<  "Mean " << histo->GetMean() << " RMS " << histo->GetRMS() << std::endl;
  }
  histo->GetXaxis()->SetRange(0,index);
  if (verbose) {
    std::cout <<  "Truncated Mean " << histo->GetMean() << " RMS " << histo->GetRMS() << std::endl;
  }
  mean = histo->GetMean();
  mean_err = histo->GetMeanError();

}

void CaloAnalysis_simple::gaussian_fit(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose)
{

  TF1 *fit_gaus = new TF1("fit_gaus","gaus");

  histo->Fit("fit_gaus","Q");
  mean = fit_gaus->GetParameter(1);
  double sigma = fit_gaus->GetParameter(2);
  histo->Fit("fit_gaus","Q","",mean-2*sigma,mean+2*sigma);
  mean = fit_gaus->GetParameter(1);
  mean_err = fit_gaus->GetParError(1);

  std::cout << "Mean " << histo->GetMean() << " Fit "<< mean << std::endl;

}
