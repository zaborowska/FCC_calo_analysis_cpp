import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("-t","--title", default="Energy resolution", help="Graph title", type=str)
calo_init.parser.add_argument("-n","--histogramName", default="energy", help="Name of the histogram containing the energy distribution", type = str)
calo_init.parser.add_argument("-m","--axisMax", help="Maximum of the axis", type = float)
calo_init.parse_args()
calo_init.print_config()

histName = calo_init.args.histogramName

from ROOT import gSystem, gROOT, TCanvas, TGraphErrors, TF1, gStyle, kRed, kBlue, TFile, TTree
from draw_functions import draw_1histogram, draw_text
import numpy

gRes = TGraphErrors()

# first loop over all files to get the resolutions
for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    f = TFile(filename, "READ")
    htotal = f.Get(histName)
    myfunPre = TF1("firstGaus","gaus", htotal.GetMean() - 2. * htotal.GetRMS(),
                   htotal.GetMean() + 2. * htotal.GetRMS())
    resultPre = htotal.Fit(myfunPre, "SRQN")
    myfun = TF1("finalGaus", "gaus", resultPre.Get().Parameter(1) - 2. * resultPre.Get().Parameter(2),
                resultPre.Get().Parameter(1) + 2. * resultPre.Get().Parameter(2) )
    result = htotal.Fit(myfun, "SRQN")
    resolution = result.Get().Parameter(2) / result.Get().Parameter(1)
    resolutionError = result.Get().Error(2) / result.Get().Parameter(1) + result.Get().Error(1) * result.Get().Parameter(2) / ( result.Get().Parameter(1) ** 2)
    gRes.SetPoint(ifile, energy, resolution)
    gRes.SetPointError(ifile, 0, resolutionError)

# Set properties of the graph
if filename.find("Bfield0") > 0:
    colour = kRed + 1 # red colour if no B field
else:
    colour = kBlue + 1 # blue otherwise (default)
gRes.SetTitle(calo_init.args.title+";particle energy (GeV);energy resolution")
gRes.SetName("resolution")
gRes.SetMarkerStyle(21)
gRes.SetMarkerSize(1.2)
gRes.SetMarkerColor(colour)
gRes.SetLineColor(colour)
gRes.GetXaxis().SetTitleOffset(1.3)
gRes.GetYaxis().SetTitleOffset(1.5)

# Fit energy resolution
fRes = TF1("res", "sqrt([0]*[0] + pow([1]/sqrt(x),2))",5,600)
fRes.SetParName(0,"const")
fRes.SetParName(1,"sqrt")
fRes.SetLineColor(colour)
fitResult = gRes.Fit(fRes, 'S')

# Draw
cRes = TCanvas("resolution","Energy resolution",1200,900)
gRes.Draw("ape")
if calo_init.args.axisMax:
    gRes.GetYaxis().SetRangeUser(0, calo_init.args.axisMax)
formula = str(round(fitResult.Get().Parameter(0),4))+" #oplus #frac{"+str(round(fitResult.Get().Parameter(1),4))+"}{#sqrt{E}}"
constString = "const: "+str(round(fitResult.Get().Parameter(0),4))+" #pm "+str(round(fitResult.Get().Error(0),4))
samplingString = "sampl: "+str(round(fitResult.Get().Parameter(1),4))+" #pm "+str(round(fitResult.Get().Error(1),4))
draw_text([formula], [0.55,0.8,0.88,0.86], colour, 0)
draw_text([constString, samplingString], [0.55,0.7,0.88,0.78], colour+1, 0)
cRes.Update()

# Save canvas and root file with graph, const term and sampling term
if calo_init.output(0):
    cRes.SaveAs(calo_init.output(0)+".png")
    plots = TFile(calo_init.output(0)+".root","RECREATE")
else:
    cRes.SaveAs("energy_resolution_plots.png")
    plots = TFile("energy_resolution_plots.root","RECREATE")
gRes.Write()
const = numpy.zeros(1, dtype=float)
sampl = numpy.zeros(1, dtype=float)
constErr = numpy.zeros(1, dtype=float)
samplErr = numpy.zeros(1, dtype=float)
t = TTree("params", "Fit parameters")
t.Branch("const", const, "const/D");
t.Branch("sampl", sampl, "sampl/D");
t.Branch("constErr", constErr, "constErr/D");
t.Branch("samplErr", samplErr, "samplErr/D");
const[0] = fitResult.Get().Parameter(0)
sampl[0] = fitResult.Get().Parameter(1)
constErr[0] = fitResult.Get().Error(0)
samplErr[0] = fitResult.Get().Error(1)
t.Fill()
plots.Write()
plots.Close()

raw_input("Press ENTER to exit")
