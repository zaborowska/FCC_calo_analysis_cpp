# Setup ROOT
import ROOT
from ROOT import TH1F, TLegend, gPad, TCanvas, SetOwnership, TPaveText, TLine

def draw_1histogram( histo, x_axisName, y_axisName ):
   histo.GetXaxis().SetTitle(x_axisName)
   if (y_axisName==""):
      histo.GetYaxis().SetTitle("Entries/per bin")
   else:
      histo.GetYaxis().SetTitle(y_axisName)
   maximum = 1.2*histo.GetMaximum()
   histo.SetMaximum(maximum)
   histo.Draw()
   gPad.Update()
   return

def draw_2histograms( histo1, histo2, x_axisName, y_axisName, leg1Name, leg2Name ):
   histo1.GetXaxis().SetTitle(x_axisName)
   if (y_axisName==""):
      histo1.GetYaxis().SetTitle("Entries/per bin")
   else:
      histo1.GetYaxis().SetTitle(y_axisName)

   histo2.SetLineColor(2)
   
   if (leg1Name!=""):
      histo1.SetStats(0)
      histo2.SetStats(0)
      legend=TLegend(0.45,0.78,0.9,0.9)
      legend.AddEntry(histo1, leg1Name, "l")
      legend.AddEntry(histo2, leg2Name, "l")
      legend.SetTextSize(0.05)
      SetOwnership( legend, 0 )

   maximum1 = 1.2*histo1.GetMaximum()
   maximum2 = 1.2*histo2.GetMaximum()
   maximum = maximum1
   if (maximum2>maximum):
      maximum = maximum2
   histo1.SetMaximum(maximum)
   histo1.Draw()
   histo2.Draw("same")
   if (leg1Name!=""):
      legend.Draw("same")

   gPad.Update()

   return


def draw_1histogram_normalized( histo, x_axisName, y_axisName ):
   histo.Scale(1./histo.GetEntries())
   draw_1histogram(histo, x_axisName, y_axisName)
   return

def draw_2histograms_normalized( histo1, histo2, x_axisName,y_axisName, leg1Name, leg2Name ):
   histo1.Scale(1./histo1.GetEntries())
   histo2.Scale(1./histo2.GetEntries())
   draw_2histograms( histo1, histo2, x_axisName, y_axisName, leg1Name, leg2Name)
   return

def draw_hist2d(hist):
    hist.Draw('colz')

def draw_text(lines, coordinates = [0.1,0.8,0.5,0.9], colour = 36):
   text = TPaveText(coordinates[0],
                    coordinates[1],
                    coordinates[2],
                    coordinates[3],"brNDC")
   text.SetFillColorAlpha(0,1)
   for line in lines:
      text.AddText("#color["+str(colour)+"]{"+line+"}")
      print(line)
      text.Draw()
      ROOT.SetOwnership(text,False)

def draw_rectangle(start = [0,0], end = [1,1], colour = 2, width = 2):
   lines = []
   lines.append(TLine(start[0],start[1],end[0],start[1]))
   lines.append(TLine(end[0],start[1],end[0],end[1]))
   lines.append(TLine(start[0],end[1],end[0],end[1]))
   lines.append(TLine(start[0],start[1],start[0],end[1]))
   for line in lines:
      line.SetLineColor(colour)
      line.SetLineWidth(width)
      line.Draw()
      ROOT.SetOwnership(line,False)
