eta = [0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5]
Deta = [0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125]
P00 = [0.1235, 0.1314, 0.1518, 0.1824, 0.2077, 0.2328, 0.01205]
P01 = [0.0004518, 0.0004978, 0.0006698, 0.0009367, 0.00138, 0.00211, 0.003343]
P10 = [4.426, 4.312, 4.021, 3.68, 3.324, 2.754, 2.211]
P11 = [2.466, 4.068, 4.132, 6.176, 10.52, 14.62, 22.18]

from ROOT import TCanvas, TGraphErrors
from draw_functions import prepare_graph, prepare_divided_canvas,  prepare_single_canvas, draw_text, draw_1histogram

c = prepare_divided_canvas( 'etaParameters', 'Upstream energy correction parameters E_{upstream}=(p00(#eta)+p01(#eta)#cdot E) + (p10(#eta)+ p11(#eta)/#sqrt{E})#cdot E', 4 )
g00 = TGraphErrors()
g01 = TGraphErrors()
g10 = TGraphErrors()
g11 = TGraphErrors()
for i in range(0,len(eta)):
    g00.SetPoint(i,eta[i],P00[i])
    g00.SetPointError(i,0.125,0)
    g01.SetPoint(i,eta[i],P01[i])
    g01.SetPointError(i,0.125,0)
    g10.SetPoint(i,eta[i],P10[i])
    g10.SetPointError(i,0.125,0)
    g11.SetPoint(i,eta[i],P11[i])
    g11.SetPointError(i,0.125,0)
graphs = [g00, g01, g10, g11]
parTitles = ['P00', 'P01', 'P10', 'P11']
for igraph, g in enumerate(graphs):
    pad = c.cd(igraph + 1)
    prepare_graph(g, parTitles[igraph], parTitles[igraph]+'(#eta);#eta; parameter '+parTitles[igraph])
    g.GetXaxis().SetRangeUser(0,eta[len(eta)-1]+0.125)
    g.Draw("aep")
c.Update()
input("")
