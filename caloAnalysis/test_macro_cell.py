from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_cell, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 50

#SF not used in the code for cells - calibration by SF done in FCCSW 
SF=1.0
filename="../../FCCSW/output_reco.root"    

print "Processing file ",filename
ma = CaloAnalysis_cell(SF, ENERGY, PARTICLE)
ma.loop(filename)
print "Mean cell energy: ", ma.histClass.h_cellEnergy.GetMean()
print "Mean cell Id: ", ma.histClass.h_cellId.GetMean()
print "Cell Id underflow: ",ma.histClass.h_cellId.GetBinContent(0)
print "Cell Id overflow: ",ma.histClass.h_cellId.GetBinContent(ma.histClass.h_cellId.GetNbinsX()+1)

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
ma.histClass.h_cellEnergy.Rebin(2)
ma.histClass.h_cellEnergy.Draw()
ma.histClass.h_cellEnergy.Fit("gaus")
c1.cd(2)
ma.histClass.h_cellId.Draw()
gPad.SetLogy(1)

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
