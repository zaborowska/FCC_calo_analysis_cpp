import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("--legend","-l",default=[],type=str,nargs='+')
calo_init.parser.add_argument("--title","-t",default="Energy resolution",type=str)
calo_init.parser.add_argument("-m","--axisMax", help="Maximum of the axis", type = float)
calo_init.parser.add_argument("--sequentialColours", "--colours", help="If Gradient of colours should be used insted of ROOT standard", default=False, action='store_true')
calo_init.parse_args()
calo_init.print_config()

# set proper names for the plot legend
if len(calo_init.args.legend) > 1:
    graphTitles = calo_init.args.legend
elif calo_init.args.legend:
    graphTitles, check = calo_init.substitute(calo_init.args.legend[0])
else:
    graphTitles=[]

from ROOT import gSystem, gROOT, TCanvas, TGraphErrors, TF1, TFile, TColor
from draw_functions import draw_1histogram, draw_text

graphs=[]

# first loop over all files to get the resolutions
for ifile, filename in enumerate(calo_init.filenamesIn):
    energy = calo_init.energy(ifile)
    f = TFile(filename, "READ")
    graphs.append(f.Get("resolution"))

# Draw
cRes = TCanvas("resolution","Energy resolution",1200,900)
graphs[0].Draw("aep")
graphs[0].SetTitle(calo_init.args.title)
for g in graphs[1:]:
    g.Draw("sameep")

# Set nice colours, axis range, ...
if calo_init.args.sequentialColours and len(graphs) < 8:
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
for i,g in enumerate(graphs):
    g.SetMarkerColor(colour[i])
    g.SetLineColor(colour[i])
    g.GetFunction("res").SetLineColor(colour[i])
    minima.append(g.GetYaxis().GetXmin())
    maxima.append(g.GetYaxis().GetXmax())

if calo_init.args.axisMax:
    graphs[0].GetYaxis().SetRangeUser(0, calo_init.args.axisMax)
else:
    graphs[0].GetYaxis().SetRangeUser(0.8*min(minima),0.55*max(maxima))
cRes.Update()

graphTitles = ['#color['+str(colour[i])+']{'+t+'}' for i,t in enumerate(graphTitles)]
draw_text(graphTitles, [0.65,0.88 - 0.06 * len(graphTitles),0.88,0.88], 1, 0)
cRes.Update()

# Save canvas
if calo_init.output(0):
    cRes.SaveAs(calo_init.output(0)+".png")
else:
    cRes.SaveAs("energy_resolution_plots.png")

raw_input("Press ENTER to exit")
