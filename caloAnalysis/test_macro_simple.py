from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 50

SF=5.4
#filename="../../FCCSW/output_e50_10events.root" 
filename="../../FCCSW/output_ecal.root"

print "Processing file ",filename
ma = CaloAnalysis_simple(SF, ENERGY, PARTICLE)
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
ma.histClass.h_cellEnergy.Fit("gaus")
c1.cd(3)
ma.histClass.h_ptGen.Draw()

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
