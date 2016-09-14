from ROOT import gSystem
gSystem.Load("libcaloanalysis-myanalysis")
from ROOT import CaloAnalysis_cell, TCanvas, TFile, TF1, gPad

PARTICLE = "e"
ENERGY = 50

#SF not used in the code for cells - calibration by SF done in FCCSW 
SF=1.0
##filename="../../FCCSW/output_reco_ecal.root"  
filename="../../FCCSW/output_recoPhiEta_noise_test.root"  

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
ma.histClass.h_cellEnergy_check.Rebin(2)
ma.histClass.h_cellEnergy.Draw()
ma.histClass.h_cellEnergy.Fit("gaus")
ma.histClass.h_cellEnergy.SetLineColor(2)
ma.histClass.h_cellEnergy_check.Draw("same")
c1.cd(2)
ma.histClass.h_ene_r.Draw()
ma.histClass.h_ene_r_check.Draw("same")
ma.histClass.h_ene_r.SetLineColor(2)
print "Original: r bins: ", ma.histClass.h_ene_r_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_r_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_r_check.GetBinContent( ma.histClass.h_ene_r_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_r_check.Integral()
print "New: r bins: ", ma.histClass.h_ene_r.GetNbinsX(), " underflow ", ma.histClass.h_ene_r.GetBinContent(0), " overflow ", ma.histClass.h_ene_r.GetBinContent( ma.histClass.h_ene_r.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_r.Integral()
c1.cd(3)
ma.histClass.h_ene_phi.Draw()
ma.histClass.h_ene_phi_check.Draw("same")
ma.histClass.h_ene_phi.SetLineColor(2)
gPad.SetLogy(1)
print "Original: phi bins: ", ma.histClass.h_ene_phi_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_phi_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_phi_check.GetBinContent( ma.histClass.h_ene_phi_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_phi_check.Integral()
print "New: phi bins: ", ma.histClass.h_ene_phi.GetNbinsX(), " underflow ", ma.histClass.h_ene_phi.GetBinContent(0), " overflow ", ma.histClass.h_ene_phi.GetBinContent( ma.histClass.h_ene_phi.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_phi.Integral()
c1.cd(4)
ma.histClass.h_ene_eta.Draw()
ma.histClass.h_ene_eta_check.Draw("same")
ma.histClass.h_ene_eta.SetLineColor(2)
gPad.SetLogy(1)
print "Original: eta bins: ", ma.histClass.h_ene_eta_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_eta_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_eta_check.GetBinContent( ma.histClass.h_ene_eta_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_eta_check.Integral()
print "New: eta bins: ", ma.histClass.h_ene_eta.GetNbinsX(), " underflow ", ma.histClass.h_ene_eta.GetBinContent(0), " overflow ", ma.histClass.h_ene_eta.GetBinContent( ma.histClass.h_ene_eta.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_eta.Integral()

closeInput = raw_input("Press ENTER to exit") 
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
