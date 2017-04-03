from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

ENERGY = 10
SF=1
filename="../../FCCSW/output_ecalSim_e50GeV_10events.root"
#filename="root://eospublic.cern.ch//eos/fcc/users/n/novaj/newgeometry_4nov/output_e"+str(ENERGY)+"GeV_bfield0_part1_Lar4mm_Pb2mm_tracker_v2.root"

print "Processing file ",filename
ma = CaloAnalysis_simple(SF, ENERGY)
ma.loop(filename)
print "Mean hit energy: ", ma.histClass.h_hitEnergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.histClass.h_hitEnergy.GetMean())

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
draw_1histogram(ma.histClass.h_hitEnergy,"hit level energy [GeV]","") 
c1.cd(2)
draw_1histogram(ma.histClass.h_cellEnergy,"cell level energy [GeV]","") 
ma.histClass.h_cellEnergy.Rebin(2)
ma.histClass.h_cellEnergy.Fit("gaus")
c1.cd(3)
draw_1histogram(ma.histClass.h_ptGen,"Generated pt [GeV]","") 

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
