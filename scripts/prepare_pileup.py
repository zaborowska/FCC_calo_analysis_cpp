import calo_init
## add arguments relevant only for that script
calo_init.add_defaults()
calo_init.parser.add_argument("--cellColl", help="Name of the cells collection (fcc::CaloHitCollection) for the upstream energy correction", type = str)
calo_init.parser.add_argument("--bitfield", help="Bitfield used to encode the IDs (from DD4hep xml, e.g. \"system:4,x:4,y:4\"", type = str)
calo_init.parser.add_argument("--dEta", help="Size of the tower in eta", type = float, nargs=2)
calo_init.parser.add_argument("--maxEta", help="Maximum eta", type = float)
group = calo_init.parser.add_mutually_exclusive_group()
group.add_argument("--dPhi", help="Size of the tower in phi", type = float)
group.add_argument("--numPhi", help="Number of the towers in phi", type = int)
calo_init.parse_args()

from math import pi, floor
# set of default parameters
maxEta = 1.716
maxPhi = pi-(pi/512.)
nPhi = 512 # artificially increase by 1 (odd number) - to make plots look OK
dEta = 0.01
nEta = int(2*maxEta/dEta + 1)
dPhi = 2*pi/nPhi
nLayers = 8

# get parameters if passed from command line
if calo_init.args.maxEta:
    maxEta = calo_init.args.maxEta
if calo_init.args.numPhi:
    nPhi = calo_init.args.numPhi
    dPhi = 2*pi/nPhi
if calo_init.args.dPhi:
    dPhi = calo_init.args.dPhi
    nPhi = int(dPhi*2*pi)
if calo_init.args.cellColl and calo_init.args.bitfield:
    nameCellCollection = calo_init.args.cellColl
    bitfield = calo_init.args.bitfield

from ROOT import gSystem
gSystem.Load("libCaloAnalysis")
from ROOT import PileupNoise, TCanvas, TFile, gStyle, gPad, kGreen, kRed, kBlue, TColor, TPad, TH1, TH2
from draw_functions import *

# use this script for multiple files
# gStyle.SetPalette(56) # kInvertedDarkBodyRadiator
gStyle.SetPalette(73) # kCMYK
gStyle.SetOptFit(1)

for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    print "File with cells: " + filename
    analysis = PileupNoise( nameCellCollection,
                            energy,
                            maxEta, # max eta
                            maxPhi,
                            nEta, # number of bins in eta
                            nPhi, # number of bins in phi
                            dEta, # tower size in eta
                            dPhi, # tower size in phi
                            bitfield,
                            "cell", # layer field name in the bitfield
                            nLayers
                          )
    analysis.loop(filename, calo_init.verbose)
    # retrieve histograms to draw them
    hEnCell = analysis.hEnCell
    hEnCellTest = analysis.hEnCellTest
    hEnFcnAbsEta = analysis.hEnFcnAbsEta
    h1dset1 = [hEnCell, hEnCellTest]
    #h2set = [hEnFcnAbsEta]
    for h in h1dset1:
        h.SetMarkerColor(kBlue+3)
        #h.SetFillColor(0)
        h.SetLineColor(39)
        h.SetMarkerSize(2.2)

    canv = TCanvas('Minimum bias event in ECal', 'ECal', 2600, 1200 )
    canv.Divide(3,3)
    canv.cd(1)
    hEnCell.Draw()
    gPad.SetLogy(1)
    canv.cd(2)
    hEnCellTest.Draw()
    canv.cd(3)
    hEnFcnAbsEta[0].Draw()
    canv.cd(4)
    pfx = hEnFcnAbsEta[0].ProjectionY( "pfx", 1, 1,"de")
    gPad.SetLogy(1)


    print "cellEnergyTest", hEnCellTest.GetEntries(), hEnCellTest.GetMean(), hEnCellTest.GetRMS(), hEnCellTest.Integral(0,101)
    print "projection", pfx.GetEntries(), pfx.GetMean(), pfx.GetRMS(), pfx.Integral(0,1001)

    if calo_init.output(ifile):
        canv.Print(calo_init.output(ifile)+".gif")
        plots = TFile(calo_init.output(ifile)+".root","RECREATE")
    else:
        #canv.Print("Pileup.gif")
        plots = TFile("PileupTest.root","RECREATE")

    plots.cd()
    for hset in [h1dset1, hEnFcnAbsEta]:
    #for hset in [h1dset1]:
        for h in hset:
            h.Write()
    pfx.Write()
    plots.Close()
