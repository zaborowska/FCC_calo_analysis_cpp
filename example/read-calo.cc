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

unsigned createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void processEvent(podio::EventStore& store, bool verbose,
                  podio::ROOTReader& reader, std::vector<TH1F*> h_histo) {

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

  bool colGenParticlesOK     = store.get("allGenParticles" , colGenParticles);
  bool colECalClusterOK     = store.get("ECalClusters" , colECalCluster);
  bool colECalHitOK     = store.get("ECalHits" , colECalHit);
  bool colHCalClusterOK     = store.get("HCalClusters" , colHCalCluster);
  bool colHCalHitOK     = store.get("HCalHits" , colHCalHit);

  double SumE_hit_ecal = 0;
  double SumE_hit_hcal = 0;

  double rho = 0;
  double rmin = 2700.;
  double rmax = 3400.;
  double r_epsilon = 0.0001;
  double dr = 10.;
  unsigned int n_layers = (rmax-rmin)/dr;

  unsigned int layer = 0;
  unsigned int layer_fromcellid = 0;

  std::cout << "Number of layers " << n_layers << std::endl;

  if (colGenParticlesOK && colECalClusterOK && colECalHitOK && colHCalClusterOK && colHCalHitOK) {

    std::cout << " Collections: "          << std::endl;
    std::cout << " -> #GenParticles:     " << colGenParticles->size()    << std::endl;
    std::cout << " -> #ECalClusters:     " << colECalCluster->size()    << std::endl;
    std::cout << " -> #ECalHits:     " << colECalHit->size()    << std::endl;
    std::cout << " -> #HCalClusters:     " << colHCalCluster->size()    << std::endl;
    std::cout << " -> #HCalHits:     " << colHCalHit->size()    << std::endl;
    
    SumE_hit_ecal = 0;
    SumE_hit_hcal = 0;

    //  std::cout << std::endl;
    //  std::cout << "ECalClusters: " << std::endl;
     
    if (colECalCluster->size() != colECalHit->size() ) std::cout << "WARNING!!!! Hit Collection is of different size than Cluster Collection " << std:: endl;

    auto& iehit=colECalHit->begin();
    for (auto& iecluster=colECalCluster->begin(); iecluster!=colECalCluster->end(); ++iecluster) 
      {
	//std::cout << "ECal cluster energy " << iecluster->Core().Energy << std::endl;
	SumE_hit_ecal += iecluster->Core().Energy;

	//In which layer is the hit?
	rho = std::sqrt(pow(iecluster->Core().position.X,2) +pow(iecluster->Core().position.Y,2));
	if ( (rho<2705)|| (rho>3395)) std::cout << "Cluster position X,Y,Z " << iecluster->Core().position.X << " , "<< iecluster->Core().position.Y << " , " << iecluster->Core().position.Z << " R " << rho << std::endl;
	for (unsigned int i = 0; i <(n_layers-1); i++) {
	  if ((rho>(rmin+i*dr))&&(rho<=(rmin+r_epsilon+(i+1)*dr))) {
	    layer = i+1;
	    //std::cout << "layer " << i+1 << " rho " << rho << std::endl;
	    break;
	  }
	}
	//Extract the layer number from CellID info
	layer_fromcellid = iehit->Core().Cellid & 0xFFFFFFFFFFFF; //Look at lower 32 bits (each F for 4 bits)
	layer_fromcellid = layer_fromcellid & createMask(6,12); // mask other bits then the ones between 6 and 12
	layer_fromcellid = layer_fromcellid >> 6; //shift to the right by 6 to get rid of the lower 6 bits you don't care about
       
	//For debugging only: bitset<16> write 16 bits in the 0s and 1s
	// std::cout << std::bitset<16>((iehit->Core().Cellid & 0xFFFFFFFFFFFF)) << " "<<   std::bitset<16> (createMask(6,12)) << " "  << std::bitset<16>(layer_fromcellid) << " "<<layer_fromcellid << std::endl;
   
	if (layer!= layer_fromcellid) std::cout << "WARNING!!! mismatch between the layers!!! " << " rho: " << rho << " layer from segmentation: " << layer << " layer from cellID: " << layer_fromcellid << std::endl;

	++iehit;
      }
    
    for (auto& ihcluster=colHCalCluster->begin(); ihcluster!=colHCalCluster->end(); ++ihcluster) 
      {
	//std::cout << "HCal cluster energy " << ihcluster->Core().Energy << std::endl;
        SumE_hit_hcal += ihcluster->Core().Energy;
      }
    std::cout << "Total hit energy in ECAL: " << SumE_hit_ecal << std::endl;
    std::cout << "Total hit energy in HCAL: " << SumE_hit_hcal << std::endl;
    
    h_histo.at(0)->Fill(SumE_hit_ecal);
    h_histo.at(1)->Fill(SumE_hit_hcal);
    
  }

  else {
    
    if (!colGenParticlesOK)         std::cout << "Missing genParticles collection"                 << std::endl;
    
    if (!colECalClusterOK)         std::cout << "Missing ECalClusters collection"                 << std::endl;
    if (!colECalHitOK)         std::cout << "Missing ECalHits collection"                 << std::endl;
    if (!colHCalClusterOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
    if (!colHCalHitOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
  }
}


int main(){

  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    reader.openFile("output-calo.root");
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
  TH1F *h_hit_energy_ecal = new TH1F("h_hit_energy_ecal","h_hit_energy_ecal",100,0,5000);
  TH1F *h_hit_energy_hcal = new TH1F("h_hit_energy_hcal","h_hit_energy_hcal",100,0,500);
  h_histo.push_back(h_hit_energy_ecal);
  h_histo.push_back(h_hit_energy_hcal);

  // unsigned nEvents = 5;
  unsigned nEvents = reader.getEntries();
  std::cout << "Number of events to be processed: " << nEvents <<std::endl;
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0) {
      std::cout<<"reading event "<<i<<std::endl;
    }
    if(i>10) {
      verbose = false;
    }
    processEvent(store, verbose, reader, h_histo);
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
      std::cout << "Histo # " << i << " mean: " <<  h_histo.at(i)->GetMean() << std::endl;
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
