#Setup ROOT
from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_profiles, gStyle, TCanvas, TFile, TF1, gPad, TMath
#import draw functions
from draw_functions import draw_1histogram, draw_2histograms

ENERGY = 50
SF=5.4
filename="../../FCCSW/output_ecalSim_e"+str(ENERGY)+"GeV_eta0_10events.root"
#filename="root://eospublic.cern.ch//eos/fcc/users/n/novaj/newgeometry_4nov/output_e"+str(ENERGY)+"GeV_bfield0_part1_Lar4mm_Pb2mm_tracker_v2.root"

print "Processing file ",filename
ma = CaloAnalysis_profiles(SF, ENERGY)
ma.loop(filename)
print "Mean hit energy: ", ma.histClass.h_hitEnergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.histClass.h_hitEnergy.GetMean())

  #Longo-Sestili formula 
  #http://arxiv.org/pdf/hep-ex/0001020.pdf
fit = TF1("fit","[0]*(pow([2]*x,[1]-1)*[2]*exp(-[2]*(x))/TMath::Gamma([1]))")
fit.SetParName(0,"A") #normalization factor
fit.SetParName(1,"#alpha")
fit.SetParName(2,"#beta")

gStyle.SetOptStat("emr");

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(3,2)
c1.cd(1)
draw_1histogram(ma.histClass.h_ptGen, "p_{T}^{gen} [GeV]","")
c1.cd(2)
draw_1histogram(ma.histClass.h_pdgGen, "PDG code","")
c1.cd(4)
ma.histClass.h_cellEnergy.Rebin(2)
draw_1histogram(ma.histClass.h_cellEnergy, "Total cell energy [GeV]","")
if (ma.histClass.h_cellEnergy.GetEntries()>50):
    ma.histClass.h_cellEnergy.Fit("gaus")
c1.cd(5)
draw_2histograms(ma.histClass.h_longProfile_particle, ma.histClass.h_longProfile, "Longitudinal distance/X0", "Energy [GeV]", "Particle dir.", "Hits in 1st layer")
fit.SetParameters(100, 8.15/1.15,1.0/1.15);
ma.histClass.h_longProfile.Fit("fit")
gPad.SetLogy(1)
c1.cd(6)
draw_2histograms(ma.histClass.h_radialProfile_particle, ma.histClass.h_radialProfile, "Radial distance/X0", "Energy [GeV]", "Particle dir.", "Hits in 1st layer")

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
