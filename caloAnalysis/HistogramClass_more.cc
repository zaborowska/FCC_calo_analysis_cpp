#include "HistogramClass_more.h"

#include "TString.h"

// STL
#include <vector>
#include <iostream>
#include <bitset>

HistogramClass_more::HistogramClass_more(const double sf, const double ENE, const TString particle)
{
  PARTICLE=particle;
  ENERGY = ENE;
}

HistogramClass_more::~HistogramClass_more()
{
}


void HistogramClass_more::Initialize_histos()
{


  h_hitEnergy = new TH1F("h_hitEnergy","h_hitEnergy", 2*nbins, 0, ENERGY/1e3);
  histVector.push_back(h_hitEnergy);
  h_totalHitEnergy = new TH1F("h_totalHitEnergy","h_totalHitEnergy", 2*nbins, 0, ENERGY);
  histVector.push_back(h_totalHitEnergy);

  const double ene_fraction = 0.2;
  double e_min = 0;
  double e_max = ENERGY+ene_fraction*ENERGY;
  if (PARTICLE=="e") {
    e_min = ENERGY-ene_fraction*ENERGY;
    e_max = ENERGY+ene_fraction*ENERGY;
  }
  if (PARTICLE=="mu") {
    e_min = 0;
    e_max = ENERGY-4*ene_fraction*ENERGY;
  }
  h_totalCellEnergy = new TH1F("h_totalCellenergy","h_totalCellEnergy", nbins, e_min, e_max);
  histVector.push_back(h_totalCellEnergy);

  h_phiDiff = new TH1F("h_phiDiff","h_phiDiff", 2*nbins_phi, 2*phi_min, 2*phi_max);
  histVector.push_back(h_phiDiff);

  h_longProfile = new TH1F("h_longProfile","h_longProfile",90 , rmin_calo, rmax_calo);
  histVector.push_back(h_longProfile);
  
  h_radialProfile = new TH1F("h_radialProfile","h_radialProfile", nbins, -0.5, 0.5);
  histVector.push_back(h_radialProfile);

  g_longProfile = new TGraphErrors();
  g_longProfile->SetMarkerStyle(25);
  g_radialProfile = new TGraphErrors();
  g_radialProfile->SetMarkerStyle(25);

  //Truth information
  h_ptGen = new TH1F("h_ptGen","h_ptGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_ptGen);
  h_pGen = new TH1F("h_pGen","h_pGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_pGen);
  h_EGen = new TH1F("h_EGen","h_EGen", nbins, 0., ENERGY+ene_fraction*ENERGY);
  histVector.push_back(h_EGen);
  h_phiGen = new TH1F("h_phiGen","h_phiGen", nbins_phi, phi_min, phi_max);
  histVector.push_back(h_phiGen);
  h_etaGen = new TH1F("h_etaGen","h_etaGen", nbins, eta_min, eta_max);
  histVector.push_back(h_etaGen);

  n_layers = h_longProfile->GetNbinsX();
  n_slices_phi = h_radialProfile->GetNbinsX();
 
  if (n_layers>NLAYERS_MAX) {
    n_layers = NLAYERS_MAX;
    std::cout << "WARNING: Too many layers required!! Maximum set to " << NLAYERS_MAX << std::endl;
  }
  if (n_slices_phi>NLAYERS_MAX) {
    n_slices_phi = NLAYERS_MAX;
    std::cout << "WARNING: Too many slices in phi required!! Maximum set to " << NLAYERS_MAX << std::endl;
  }

  TString name; 
  for (unsigned int i = 0; i<n_layers; i++) {
    //Total hit energy per layer
    name = Form("h_layer_e_%d",i);
    h_layer_e[i] = new TH1F(name, name, nbins, 0, ENERGY);
    histVector.push_back(h_layer_e[i]);
    //Hit rho (weighted by hit energy) - used to find mean rho per layer (instead of using the middle of the interval)
    name = Form("h_layer_rho_%d",i);
    h_layer_rho[i] = new TH1F(name, name, nbins, rmin_calo, rmax_calo);
    histVector.push_back(h_layer_rho[i]);
    //Hit phi (weighted by hit energy) - used to find mean phi per layer (instead of using the middle of the interval)
    name = Form("h_layer_phi_%d",i);
    h_layer_phi[i] = new TH1F(name, name, nbins, -0.5, 0.5);
    histVector.push_back(h_layer_phi[i]);
  }

  for (unsigned int i = 0; i<n_slices_phi; i++) {
    //Total hit energy per phi slice
    name = Form("h_slicePhi_e_%d",i);
    h_slicePhi_e[i] = new TH1F(name, name, nbins, 0, ENERGY);
    histVector.push_back(h_slicePhi_e[i]);
    //Hit phi (weighted by hit energy) - used to find mean phi per layer (instead of using the middle of the interval)
    name = Form("h_layerPhi_%d",i);
    h_slicePhi_phi[i] = new TH1F(name, name, nbins, -0.5, 0.5);
    histVector.push_back(h_slicePhi_phi[i]);
  }


}


void HistogramClass_more::Reset_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    (*iterator)->Reset();
    (*iterator)->Sumw2();
  }



} 

void HistogramClass_more::Delete_histos()
{

  for (auto iterator=histVector.begin(); iterator<histVector.end(); iterator++) {
    delete (*iterator);
  }
  
  histVector.clear();

  delete g_longProfile;
  delete g_radialProfile;

}
