import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("sf", help="SF", type = float)
calo_init.parse_args()
calo_init.print_config()
SF = calo_init.args.sf

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import CaloAnalysis_simple, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

# use this script for multiple files

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    analysis = CaloAnalysis_simple(SF, energy)
    analysis.loop(filename)
    histograms = analysis.histograms()
    print "Energy of the initial particle: ", energy
    print "Mean hit energy: ", histograms.hHitEnergy.GetMean()
    print "1/SF calculated: ", energy/(histograms.hHitEnergy.GetMean())

    c1 = TCanvas("c1"+str(energy),"c1_"+str(energy)+"_GeV",1000,1000)
    c1.Divide(2,2)
    c1.cd(1)
    draw_1histogram(histograms.hHitEnergy,"hit level energy [GeV]","")
    c1.cd(2)
    draw_1histogram(histograms.hCellEnergy,"cell level energy [GeV]","")
    histograms.hCellEnergy.Rebin(2)
    histograms.hCellEnergy.Fit("gaus")
    c1.cd(3)
    draw_1histogram(histograms.hGenPt,"Generated pt [GeV]","")
    if calo_init.output(ifile):
        c1.SaveAs(calo_init.output(ifile)+".png")
    else:
        c1.SaveAs("plots_electron_"+str(energy)+"GeV.png")

raw_input("Press ENTER to exit")
