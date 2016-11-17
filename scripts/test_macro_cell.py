import calo_init
calo_init.parse_args()
calo_init.print_config()
from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import CaloAnalysis_cell, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

# use this script for only 1 file
energy = calo_init.energies[0]
filename = calo_init.filenames[0]
if len(calo_init.filenames) > 1:
    print "WARNING: analysis of the first input file, ignoring the rest"

ma = CaloAnalysis_cell(calo_init.sf, energy)
ma.loop(filename)
print "Mean cell energy: ", ma.histClass.h_cellEnergy.GetMean()
print "Mean cell Id: ", ma.histClass.h_cellId.GetMean()
print "Cell Id underflow: ",ma.histClass.h_cellId.GetBinContent(0)
print "Cell Id overflow: ",ma.histClass.h_cellId.GetBinContent(ma.histClass.h_cellId.GetNbinsX()+1)

c1 = TCanvas("c1","c1",1000,1000)
c1.Divide(2,2)
c1.cd(1)
draw_2histograms(ma.histClass.h_cellEnergy, ma.histClass.h_cellEnergy_check,"Energy per cell","", "FCCSW cells", "Hits in cells")
ma.histClass.h_cellEnergy.Rebin(2)
ma.histClass.h_cellEnergy_check.Rebin(2)
ma.histClass.h_cellEnergy.Fit("gaus")
c1.cd(2)
draw_2histograms(ma.histClass.h_ene_r, ma.histClass.h_ene_r_check,"rCell [mm]","Energy per bin [GeV]", "FCCSW cells", "Hits in cells")
gPad.SetLogy(1)
print "Original: r bins: ", ma.histClass.h_ene_r_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_r_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_r_check.GetBinContent( ma.histClass.h_ene_r_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_r_check.Integral()
print "New: r bins: ", ma.histClass.h_ene_r.GetNbinsX(), " underflow ", ma.histClass.h_ene_r.GetBinContent(0), " overflow ", ma.histClass.h_ene_r.GetBinContent( ma.histClass.h_ene_r.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_r.Integral()
c1.cd(3)
draw_2histograms(ma.histClass.h_ene_phi, ma.histClass.h_ene_phi_check,"#phi","Energy per bin [GeV]", "FCCSW cells", "Hits in cells")
gPad.SetLogy(1)
print "Original: phi bins: ", ma.histClass.h_ene_phi_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_phi_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_phi_check.GetBinContent( ma.histClass.h_ene_phi_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_phi_check.Integral()
print "New: phi bins: ", ma.histClass.h_ene_phi.GetNbinsX(), " underflow ", ma.histClass.h_ene_phi.GetBinContent(0), " overflow ", ma.histClass.h_ene_phi.GetBinContent( ma.histClass.h_ene_phi.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_phi.Integral()
c1.cd(4)
draw_2histograms(ma.histClass.h_ene_eta, ma.histClass.h_ene_eta_check,"#eta","Energy per bin [GeV]", "FCCSW cells", "Hits in cells")
gPad.SetLogy(1)
print "Original: eta bins: ", ma.histClass.h_ene_eta_check.GetNbinsX(), " underflow ", ma.histClass.h_ene_eta_check.GetBinContent(0), " overflow ", ma.histClass.h_ene_eta_check.GetBinContent( ma.histClass.h_ene_eta_check.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_eta_check.Integral()
print "New: eta bins: ", ma.histClass.h_ene_eta.GetNbinsX(), " underflow ", ma.histClass.h_ene_eta.GetBinContent(0), " overflow ", ma.histClass.h_ene_eta.GetBinContent( ma.histClass.h_ene_eta.GetNbinsX()+1 ), " integral ", ma.histClass.h_ene_eta.Integral()

raw_input("Press ENTER to exit")
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
