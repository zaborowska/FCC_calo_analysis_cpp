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
#include "TH2F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>
#include <string>
#include <sstream>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"




unsigned createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void processEvent(podio::EventStore& store, bool verbose,
                  podio::ROOTReader& reader, std::vector<TH1F*> h_histo, unsigned int i) {


  reader.goToEvent(i);

  // read event information
  const fcc::EventInfoCollection* evinfocoll(nullptr);
  bool evinfo_available = store.get("EventInfo", evinfocoll);
  if(evinfo_available) {
    auto evinfo = evinfocoll->at(0);

    if(verbose)
      std::cout << "event number " << evinfo.Number() << std::endl;
  }

  const fcc::MCParticleCollection*  colGenParticles(nullptr);
  const fcc::CaloClusterCollection* colECalCluster(nullptr);
  const fcc::CaloHitCollection*     colECalHit(nullptr);
  const fcc::CaloClusterCollection* colHCalCluster(nullptr);
  const fcc::CaloHitCollection*     colHCalHit(nullptr);  

  //bool colGenParticlesOK     = store.get("allGenParticles" , colGenParticles);
  bool colECalClusterOK     = store.get("ECalClusters" , colECalCluster);
  bool colECalHitOK     = store.get("ECalHits" , colECalHit);
  //bool colHCalClusterOK     = store.get("HCalClusters" , colHCalCluster);
  //bool colHCalHitOK     = store.get("HCalHits" , colHCalHit);

  double SumE_hit_lar = 0;
  double SumE_hit_lead = 0;
  double SumE_hit_ecal = 0;
  double SumE_hit_ecal_x0 [7];
  double SumE_hit_ecal_x0cut = 0;
  //double SumE_hit_hcal = 0;


  double rmin = 2700.;
  double rmax = 3400.;
  double r_epsilon = 0.0001;
  double dr = 3.5 + 2.5;
  unsigned int n_layers = (rmax-rmin)/dr;
  //double SumElayer[n_layers];

  unsigned int layer = 0;
  unsigned int layer_fromcellid = 0;

  //double sf = 4.79; //without bfield
  //double sf = 6.81; //with bfield
  //double x0 = 12.75;

  std::vector<double> sf_vec;
  std::vector<double> x0_vec;
  int index = 10;

  //6mm
  sf_vec.push_back(2.79); // [0] 5+1
  x0_vec.push_back(28.1);
  sf_vec.push_back(3.94); //[1] 4.5+1.5
  x0_vec.push_back(20.0);
  sf_vec.push_back(5.39); //[2] 4+2
  x0_vec.push_back(15.6);
  sf_vec.push_back(7.21); //[3] 3.5+2.5
  x0_vec.push_back(12.8);
  sf_vec.push_back(9.58); //[4] 3+3
  x0_vec.push_back(10.8);

  //12mm
  sf_vec.push_back(2.93); //[5] 10+2
  x0_vec.push_back(28.1);

  sf_vec.push_back(4.21); //[6] 9+3
  x0_vec.push_back(20.0);

  sf_vec.push_back(5.80); //[7] 8+4
  x0_vec.push_back(15.6);

  sf_vec.push_back(7.81); //[8] 7+5
  x0_vec.push_back(12.8);

  sf_vec.push_back(10.41); //[9] 6+6
  x0_vec.push_back(10.8);

  //6mm lar gap
  sf_vec.push_back(4.04); //[10] 6+2
  x0_vec.push_back(20.00); 

  sf_vec.push_back(7.229); //[11] 6+4
  x0_vec.push_back(13.23);
  
  sf_vec.push_back(10.41); //[12] 6+6
  x0_vec.push_back(10.48);


  float sf = sf_vec.at(index);
  float x0 = x0_vec.at(index);

  //double GeV = 1000.;
  

  bool use_cellID = false;

  double posn_x;
  double posn_y;
  double posn_z;
  double rho;
  double phi;
  unsigned nevents = reader.getEntries();
  //TH1F *h_temp = new TH1F("h_temp","h_temp",1000,-3.14,3.14);

  //double ene_rho[20];
  //for(int i=0; i<20; ++i){
  //  ene_rho[i]=0.0;
  //}

  //int nbin = 20;
  //h_histo.at(6)->Clear();

  //std::cout << "Number of layers " << n_layers << std::endl;

  
  
  
  if (colECalClusterOK && colECalHitOK) {

    int n = 0;
    SumE_hit_ecal = 0;
    for (int i=0; i<=6; ++i){
      SumE_hit_ecal_x0[i]=0.0;
	}
     
    if (colECalCluster->size() != colECalHit->size() ) std::cout << "WARNING!!!! Hit Collection is of different size than Cluster Collection " << std:: endl;
    auto& iehit=colECalHit->begin();
    
    for (auto& iecluster=colECalCluster->begin(); iecluster!=colECalCluster->end(); ++iecluster) {//each hit in here
      posn_x = iecluster->Core().position.X;
      posn_y = iecluster->Core().position.Y;
      posn_z = iecluster->Core().position.Z;

      	rho = sqrt(pow(posn_x,2)+pow(posn_y,2));
	//std::cout<<rho<<std::endl;


	//
	for (unsigned int i=0; i<(n_layers-1);i++){
	  if((rho>(rmin+i*dr))&&(rho<=(rmin+r_epsilon+(i+1)*dr))) {
	    layer = i+1;
	    //std::cout << "layer " << i+1 << " rho " << rho << std::endl;
	    break;
	  }
	}
	//
      
	layer_fromcellid = iehit->Core().Cellid & 0xFFFFFFFFFFFF;
	//Look at lower 32 bits (each F for 4 bits)
	layer_fromcellid = layer_fromcellid & createMask(6,15); 
	// mask other bits then the ones between 6 and 15
	//assuming 10 bits allocated for the layer in CellID

	layer_fromcellid = layer_fromcellid >> 6; //shift to the right by 6
	
	//For debugging only: bitset<16> write 16 bits in the 0s and 1s
	//std::cout << std::bitset<16>((iehit->Core().Cellid & 0xFFFFFFFFFFFF)) << " "<<   std::bitset<16> (createMask(6,15)) << " "  << std::bitset<16>(layer_fromcellid) << " "<<layer_fromcellid << std::endl;
   
	   
	//if (layer!= layer_fromcellid+1 && layer+116-1 != layer_fromcellid ) std::cout << "WARNING!!! mismatch between the layers!!! " << " rho: " << rho << " layer from segmentation: " << layer << " layer from cellID: " << layer_fromcellid << std::endl;
	//remember that the layers in the CellID start from 0
	//lead layers: 0 --> n_layers - 1
	//lar layers: n_layers --> ~2*n_layers
	//std::cout<<layer_fromcellid<<std::endl;

	if (use_cellID){

	  if (layer_fromcellid < n_layers){
	    SumE_hit_lead += iecluster->Core().Energy;
	  }

	  if (layer_fromcellid >= n_layers){
	    SumE_hit_lar += iecluster->Core().Energy;
	  }
	}

	if (!use_cellID){
	  SumE_hit_ecal += iecluster->Core().Energy;
	  
	  rho = rho - 2700;

	  if(rho < 20*x0){
	    SumE_hit_ecal_x0[0] += iecluster->Core().Energy;
	  }

	  if (rho < 25*x0){
	    SumE_hit_ecal_x0[1] += iecluster->Core().Energy;
	  }

	  if (rho < 30*x0){
    	    SumE_hit_ecal_x0[2] += iecluster->Core().Energy;
	  }

	  if (rho < 35*x0){
    	    SumE_hit_ecal_x0[3] += iecluster->Core().Energy;
	  }

	  if ( rho < 40*x0){
	    SumE_hit_ecal_x0[4] += iecluster->Core().Energy;
	  }

	  if ( rho < 45*x0){
	    SumE_hit_ecal_x0[5] += iecluster->Core().Energy;
	  }

	  if (rho < 50*x0){
	    SumE_hit_ecal_x0[6] += iecluster->Core().Energy;
	  }

	}

	h_histo.at(0)->Fill(iecluster->Core().Energy);
	//h_histo.at(#)->Fill(rho,iecluster->Core().Energy);
	//we have now summed up all the hits.
	//SumE_hit_ecal += iecluster->Core().Energy;
	//SumE_layer[layer_fromcellid] += iecluster->Core().Energy;
	++iehit;
	++n;
    }

    if (use_cellID){
      h_histo.at(1)->Fill(SumE_hit_ecal);
      h_histo.at(2)->Fill((SumE_hit_lar+SumE_hit_lead));
      //h_histo.at(#)->Fill(SumE_hit_lead);
      //h_histo.at(#)->Fill(SumE_hit_lar);
    }
    if (!use_cellID){
      h_histo.at(1)->Fill(SumE_hit_ecal);
      h_histo.at(2)->Fill(SumE_hit_ecal*sf);
      std::cout<<SumE_hit_ecal*sf<<std::endl;
      h_histo.at(3)->Fill(SumE_hit_ecal_x0[0]);
      h_histo.at(4)->Fill(SumE_hit_ecal_x0[1]);
      h_histo.at(5)->Fill(SumE_hit_ecal_x0[2]);
      h_histo.at(6)->Fill(SumE_hit_ecal_x0[3]);
      h_histo.at(7)->Fill(SumE_hit_ecal_x0[4]);
      h_histo.at(8)->Fill(SumE_hit_ecal_x0[5]);
      h_histo.at(9)->Fill(SumE_hit_ecal_x0[6]);
      
    }
    
    //std::cout<<n<<"  "<<SumE_hit_ecal<<std::endl;
  }


 

	


  else {
    
    if (!colECalClusterOK)         std::cout << "Missing ECalClusters collection"                 << std::endl;
    if (!colECalHitOK)         std::cout << "Missing ECalHits collection"                 << std::endl;
  }
  //delete h_temp;
}



int main(){

  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {

    //reader.openFile("root://eospublic//eos/fcc/users/b/broach/6mm/LAR3.5_LEAD2.5/no_cryo_fixed/e20_n500_b0.root");
    reader.openFile("/tmp/novaj/e500_LAr6mm_Lead2mm_nocryo.root");
    //reader.openFile("/afs/cern.ch/user/b/broach/FCC_calo_analysis_cpp/e50_lar4_lead2.root");
    //reader.openFile("/afs/cern.ch/work/b/broach/6mm/LAR4_LEAD2/no_cryo_fixed/e1000_n500_b0.root");
    //reader.openFile("/afs/cern.ch/work/b/broach/6mm/LAR4.5_LEAD1.5/no_cryo_fixed/e500_n500_b0.root");
    //reader.openFile("/tmp/broach/e20_n500_b0.root");
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting."<<std::endl;
    exit(1);
  }
  store.setReader(&reader);

  bool verbose = true;

  // Open output file
  TFile* outFile = new TFile("output-calo-histo.root", "RECREATE");


  std::vector<TH1F*> h_histo;

  TH1F *h_hit_energy_ecal = new TH1F("h_hit_energy_ecal","h_hit_energy_ecal",10000,0,1000);
  TH1F *h_res = new TH1F("h_res","h_res",15000,0,1500e3);
  TH1F *h_res_sf = new TH1F("h_res_sf","h_res_sf",15000,0,1500e3);
  //TH1F *h_res_lead = new TH1F("h_res_lead","h_res_lead",100,0,0);
  //TH1F *h_res_lar = new TH1F("h_res_lar","h_res_lar",100,0,0);
  TH1F *h_res_20x0 = new TH1F("h_res_20x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_25x0 = new TH1F("h_res_25x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_30x0 = new TH1F("h_res_30x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_35x0 = new TH1F("h_res_35x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_40x0 = new TH1F("h_res_40x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_45x0 = new TH1F("h_res_45x0","h_res_20x0",50000,0,1500e3);
  TH1F *h_res_50x0 = new TH1F("h_res_50x0","h_res_20x0",50000,0,1500e3);

 
  //TH1F *h_phi_maxhit_nocut = new TH1F("h_phi_maxhit_nocut","h_phi_maxhit_nocut",100,-3.14,3.14);
  //TH1F *h_phi_sigma = new TH1F("h_phi_sigma","h_phi_sigma",50,0,0.5);
  //TH1F *h_phi_sigma_cut = new TH1F("h_phi_sigma_cut","h_phi_sigma_cut",50,0,0.5);
  //TH1F *h_hit_energy_cut = new TH1F("h_hit_energy_cut","h_hit_energy_cut",50,0,20e3);
  //TH1F *h_n = new TH1F("h_n","h_n",50,0,50);

  //TH1F *h_hit_energy_hcal = new TH1F("h_hit_energy_hcal","h_hit_energy_hcal",100,0,500);
  h_histo.push_back(h_hit_energy_ecal);
  h_histo.push_back(h_res);
  h_histo.push_back(h_res_sf);
  h_histo.push_back(h_res_20x0);
  h_histo.push_back(h_res_25x0);
  h_histo.push_back(h_res_30x0);
  h_histo.push_back(h_res_35x0);  
  h_histo.push_back(h_res_40x0);
  h_histo.push_back(h_res_45x0); 
  h_histo.push_back(h_res_50x0);
  // h_histo.push_back(h_res_lead);
  //h_histo.push_back(h_res_lar);

 


  unsigned nEvents = reader.getEntries();
  //unsigned nEvents = 1;
  std::cout << "Number of events to be processed: " << nEvents <<std::endl;
  for(unsigned int i=0; i<499; ++i) {
    if (i%10 == 0){
      std::cout<<i<<std::endl;
    }
    //std::cout<<"reading event "<<i<<std::endl;
    //if(i>10) {
    //  verbose = false;
    // }
    processEvent(store, verbose, reader, h_histo, i);
    store.clear();
    reader.endOfEvent();
  }

  
  TCanvas *c = new TCanvas("c","c",1000,800);
  c->Divide(2,2);
  unsigned int NDIV = 4;
  for (unsigned int i = 0; i<NDIV; i++) {
    if (i<h_histo.size()) {
      c->cd(i+1);
      h_histo.at(i)->Draw();
    }
  }

  

  outFile->cd();
  if (outFile) {
    
   for (unsigned int i = 0; i<h_histo.size();i++) {
     h_histo.at(i)->Write();
   }
  }

  outFile->Close();
  delete outFile;
  outFile=nullptr;

  // outFile2->cd();
  // if (outFile2) {
  //   for (unsigned int j = 0; j<h_langaus.size();j++){
  //     h_langaus.at(j)->Write();
  //   }
  // }

  //  outFile2->Close();
  //  delete outFile2;
  // outFile2=nullptr;

  return 0;

}
  
