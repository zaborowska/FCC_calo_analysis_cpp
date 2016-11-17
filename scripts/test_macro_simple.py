import calo_init
calo_init.parse_args()
calo_init.print_config()

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

ma = CaloAnalysis_simple(calo_init.sf, calo_init.energy)
ma.loop(calo_init.filenameIn)
print "Mean hit energy: ", ma.histClass.h_hitEnergy.GetMean()
print "1/SF calculated: ", calo_init.energy/(ma.histClass.h_hitEnergy.GetMean())

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

raw_input("Press ENTER to exit")
#c1.SaveAs("plots_"+PARTICLE+str(ENERGY)+".gif")
