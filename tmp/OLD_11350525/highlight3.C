// Author: Jan Musinsky
// 30/09/2014

#include <TROOT.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TGraph.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TExec.h>
#include <TBox.h>
#include <TText.h>
#include <TMath.h>

TNtuple *ntuple = 0;
TGraph *graph = 0;

void highlight3()
{
   TFile *file = TFile::Open("$ROOTSYS/tutorials/hsimple.root");
   if (!file || file->IsZombie()) {
      Printf("please first run $ROOTSYS/tutorials/hsimple.C");
      return;
   }

   file->GetObject("ntuple", ntuple);
   if (!ntuple) return;

   TCanvas *c1 = new TCanvas("c1", "c1", 0,   0, 500, 500);
   const char *cut = "pz > 3.0";
   ntuple->Draw("px:py", cut);
   graph = (TGraph *)gPad->FindObject("Graph");
   graph->SetHighlight();
   TExec *ex = new TExec("ex", "HighlightBinId();");
   graph->GetListOfFunctions()->Add(ex);
   TText *info = new TText(0.0, 4.5, "please move the mouse over the graph");
   info->SetTextAlign(22);
   info->SetTextSize(0.03);
   info->SetTextColor(kRed+1);
   info->SetBit(kCannotPick);
   info->Draw();

   TCanvas *c2 = new TCanvas("c2", "c2", 505, 0, 600, 400);
   ntuple->Draw("TMath::Sqrt(px*px + py*py + pz*pz)>>histo(100, 0, 15)", cut);

   // must be as last
   ntuple->Draw("px:py:pz:i", cut, "goff");
}

void HighlightBinId()
{
   TCanvas *c2 = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c2");
   if (!c2) return;
   TH1F *histo = (TH1F *)c2->FindObject("histo");
   if (!histo) return;
   Int_t ih = graph->GetHighlightPoint();
   if (ih == -1) return;

   Double_t px = ntuple->GetV1()[ih];
   Double_t py = ntuple->GetV2()[ih];
   Double_t pz = ntuple->GetV3()[ih];
   Double_t i  = ntuple->GetV4()[ih];
   Double_t p  = TMath::Sqrt(px*px + py*py + pz*pz);
   Int_t hbin = histo->FindBin(p);

   Bool_t redraw = kFALSE;
   TBox *bh = (TBox *)c2->FindObject("TBox");
   if (!bh) {
      bh = new TBox();
      bh->SetFillColor(histo->GetLineColor());
      bh->SetFillStyle(3001);
      bh->SetBit(kCanDelete);
      redraw = kTRUE;
   }
   bh->SetX1(histo->GetBinLowEdge(hbin));
   bh->SetY1(histo->GetMinimum());
   bh->SetX2(histo->GetBinWidth(hbin) + histo->GetBinLowEdge(hbin));
   bh->SetY2(histo->GetBinContent(hbin));

   TText *th = (TText *)c2->FindObject("TText");
   if (!th) {
      th = new TText();
      th->SetName("TText");
      th->SetTextColor(graph->GetMarkerColor());
      th->SetBit(kCanDelete);
      redraw = kTRUE;
   }
   th->SetText(histo->GetXaxis()->GetXmax()*0.75, histo->GetMaximum()*0.5,
               TString::Format("id = %d", (Int_t)i));

   c2->Modified();
   c2->Update();

   if (!redraw) return;
   TVirtualPad *savepad = gPad;
   c2->cd();
   bh->Draw();
   th->Draw();
   c2->Update();
   if (savepad) savepad->cd();
}
