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

    // if(verbose)
      std::cout << "event number " << evinfo.Number() << std::endl;
  }

  //  const fcc::MCParticleCollection*  colGenParticles(nullptr);
  const fcc::CaloClusterCollection* colHCalCluster(nullptr);
  const fcc::CaloHitCollection*     colHCalHit(nullptr);  

  //  bool colGenParticlesOK     = store.get("allGenParticles" , colGenParticles);
  bool colHCalClusterOK     = store.get("HCalClusters" , colHCalCluster);
  bool colHCalHitOK     = store.get("HCalHits" , colHCalHit);

  const double GeV = 1000.;

  double SumE_hit_hcal = 0.;
  double rho;

  //std::cout << "Number of layers " << n_layers << std::endl;

  if (colHCalClusterOK && colHCalHitOK) {

    /*
    std::cout << " Collections: "          << std::endl;
    std::cout << " -> #HCalClusters:     " << colHCalCluster->size()    << std::endl;
    std::cout << " -> #HCalHits:     " << colHCalHit->size()    << std::endl;
    */

    for (auto& ihcluster=colHCalCluster->begin(); ihcluster!=colHCalCluster->end(); ++ihcluster) 
      {
	//std::cout << "HCal cluster energy " << ihcluster->Core().Energy << std::endl;
	SumE_hit_hcal += ihcluster->Core().Energy;
	rho = sqrt(std::pow(ihcluster->Core().position.X,2)+std::pow(ihcluster->Core().position.Y,2));
	h_histo.at(1)->Fill(rho);
	h_histo.at(2)->Fill(rho, ihcluster->Core().Energy/GeV);
      }
    //    std::cout << "Total hit energy in HCAL: " << SumE_hit_hcal << std::endl;
    h_histo.at(0)->Fill(SumE_hit_hcal/GeV);    
  }

  else {
    
    if (!colHCalClusterOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
    if (!colHCalHitOK)         std::cout << "Missing HCalHits collection"                 << std::endl;
  }
}


int main(){

  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  try {
    //reader.openFile("../FCCSW/output.root");
    reader.openFile("output.root");
    //reader.openFile("calohits_fccsw_e100.root");
    //reader.openFile("/tmp/novaj/hits_fccsw_calo_e100.root");
    //reader.openFile("/tmp/novaj/hits_fccsw_hcalOnly_bfield_more_e100.root");
    //reader.openFile("root://eosatlas.cern.ch//eos/atlas/user/n/novaj/FCC/March21/hits_fccsw_hcalOnly_e100.root"); //TODO: not working!!!
  }
  catch(std::runtime_error& err) {
    std::cerr<<err.what()<<". Quitting. Input file not found!!!"<<std::endl;
    exit(1);
  }

  store.setReader(&reader);

  bool verbose = true;

  // Open output file
  TFile* outFile = new TFile("output-hcal-histo.root", "RECREATE");

  std::vector<TH1F*> h_histo;
  TH1F *h_hit_energy_hcal = new TH1F("h_hit_energy_hcal","h_hit_energy_hcal",100,0.,100);
  TH1F *h_hit_rho = new TH1F("h_hit_rho","h_hit_rho",70,0.,7000);
  TH1F *h_ene_rho = new TH1F("h_ene_rho","h_ene_rho",25,3500.,6000);

  h_histo.push_back(h_hit_energy_hcal);
  h_histo.push_back(h_hit_rho);
  h_histo.push_back(h_ene_rho);

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
    processEvent(store, verbose, reader, h_histo);
    store.clear();
    reader.endOfEvent();
  }
 
  unsigned int ii = 0;
  std::cout << "Total hit energy mean: " <<  h_histo.at(ii)->GetMean() << " rms: " << h_histo.at(ii)->GetRMS() << " 1/SF " << 100./(h_histo.at(ii)->GetMean()) << std::endl;
  ii = 1;
  std::cout << "Hit position in rho mean: " <<  h_histo.at(ii)->GetMean() << " rms: " << h_histo.at(ii)->GetRMS() << std::endl;
  ii = 2;
  h_histo.at(ii)->Scale(1./(double)nEvents);
  std::cout << "Energy per rho slices mean: " <<  h_histo.at(ii)->GetMean() << " rms: " << h_histo.at(ii)->GetRMS() << std::endl;

  if (outFile) {
    for (unsigned int i = 0; i<h_histo.size(); i++) {
      h_histo.at(i)->Write();
    }
    outFile->Close();
    delete outFile;
    outFile=nullptr;

  }

  return 0;

}
