from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_more, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 100
nevents = 1000

SF=5.36
#SF=6.67
filename="root://eospublic//eos/fcc/users/n/novaj/July7_Bfield/e"+str(ENERGY)+"_b1_LAr4mm_Lead2mm_nocryo.root"
#filename="/tmp/novaj/e10_b1_LAr4mm_Lead2mm_nocryo.root"
#filename="/tmp/novaj/e"+str(ENERGY)+"_b0_LAr3mm_Lead2mm_nocryo.root"
#filename="/tmp/novaj/e"+str(ENERGY)+"_b0_LAr3mm_Lead2mm_rangeCut100mikrons.root"

print "Processing file ",filename
ma = CaloAnalysis_more(SF, ENERGY, PARTICLE, nevents)
ma.loop(filename)
print "Mean total hit energy: ", ma.histClass.h_totalHitEnergy.GetMean()
print "1/SF calculated: ", ENERGY/(ma.histClass.h_totalHitEnergy.GetMean())
print "Total cell energy: ", ma.histClass.h_totalCellEnergy.GetMean()
print "Total energy (from long. profile): ", ma.histClass.h_longProfile.Integral()
print "Total energy (from rad. profile): ", ma.histClass.h_radialProfile.Integral()
print "phiDiff underflow: ", ma.histClass.h_phiDiff.GetBinContent(0)
nbinsX =  ma.histClass.h_phiDiff.GetNbinsX()
print "phiDiff overflow: ", ma.histClass.h_phiDiff.GetBinContent(nbinsX+1)
#print "Total energy (from graph long. profile): ", ma.histClass.g_longProfile.Integral()

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
#ma.histClass.h_hitEnergy.Draw()
ma.histClass.h_phiDiff.Draw()
gPad.SetLogy(1)
c1.cd(2)
ma.histClass.h_longProfile.Draw()
ma.histClass.g_longProfile.Draw("Psame")
#gPad.SetLogy(1)
c1.cd(3)
ma.histClass.h_totalCellEnergy.Rebin(2)
ma.histClass.h_totalCellEnergy.Draw()
ma.histClass.h_totalCellEnergy.Fit("gaus")
fit=ma.histClass.h_totalCellEnergy.GetFunction("gaus")
mean=fit.GetParameter(1)
sigma=fit.GetParameter(2)
ma.histClass.h_totalCellEnergy.Fit("gaus","","r",mean-2*sigma,mean+2*sigma)
#print "Fit resuls: mean ", mean, " sigma: ", sigma
c1.cd(4)
ma.histClass.h_radialProfile.Draw()
#ma.histClass.h_phiDiff.Draw()
#gPad.SetLogy(1)

gPad.Update()

#c2 = TCanvas("c2","c2",1000,1000)
#c2.Divide(2,2)
#c2.cd(1)
#ma.histClass.h_EGen.Draw()
#c2.cd(2)
#ma.histClass.h_pGen.Draw()
#c2.cd(3)
#ma.histClass.h_phiGen.Draw()
#c2.cd(4)
#ma.histClass.h_etaGen.Draw()

#gPad.Update()

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
