from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_more, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 250
nevents=20

SF=5.36
#filename="root://eospublic.cern.ch//eos/fcc/users/n/novaj/July7_Bfield/e10_b1_LAr4mm_Lead2mm_nocryo.root"
#filename="/tmp/novaj/e10_b1_LAr4mm_Lead2mm_nocryo.root"
filename="/tmp/novaj/e250_b0_LAr3mm_Lead2mm_nocryo.root"

print "Processing file ",filename
ma = CaloAnalysis_more(SF, ENERGY, PARTICLE, nevents)
ma.loop(filename)
print "Mean hit energy: ", ma.histClass.h_hitEnergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.histClass.h_hitEnergy.GetMean())

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
ma.histClass.h_hitEnergy.Draw()
c1.cd(2)
ma.histClass.h_cellEnergy.Rebin(2)
ma.histClass.h_cellEnergy.Draw()
#ma.histClass.h_cellEnergy.Fit("gaus")

gPad.Update()

c2 = TCanvas("c2","c2",1000,1000)
c2.Divide(2,2)
c2.cd(1)
ma.histClass.h_EGen.Draw()
c2.cd(2)
ma.histClass.h_pGen.Draw()
c2.cd(3)
ma.histClass.h_phiGen.Draw()
c2.cd(4)
ma.histClass.h_etaGen.Draw()

gPad.Update()

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
