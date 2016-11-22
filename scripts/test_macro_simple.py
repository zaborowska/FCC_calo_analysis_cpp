import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("sf", help="inverted sampling fraction", type = float)
calo_init.parse_args()
calo_init.print_config()
SF = calo_init.args.sf

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import HistogramClass, TCanvas, TFile, TF1, gPad
from draw_functions import draw_1histogram, draw_2histograms

# use this script for multiple files

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    analysis = HistogramClass(energy, SF)
    analysis.loop(filename, calo_init.verbose)
    print "Energy of the initial particle: ", energy
    print "Mean hit energy: ", analysis.hHitEnergy.GetMean()
    print "1/SF calculated: ", energy/(analysis.hHitEnergy.GetMean())

    c1 = TCanvas("c1"+str(energy),"c1_"+str(energy)+"_GeV",1000,1000)
    c1.Divide(2,2)
    c1.cd(1)
    draw_1histogram(analysis.hHitEnergy,"hit level energy [GeV]","")
    c1.cd(2)
    draw_1histogram(analysis.hCellEnergy,"cell level energy [GeV]","")
    analysis.hCellEnergy.Rebin(2)
    analysis.hCellEnergy.Fit("gaus")
    c1.cd(3)
    draw_1histogram(analysis.hGenPt,"Generated pt [GeV]","")
    if calo_init.output(ifile):
        c1.SaveAs(calo_init.output(ifile)+".png")
    else:
        c1.SaveAs("plots_electron_"+str(energy)+"GeV.png")

raw_input("Press ENTER to exit")
