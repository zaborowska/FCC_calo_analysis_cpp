#include "CaloAnalysis.h"

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


CaloAnalysis::CaloAnalysis(const double sf, const double ENE, const std::string particle) 
{

  TH1::AddDirectory(kFALSE);

  SF = sf;
  PARTICLE=particle;
  ENERGY = ENE;

  hitenergy = new TH1F("hitenergy","hitenergy", 200, 0, ENERGY);
  if (PARTICLE=="e") {
    cellenergy = new TH1F("cellenergy","cellenergy", 100, ENERGY-0.2*ENERGY, ENERGY+0.2*ENERGY);
  }
  else {
    if (PARTICLE=="mu") cellenergy = new TH1F("cellenergy","cellenergy", 1000, 0, ENERGY-0.8*ENERGY);
    else std::cout << "WARNING!!! Undefined particle type!!!" <<std::endl;
  }
  hitphi = new TH1F("hitphi","hitphi", 64, -TMath::Pi(), TMath::Pi());
  //deltaphi = new TH1F("deltaphi","deltaphi", 50, 0, 0.5);
  //Muons:
  deltaphi = new TH1F("deltaphi","deltaphi", 100, 0, 0.5);
  phi_out = new TH1F("phi_out","phi_out",64, -TMath::Pi(), TMath::Pi());
  phi_outliers = new TH1F("phi_outliers","phi_outliers",64, -TMath::Pi(), TMath::Pi());
  x_outliers = new TH1F("x_outliers","x_outliers",100, -3500, 3500);
  y_outliers = new TH1F("y_outliers","y_outliers",100, -3500, 3500);
  z_outliers = new TH1F("z_outliers","z_outliers",100, -8000, 8000);
  r_outliers = new TH1F("r_outliers","r_outliers",100, 2600, 3500);

  phi_e = new TH1F("phi_e","phi_e",64, -TMath::Pi(), TMath::Pi());
  x_e = new TH1F("x_e","x_e",100, -3500, 3500);
  y_e = new TH1F("y_e","y_e",100, -3500, 3500);
  z_e = new TH1F("z_e","z_e",100, -8000, 8000);
  r_e = new TH1F("r_e","r_e",100, 2600, 3500);

  phi_max = new TH1F("phi_max","phi_max",64, -TMath::Pi(), TMath::Pi());
  x_max = new TH1F("x_max","x_max",100, -3500, 3500);
  y_max = new TH1F("y_max","y_max",100, -3500, 3500);
  r_max = new TH1F("r_max","r_max",100, 2600, 3500);

  //primary particle
  phi_prim = new TH1F("phi_prim","phi_prim",64, -TMath::Pi(), TMath::Pi());
  p_prim = new TH1F("p_prim","p_prim",100, 0, ENERGY*1.1);
  pt_prim = new TH1F("pt_prim","pt_prim",100, 0, ENERGY*1.1);

  phi_prim_out = new TH1F("phi_prim_out","phi_prim_out",64, -TMath::Pi(), TMath::Pi());

  diff_phimaxhit_phi_prim = new TH1F("diff_phimaxhit_phi_prim","diff_phimaxhit_prim",64, -TMath::Pi(), TMath::Pi());

  //bremstralung gamma
  phi_brem = new TH1F("phi_brem","phi_brem",64, -TMath::Pi(), TMath::Pi());
  p_brem = new TH1F("p_brem","p_brem",100, 0, ENERGY*1.1);
  pt_brem = new TH1F("pt_brem","pt_brem",100, 0, ENERGY*1.1);
  r_brem = new TH1F("r_brem","r_brem",100, 0, 3500);

  phi_brem_out = new TH1F("phi_brem_out","phi_brem_out",64, -TMath::Pi(), TMath::Pi());
  p_brem_out = new TH1F("p_brem_out","p_brem_out",100, 0, ENERGY*1.1);
  pt_brem_out = new TH1F("pt_brem_out","pt_brem_out",100, 0, ENERGY*1.1);
  r_brem_out = new TH1F("r_brem_out","r_brem_out",100, 0, 3500);

  h_ene_diff = new TH1F("h_ene_diff","h_ene_diff",100,-100,100);

  //nlayers = (rmax-rmin)/dr;

  nlayers = 20;
  dr = (rmax-rmin)/(double)nlayers;
  longprofile = new TH1F("longprofile","longprofile",nlayers,rmin,rmax);
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    if (PARTICLE=="e") {
      h_layers[ii] = new TH1F(Form("h_%d",ii),Form("h_%d",ii),100,0,ENERGY);
    }
    else {
      if (PARTICLE=="mu") h_layers[ii] = new TH1F(Form("h_%d",ii),Form("h_%d",ii),1000, 0, ENERGY-0.8*ENERGY);
      else std::cout << "WARNING!!! Undefined particle type!!!" <<std::endl;
    }
  }


}


CaloAnalysis::~CaloAnalysis() {
  
  delete hitenergy;
  delete cellenergy;
  delete longprofile;
  //  delete[] h_layers;
  /*
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    delete h_layers;
  }
  */
  delete hitphi;
  delete deltaphi;
  delete phi_out;
  delete phi_outliers;
  delete phi_max;
  delete y_max;
  delete x_max;
  delete r_max;
  delete x_outliers;
  delete y_outliers;
  delete z_outliers;
  delete r_outliers;

  delete phi_prim;
  delete pt_prim;
  delete p_prim;
  delete phi_prim_out;
  delete diff_phimaxhit_phi_prim;

  delete phi_brem;
  delete pt_brem;
  delete p_brem;
  delete r_brem;
  delete phi_brem_out;
  delete pt_brem_out;
  delete p_brem_out;
  delete r_brem_out;

  delete h_ene_diff;
  delete phi_e;
  delete x_e;
  delete y_e;
  delete z_e;
  delete r_e;
  
}

  

void CaloAnalysis::loop(const std::string filename) {
  hitenergy->Reset();
  cellenergy->Reset();
  hitphi->Reset();
  deltaphi->Reset();
  longprofile->Reset();
  for (unsigned int ii = 0; ii<NLAYERS_MAX; ii++) {
    h_layers[ii]->Reset();
    h_layers[ii]->Sumw2();
  }
  phi_max->Reset();
  x_max->Reset();
  y_max->Reset();
  r_max->Reset();
  phi_out->Reset();
  phi_outliers->Reset();
  x_outliers->Reset();
  y_outliers->Reset();
  z_outliers->Reset();
  r_outliers->Reset();

  phi_prim->Reset();
  p_prim->Reset();
  pt_prim->Reset();
  phi_prim_out->Reset();
  diff_phimaxhit_phi_prim->Reset();
  phi_brem->Reset();
  p_brem->Reset();
  pt_brem->Reset();
  r_brem->Reset();
  phi_brem_out->Reset();
  p_brem_out->Reset();
  pt_brem_out->Reset();
  r_brem_out->Reset();
  h_ene_diff->Reset();

  hitenergy->Sumw2();
  cellenergy->Sumw2();
  hitphi->Sumw2();
  deltaphi->Sumw2();
  longprofile->Sumw2();
  phi_max->Sumw2();
  x_max->Sumw2();
  y_max->Sumw2();
  r_max->Sumw2();
  phi_out->Sumw2();
  phi_outliers->Sumw2();
  x_outliers->Sumw2();
  y_outliers->Sumw2();
  z_outliers->Sumw2();
  r_outliers->Sumw2();

  phi_prim->Sumw2();
  p_prim->Sumw2();
  pt_prim->Sumw2();
  phi_prim_out->Sumw2();
  diff_phimaxhit_phi_prim->Sumw2();
  phi_brem->Sumw2();
  p_brem->Sumw2();
  pt_brem->Sumw2();
  r_brem->Sumw2();
  phi_brem_out->Sumw2();
  p_brem_out->Sumw2();
  pt_brem_out->Sumw2();
  r_brem_out->Sumw2();
  h_ene_diff->Sumw2();

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
    std::cout << "CaloAnalysis opening file " << filename << std::endl;
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
     /*
     longprofile->SetBinContent(ii+1, h_layers[ii]->GetMean());
     longprofile->SetBinError(ii+1, h_layers[ii]->GetMeanError());
     */
     longprofile->SetBinContent(ii+1, mean);
     longprofile->SetBinError(ii+1, mean_err);
     
     /*
     if (verbose) {
       std::cout << "layer rho # " << ii << " from " << rmin+ii*dr << " to " << rmin+(ii+1)*dr << " bins " << longprofile->GetBinLowEdge(ii+1) << " to " <<  longprofile->GetBinLowEdge(ii+1)+ longprofile->GetBinWidth(ii+1)<< std::endl;
     }
     */
   }

  return;
}


void CaloAnalysis::processEvent(podio::EventStore& store, bool verbose,
				podio::ROOTReader& reader) {


  phi_e->Reset();
  x_e->Reset();
  y_e->Reset();
  z_e->Reset();
  r_e->Reset();

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
 
  bool colMCParticlesOK = store.get("MCTruthParticles", colMCParticles);
  bool colGenVertexOK =store.get("GenVertices", colGenVertex);
  
  bool colECalClusterOK = store.get("ECalClusters" , colECalCluster);
  bool colECalHitOK     = store.get("ECalHits" , colECalHit);

  SumE_hit_ecal = 0.;
  for (unsigned int ii = 0; ii<nlayers; ii++) {
    SumE_layer[ii] = 0.;
  }

  const double ECUT = ENERGY*0.98;
  int layer;
  double hit_energy;
  double x_hit, y_hit, z_hit, phi_hit;
  double rho;
  const double r_epsilon = 0.0001;
  int nbin_max = 0;
  double phi_bin_max_center = -100;
  
  bool is_out = false;

 if (colECalClusterOK && colECalHitOK) {
   if (verbose) {
     std::cout << " Collections: "          << std::endl;
     std::cout << " -> #ECalClusters:     " << colECalCluster->size()    << std::endl;;
   }
    //  std::cout << std::endl;
    //  std::cout << "ECalClusters: " << std::endl;
     
   if (colECalCluster->size() != colECalHit->size() ) std::cout << "WARNING!!!! Hit Collection is of different size than Cluster Collection " << std:: endl;

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

	  x_hit = iecluster->Core().position.X;
	  y_hit = iecluster->Core().position.Y;
	  z_hit = iecluster->Core().position.Z;
	  phi_hit = atan2(y_hit,x_hit);
	  hitphi->Fill(phi_hit);
	  phi_e->Fill(phi_hit,hit_energy); //energy distribution in phi per event 
	  x_e->Fill(x_hit,hit_energy);
	  y_e->Fill(y_hit,hit_energy);
	  z_e->Fill(z_hit,hit_energy);
	  r_e->Fill(sqrt(pow(x_hit,2)+pow(y_hit,2)),hit_energy);
	}

   if (verbose) std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalHit->size() << std::endl;

   hitenergy->Fill(SumE_hit_ecal/GeV);

   /*
   if ( (fabs(phi_e->GetMean()-1.57)<0.2) || (fabs(phi_e->GetMean()+1.57)<0.2) ) {
     std::cout << phi_e->GetMean() << " "<<fabs(phi_e->GetMean()-1.57)<< " " <<fabs(phi_e->GetMean()+1.57) <<std::endl;
     cellenergy->Fill(SumE_hit_ecal/GeV);
   }
   */
   cellenergy->Fill(SumE_hit_ecal*SF/GeV);
   for (unsigned int ii = 0; ii<nlayers; ii++) {
     h_layers[ii]->Fill(SumE_layer[ii]*SF/GeV);
   }

   is_out = false;

   if (SumE_hit_ecal*SF/GeV<ECUT) {
  
     is_out = true;
     if (phi_out->GetEntries()==0) phi_out = (TH1F *)phi_e->Clone("phi_out");
     nbin_max = phi_e->GetMaximumBin();
     // std::cout << "Total hit energy: " << SumE_hit_ecal << " hit collection size: " << colECalHit->size() << " Maximum bin in phi " << nbin_max << " from " << phi_outliers->GetBinLowEdge(nbin_max) << " to " <<  phi_outliers->GetBinLowEdge(nbin_max)+ phi_outliers->GetBinWidth(nbin_max)<< std::endl;
     phi_outliers->SetBinContent(nbin_max,1+phi_outliers->GetBinContent(nbin_max));
     nbin_max = x_e->GetMaximumBin();
     x_outliers->SetBinContent(nbin_max,1+x_outliers->GetBinContent(nbin_max));
     nbin_max = y_e->GetMaximumBin();
     y_outliers->SetBinContent(nbin_max,1+y_outliers->GetBinContent(nbin_max));
     nbin_max = z_e->GetMaximumBin();
     z_outliers->SetBinContent(nbin_max,1+z_outliers->GetBinContent(nbin_max));
     nbin_max = r_e->GetMaximumBin();
     r_outliers->SetBinContent(nbin_max,1+r_outliers->GetBinContent(nbin_max));
     // std::cout << "Entries phi_e " << phi_e->GetEntries() << " phi_out " << phi_out->GetEntries() << std::endl;
   
   }
   deltaphi->Fill(phi_e->GetRMS());
   nbin_max = phi_e->GetMaximumBin();
   phi_bin_max_center = phi_e->GetBinCenter(nbin_max);
   if (verbose) std::cout << "phi_max " << phi_e->GetBinCenter(nbin_max) << std::endl;
   phi_max->SetBinContent(nbin_max,1+phi_max->GetBinContent(nbin_max));
   nbin_max = x_e->GetMaximumBin();
   x_max->SetBinContent(nbin_max,1+x_max->GetBinContent(nbin_max));
   nbin_max = y_e->GetMaximumBin();
   y_max->SetBinContent(nbin_max,1+y_max->GetBinContent(nbin_max));
   nbin_max = r_e->GetMaximumBin();
   r_max->SetBinContent(nbin_max,1+r_max->GetBinContent(nbin_max));
 }

 
 /* 
 if (colMCParticlesOK) {
   if (verbose) {
     std::cout << "-> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;


   }
 }
 
 if (colGenVertexOK) {
   if (verbose) {
   std::cout <<"-> #GenVertices:     " << colGenVertex->size()    << std::endl;
   }
 }
 */
 

 
 if (colMCParticlesOK && colGenVertexOK) {
   if (verbose) {
     std::cout << " Collections: "          << std::endl;
     std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
     std::cout << " -> #GenVertices:     " << colGenVertex->size()    << std::endl;
    }

   double ene_diff = 0;
   double ene_diff_x = 0;
   double ene_diff_y = 0;
   double ene_diff_z = 0;
   bool is_brem = false;

   for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
     double momentum = sqrt( pow(iparticle->Core().P4.Px,2)+
			     pow(iparticle->Core().P4.Py,2)+
			     pow(iparticle->Core().P4.Pz,2) );
     double momentum_pt = sqrt( pow(iparticle->Core().P4.Px,2)+
			     pow(iparticle->Core().P4.Py,2) );
     double phi_particle = atan2(iparticle->Core().P4.Py,iparticle->Core().P4.Px);
     double r_vertex = sqrt( pow(iparticle->Core().Vertex.X,2)+
			     pow(iparticle->Core().Vertex.Y,2) );
     double r_startVertex = sqrt( pow(iparticle->StartVertex().Position().X,2)+
				  pow(iparticle->StartVertex().Position().Y,2) );
     double r_endVertex = sqrt( pow(iparticle->EndVertex().Position().X,2)+
				pow(iparticle->EndVertex().Position().Y,2) );

  
     //primary particle
     if (iparticle->Core().Status==1) {
       phi_prim->Fill(phi_particle);
       p_prim->Fill(momentum);
       pt_prim->Fill(momentum_pt);
       if (is_out) {
	 std::cout << "=== Energy " << SumE_hit_ecal*SF/GeV  << " phi " << phi_particle << std::endl; 
	 phi_prim_out->Fill(phi_particle);
       }
       diff_phimaxhit_phi_prim->Fill(phi_particle- phi_bin_max_center);
     }

     if (iparticle->Core().Status==1) {
       ene_diff_x += iparticle->Core().P4.Px;
       ene_diff_y += iparticle->Core().P4.Py;
       ene_diff_z += iparticle->Core().P4.Pz;
     }
     if ( (iparticle->Core().Status==10) || (iparticle->Core().Status==20) ) {
       ene_diff_x -= iparticle->Core().P4.Px;
       ene_diff_y -= iparticle->Core().P4.Py;
       ene_diff_z -= iparticle->Core().P4.Pz;
     }


     //bremstralung gamma
     if ( fabs(iparticle->Core().Type) == 22) {
       is_brem = true;
       phi_brem->Fill(phi_particle);
       p_brem->Fill(momentum);
       pt_brem->Fill(momentum_pt);
       r_brem->Fill(r_startVertex);
       if (is_out){
	 phi_brem_out->Fill(phi_particle);
	 p_brem_out->Fill(momentum);
	 pt_brem_out->Fill(momentum_pt);
	 r_brem_out->Fill(r_startVertex);
       }
     }


     if (is_out) {
       std::cout << "Particle PDG " << iparticle->Core().Type
		 << " P " << momentum
		 << " r " << r_vertex
		 << " z "<< iparticle->Core().Vertex.Z
		 << " StartVertex r " << r_startVertex
		 << " z " << iparticle->StartVertex().Position().Z
		 << " EndVertex r " << r_endVertex
		 << " z " << iparticle->EndVertex().Position().Z
		 << std::endl; 	      
     }

   }
   ene_diff = sqrt(pow(ene_diff_x,2)+pow(ene_diff_y,2)+pow(ene_diff_z,2));
   if (is_brem) h_ene_diff->Fill(ene_diff);
 }
 else {
   if (verbose) {
     std::cout << "No MCTruth info available" << std:: endl;
   }
 }
 

}

unsigned CaloAnalysis::createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void CaloAnalysis::truncated_mean(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose)
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

void CaloAnalysis::gaussian_fit(TH1F*  histo, double truncation, double &mean, double &mean_err, bool verbose)
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
