import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("--legend","-l",default=[],type=str,nargs='+')
calo_init.parser.add_argument("--title","-t",default="Energy resolution",type=str)
calo_init.parser.add_argument("-m","--axisMax", help="Maximum of the axis", type = float)
calo_init.parser.add_argument("--sequentialColours", "--colours", help="If Gradient of colours should be used insted of ROOT standard", default=False, action='store_true')
calo_init.parser.add_argument("--noLinearity", help="If linearity plot should not be drawn", action = 'store_false')
calo_init.parse_args()
calo_init.print_config()

print "Draw linearity: ", (calo_init.args.noLinearity)

# set proper names for the plot legend
if len(calo_init.args.legend) > 1:
    graphTitles = calo_init.args.legend
elif calo_init.args.legend:
    graphTitles, check = calo_init.substitute(calo_init.args.legend[0])
else:
    graphTitles=[]

from ROOT import gSystem, gROOT, TCanvas, TGraphErrors, TF1, TFile, TColor, TPad
from draw_functions import draw_1histogram, draw_text

graphsRes=[]
graphsLin=[]
# first loop over all files to get the resolutions
for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    f = TFile(filename, "READ")
    graphsRes.append(f.Get("resolution"))
    if (calo_init.args.noLinearity):
        graphsLin.append(f.Get("linearity"))

# Draw
cRes = TCanvas("resolution","Energy resolution",1200,900)
if (calo_init.args.noLinearity):
    pad1 = TPad("pad1","pad1",0,0,1,0.66)
    pad2 = TPad("pad2","pad2",0,0.66,1,1)
    pad2.SetBottomMargin(0.01)
    pad1.SetBorderMode(0)
    pad1.SetTopMargin(0.01)
    pad1.SetBottomMargin(0.15)
    pad2.SetBorderMode(0)
    pad1.SetTickx(1)
    pad2.SetTickx(1)
    pad1.SetTicky(1)
    pad2.SetTicky(1)
    pad1.Draw()
    pad2.Draw()
    pad1.cd()

graphsRes[0].Draw("aep")
#graphsRes[0].SetTitle(calo_init.args.title)
for g in graphsRes[1:]:
    g.Draw("sameep")

# Set nice colours, axis range, ...
if calo_init.args.sequentialColours and len(graphsRes) < 8:
    if calo_init.filenamesIn[0].find("Bfield1"):
        colour = ['#b84341','#bc5e42','#be7a42','#c09644','#c2b246','#b9c448','#a0c54a']
        colour = [colour[len(colour)-i-1] for i,c in enumerate(colour)]
        colour = [TColor.GetColor(c) for c in colour]
    else:
        colour = ['#4aa7bf','#4c90c0','#4d79c2','#4f62c4','#5851c6','#7253c7','#8d55c9']
        colour = [TColor.GetColor(c) for c in colour]
else:
    colour = [c for c in range(1,100)]
print(colour)
minima=[]
maxima=[]
for i,g in enumerate(graphsRes):
    g.SetMarkerColor(colour[i])
    g.SetLineColor(colour[i])
    g.GetFunction("res").SetLineColor(colour[i])
    minima.append(g.GetYaxis().GetXmin())
    maxima.append(g.GetYaxis().GetXmax())

if calo_init.args.axisMax:
    graphsRes[0].GetYaxis().SetRangeUser(0, calo_init.args.axisMax)
else:
    graphsRes[0].GetYaxis().SetRangeUser(0.8*min(minima),0.55*max(maxima))
cRes.Update()

graphTitles = ['#color['+str(colour[i])+']{'+t+'}' for i,t in enumerate(graphTitles)]
draw_text(graphTitles, [0.65,0.88 - 0.06 * len(graphTitles),0.88,0.88], 1, 0)

if (calo_init.args.noLinearity):
    pad2.cd()
    graphsLin[0].Draw("aep")
    for g in graphsLin[1:]:
        g.Draw("sameep")

    for i,g in enumerate(graphsLin):
        g.SetMarkerColor(colour[i])
        g.SetLineColor(colour[i])

cRes.Update()

# Save canvas
if calo_init.output(0):
    cRes.SaveAs(calo_init.output(0)+".png")
else:
    cRes.SaveAs("energy_resolution_plots.png")

raw_input("Press ENTER to exit")
