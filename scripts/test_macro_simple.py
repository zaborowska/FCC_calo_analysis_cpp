import calo_init
calo_init.parse_args()
calo_init.print_config()

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

# use this script for multiple files

for energy, filename in zip(calo_init.energies, calo_init.filenames):
    analysis = CaloAnalysis_simple(calo_init.sf, energy)
    analysis.loop(filename)
    histograms = analysis.histograms()
    print "Mean hit energy: ", histograms.h_hitEnergy.GetMean()
    print "1/SF calculated: ", energy/(histograms.h_hitEnergy.GetMean())

    c1 = TCanvas("c1"+str(energy),"c1_"+str(energy)+"_GeV",1000,1000)
    c1.Divide(2,2)
    c1.cd(1)
    draw_1histogram(histograms.h_hitEnergy,"hit level energy [GeV]","")
    c1.cd(2)
    draw_1histogram(histograms.h_cellEnergy,"cell level energy [GeV]","")
    histograms.h_cellEnergy.Rebin(2)
    histograms.h_cellEnergy.Fit("gaus")
    c1.cd(3)
    draw_1histogram(histograms.h_ptGen,"Generated pt [GeV]","")
    c1.SaveAs("plots_electron_"+str(energy)+"GeV.png")

raw_input("Press ENTER to exit")
