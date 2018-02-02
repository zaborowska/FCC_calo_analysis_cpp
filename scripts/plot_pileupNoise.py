import calo_init
## add arguments relevant only for that script
calo_init.add_defaults()
calo_init.parser.add_argument("--nLayers", help="Number of layers", type = int)
calo_init.parser.add_argument("--yMax", help="Maximum on y-axis", type = float)
calo_init.parse_args()

from math import sqrt
from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, TPad, TH1, TH2, TLegend
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK
gStyle.SetOptFit(0)
gStyle.SetOptStat(0)

nlayers = 8

if calo_init.args.nLayers:
    nlayers = calo_init.args.nLayers
if calo_init.args.yMax:
    yMax = calo_init.args.yMax
else:
    yMax = 0.21

hPileupNoise = []

for ifile, filename in enumerate(calo_init.filenamesIn):
    print "File with pile-up noise histograms: ", filename
    f = ROOT.TFile(filename,"r")
    # retrieve histograms to draw them
    for i in range(0, nlayers):
        histName="h_pileup_layer"+str(i+1)
        hPileupNoise.append(f.Get(histName))

#Draw pileup per layer
canv = prepare_single_canvas("noise","Pile-up noise")
canv.cd()
legend = TLegend(0.2,0.4,0.4,0.85)
legend.SetBorderSize(0)

# Prepare graphs: set colours, axis range, ...
colour = [c for c in range(1,100)]
print(colour)
for i,g in enumerate(hPileupNoise):
    prepare_graph(g, g.GetName(), g.GetTitle(), colour[i])
    g.SetLineWidth(2)
    g.SetTitle("")
    if (i==0):
        g.Draw()
    else:
        g.Draw("same")
    g.GetYaxis().SetRangeUser(0,yMax)

    legend.AddEntry(g,"layer " + str(i+1),"lp")

legend.Draw()
draw_text(["FCC-hh simulation"], [0.67,0.88, 0.95,0.98], kGray+3, 0).SetTextSize(0.05)
draw_text(["Pile-up noise per cell"], [0.25,0.88, 0.45,0.98], 1, 0).SetTextSize(0.05)
canv.Update()

if calo_init.output(ifile):
    canv.Print(calo_init.output(ifile)+".gif")
    canv.Print(calo_init.output(ifile)+".eps")
else:
    canv.Print("pileup_noise_inclined.gif")
    canv.Print("pileup_noise_inclined.eps")
    
raw_input("Press ENTER to exit")
