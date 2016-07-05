from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 50
SUFFIX = ""

#SF=5.36
#filename="../../FCCSW/output.root" 
SF=1.0
filename="../../FCCSW/output_calibHits.root" 

print "Processing file ",filename
ma = CaloAnalysis_simple(SF, ENERGY, PARTICLE)
ma.loop(filename)
print "Mean hit energy: ", ma.h_hitEnergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.h_hitEnergy.GetMean())

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
ma.h_hitEnergy.Draw()
c1.cd(2)
ma.h_cellEnergy.Draw()
ma.h_cellEnergy.Fit("gaus")
c1.cd(3)
ma.h_ptGen.Draw()

gPad.Update()

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".png")

