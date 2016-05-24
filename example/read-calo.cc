#include "datamodel/EventInfoCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// Utility functions
//#include "utilities/VectorUtils.h"
//#include "utilities/ParticleUtils.h"

// ROOT
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLorentzVector.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

const double GeV = 1000.;

unsigned createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void processEvent(podio::EventStore& store, bool verbose,
                  podio::ROOTReader& reader, bool do_ecal_hits, bool do_hcal_hits, std::vector<TH1F*> h_histo, TH1F *h_layers[104]) {

  // read event information
  const fcc::EventInfoCollection* evinfocoll(nullptr);
  bool evinfo_available = store.get("EventInfo", evinfocoll);
  if(evinfo_available) {
    auto evinfo = evinfocoll->at(0);

    // if(verbose)
      std::cout << "event number " << evinfo.Number() << std::endl;
  }

  //  const fcc::MCParticleCollection*  colGenParticles(nullptr);
  const fcc::CaloClusterCollection* colECalCluster(nullptr);
  const fcc::CaloHitCollection*     colECalHit(nullptr);
  const fcc::CaloClusterCollection* colHCalCluster(nullptr);
  const fcc::CaloHitCollection*     colHCalHit(nullptr);  

  //  bool colGenParticlesOK     = store.get("allGenParticles" , colGenParticles);
  bool colECalClusterOK     = store.get("ECalClusters" , colECalCluster);
  bool colECalHitOK     = store.get("ECalHits" , colECalHit);
  bool colHCalClusterOK     = store.get("HCalClusters" , colHCalCluster);
  bool colHCalHitOK     = store.get("HCalHits" , colHCalHit);

  //const double sf_ecal = 6.11; //no B field
  const double sf_ecal = 5.73; //with B field

  const double rmin = 2700.;
  const double rmax = 3400.;
  const double dr = 4.24+2.44;
  const double r_epsilon = 0.00001;
  const unsigned int n_layers = (rmax-rmin)/dr;

  unsigned int layer = 0;
  unsigned int layer_fromcellid = 0;

  double rho = 0.;
  double SumE_hit_ecal = 0.;
  double SumE_hit_hcal = 0.;
  double SumE_layer[n_layers];

  TH1F *h_phi_e = new TH1F("h_phi_e","h_phi_e",64, -TMath::Pi(), TMath::Pi());
  h_phi_e->Reset();
  double phi_hit = 0;
  unsigned int nbin_max = 0;

  for (unsigned int ii = 0; ii<n_layers; ii++) {
    SumE_layer[ii] = 0.;
  }

  //std::cout << "Number of layers " << n_layers << std::endl;

  //  if (colECalClusterOK && colECalHitOK && colHCalClusterOK && colHCalHitOK) {
  if (colECalClusterOK && colECalHitOK) {
    /*
    std::cout << " Collections: "          << std::endl;
    std::cout << " -> #GenParticles:     " << colGenParticles->size()    << std::endl;
    std::cout << " -> #ECalClusters:     " << colECalCluster->size()    << std::endl;
    std::cout << " -> #ECalHits:     " << colECalHit->size()    << std::endl;
    std::cout << " -> #HCalClusters:     " << colHCalCluster->size()    << std::endl;
    std::cout << " -> #HCalHits:     " << colHCalHit->size()    << std::endl;
    */

    //  std::cout << std::endl;
    //  std::cout << "ECalClusters: " << std::endl;
     
    if (do_ecal_hits) {
      if (colECalCluster->size() != colECalHit->size() ) std::cout << "WARNING!!!! Hit Collection is of different size than Cluster Collection " << std:: endl;

      auto& iehit=colECalHit->begin();
      for (auto& iecluster=colECalCluster->begin(); iecluster!=colECalCluster->end(); ++iecluster) 
	{
	  //std::cout << "ECal cluster energy " << iecluster->Core().Energy << std::endl;
	  SumE_hit_ecal += iecluster->Core().Energy;

	  //In which layer is the hit?
	  rho = std::sqrt(pow(iecluster->Core().position.X,2) +pow(iecluster->Core().position.Y,2));
	  //	if ( (rho<2705)|| (rho>3395)) std::cout << "Cluster position X,Y,Z " << iecluster->Core().position.X << " , "<< iecluster->Core().position.Y << " , " << iecluster->Core().position.Z << " R " << rho << std::endl;
	  for (unsigned int i = 0; i <(n_layers-1); i++) {
	    if ((rho>(rmin+i*dr))&&(rho<=(rmin+r_epsilon+(i+1)*dr))) {
	      layer = i+1;
	      //std::cout << "layer " << i+1 << " rho " << rho << std::endl;
	      break;
	    }
	  }
	  phi_hit = atan2(iecluster->Core().position.Y,iecluster->Core().position.X);
	  h_histo.at(4)->Fill(phi_hit,iecluster->Core().Energy); 
	  //filled and reset in each event
	  h_phi_e->Fill(phi_hit,iecluster->Core().Energy); 

	  //Extract the layer number from CellID info
	  layer_fromcellid = iehit->Core().Cellid & 0xFFFFFFFFFFFF; //Look at lower 32 bits (each F for 4 bits)
	  layer_fromcellid = layer_fromcellid & createMask(6,12); // mask other bits then the ones between 6 and 12
	  layer_fromcellid = layer_fromcellid >> 6; //shift to the right by 6 to get rid of the lower 6 bits you don't care about
       
	  //For debugging only: bitset<16> write 16 bits in the 0s and 1s
	  // std::cout << std::bitset<16>((iehit->Core().Cellid & 0xFFFFFFFFFFFF)) << " "<<   std::bitset<16> (createMask(6,12)) << " "  << std::bitset<16>(layer_fromcellid) << " "<<layer_fromcellid << std::endl;
   
	  //	if (layer!= layer_fromcellid) std::cout << "WARNING!!! mismatch between the layers!!! " << " rho: " << rho << " layer from segmentation: " << layer << " layer from cellID: " << layer_fromcellid << std::endl;


	  SumE_layer[layer_fromcellid] += iecluster->Core().Energy;

	  ++iehit;
	}
    }
    
    if ((do_hcal_hits)&&(colHCalClusterOK && colHCalHitOK)) {
      for (auto& ihcluster=colHCalCluster->begin(); ihcluster!=colHCalCluster->end(); ++ihcluster) 
	{
	  //std::cout << "HCal cluster energy " << ihcluster->Core().Energy << std::endl;
	  SumE_hit_hcal += ihcluster->Core().Energy;
	}
    //   std::cout << "Total hit energy in ECAL: " << SumE_hit_ecal << std::endl;
    //    std::cout << "Total hit energy in HCAL: " << SumE_hit_hcal << std::endl;
    }

    if (do_ecal_hits) {
      //std::cout << SumE_hit_ecal << " " << colECalHit->size() << std::endl;
      h_histo.at(0)->Fill(SumE_hit_ecal/GeV);
      h_histo.at(1)->Fill(SumE_hit_hcal/GeV);
      h_histo.at(2)->Fill(SumE_hit_ecal/GeV*sf_ecal);
      nbin_max = h_phi_e->GetMaximumBin();
      h_histo.at(5)->SetBinContent(nbin_max,1+h_histo.at(4)->GetBinContent(nbin_max));
      for (unsigned int ii = 0; ii<n_layers; ii++) {
	h_layers[ii]->Fill(SumE_layer[ii]/GeV*sf_ecal);
      }
    }
    if (do_hcal_hits) {
      h_histo.at(1)->Fill(SumE_hit_hcal/GeV);
    }
  }
  else {
    
    if (!colECalClusterOK)         std::cout << "Missing ECalClusters collection"                 << std::endl;
    if (!colECalHitOK)         std::cout << "Missing ECalHits collection"                 << std::endl;
    if (!colHCalClusterOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
    if (!colHCalHitOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
  }

  delete h_phi_e;

}


int main(){

  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //reader.openFile("../FCCSW/output.root");
     reader.openFile("output.root");
    // reader.openFile("root://eospublic.cern.ch//eos/fcc/users/n/novaj/April22/hits_fccsw_ecal_bfield0_e100GeV_eta025_primvertexR2599.root");
    //reader.openFile("root://eosatlas//eos/atlas/user/n/novaj/FCC/March21/hits_fccsw_hcalOnly_bfield_more_e100.root"); //TODO: not working!!!
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting. Input file not found!!!"<<std::endl;
    exit(1);
  }

  bool do_ecal_hits = true;
  bool do_hcal_hits = false;

  store.setReader(&reader);

  bool verbose = true;

  // Open output file
  TFile* outFile = new TFile("output-calo-histo.root", "RECREATE");

  std::vector<TH1F*> h_histo;
  const double ENE_MAX = 100.;
  const double rmin = 2700;
  const double rmax = 3400;
  const double dr = 4.24+2.44;
  const unsigned int n_layers = (rmax-rmin)/dr;

  const double e_range_min = (ENE_MAX-0.2*ENE_MAX);
  const double e_range_max = (ENE_MAX+0.2*ENE_MAX);
  const double deltae = 0.5;
  const int Nbins = (int)((e_range_max-e_range_min)/deltae);

  TH1F *h_hit_energy_ecal = new TH1F("h_hit_energy_ecal","h_hit_energy_ecal",100,0.,ENE_MAX);
  TH1F *h_hit_energy_hcal = new TH1F("h_hit_energy_hcal","h_hit_energy_hcal",100,0.,ENE_MAX);
  TH1F *h_cell_energy_ecal = new TH1F("h_cell_energy_ecal","h_cell_energy_ecal",Nbins,e_range_min,e_range_max);
  //TH1F *h_cell_energy_ecal = new TH1F("h_cell_energy_ecal","h_cell_energy_ecal",100,0,e_range_max);
  TH1F *h_long_profile_ecal = new TH1F("h_long_profile_ecal","h_long_profile_ecal",n_layers,rmin,rmax);

  TH1F *h_phi_ehit = new TH1F("h_phi_ehit","h_phi_ehit",64, -TMath::Pi(), TMath::Pi());
  TH1F *h_phi_hitmax = new TH1F("h_phi_hitmax","h_phi_hitmax",64, -TMath::Pi(), TMath::Pi());

  TH1F *h_layers[n_layers];
   
  for (unsigned int ii = 0; ii<n_layers; ii++) {
    h_layers[ii] = new TH1F(Form("h_%d",ii),Form("h_%d",ii),100,0,e_range_max);
    h_layers[ii]->Clear();
  }

  h_histo.push_back(h_hit_energy_ecal);
  h_histo.push_back(h_hit_energy_hcal);
  h_histo.push_back(h_cell_energy_ecal);
  h_histo.push_back(h_long_profile_ecal);
  h_histo.push_back(h_phi_ehit);
  h_histo.push_back(h_phi_hitmax);

  for (unsigned int i = 0; i<h_histo.size(); i++) {
    h_histo.at(i)->Sumw2();
  }

  // unsigned nEvents = 5;
  unsigned nEvents = reader.getEntries();
  std::cout << "Number of events to be processed: " << nEvents <<std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%100==0) {
      std::cout<<"reading event "<<i<<std::endl;
    }
    if(i>10) {
      verbose = false;
    }
    processEvent(store, verbose, reader, do_ecal_hits, do_hcal_hits, h_histo, h_layers);
    store.clear();
    reader.endOfEvent();
  }

  double TotalE = 0;

  for (unsigned int ii = 0; ii<n_layers; ii++) { 
    h_long_profile_ecal->SetBinContent(ii+1, h_layers[ii]->GetMean());
    h_long_profile_ecal->SetBinError(ii+1, h_layers[ii]->GetMeanError());
    TotalE += h_layers[ii]->GetMean();
    //cout << "Low edge " << h_energy_profile->GetBinLowEdge(ii+1) << " up edge " <<  h_energy_profile->GetBinLowEdge(ii+1)+h_energy_profile->GetBinWidth(ii+1) << endl;
  }

  std::cout << "Total energy " << TotalE << std::endl;

  TCanvas *c = new TCanvas("c","c",1000,800);
  c->Divide(2,2);
  unsigned int NDIV = 4;
  for (unsigned int i = 0; i<NDIV; i++) {
    if (i<h_histo.size()) {
      c->cd(i+1);
      h_histo.at(i)->Draw();
      std::cout << "Histo # " << i << " mean: " <<  h_histo.at(i)->GetMean() << " rms: " << h_histo.at(i)->GetRMS();
      if ((i<2) && (h_histo.at(i)->GetMean()>0.00001)) std::cout << " 1/SF " << ENE_MAX/(h_histo.at(i)->GetMean()) << std::endl;
      else std::cout << std::endl;
    }
  }
  
  if (outFile) {

   for (unsigned int i = 0; i<h_histo.size();i++) {
     h_histo.at(i)->Write();
   }

    outFile->Close();
    delete outFile;
    outFile=nullptr;

  }


  return 0;

}
