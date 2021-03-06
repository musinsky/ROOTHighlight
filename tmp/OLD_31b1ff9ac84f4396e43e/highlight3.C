// Author: Jan Musinsky
// 05/10/2014

#include <TROOT.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TGraph.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TExec.h>
#include <TBox.h>
#include <TText.h>

TGraph *hgraph1 = 0;
TGraph *hgraph2 = 0;
TList *list1 = 0;
TList *list2 = 0;

void InitGraphs(TNtuple *nt, TH1F *histo, Int_t hcase);
void HighlightSelectGraph();

void highlight3()
{
   TFile *file = TFile::Open("$ROOTSYS/tutorials/hsimple.root");
   if (!file || file->IsZombie()) {
      Printf("can not open $ROOTSYS/tutorials/hsimple.root file");
      return;
   }

   TNtuple *ntuple;
   file->GetObject("ntuple", ntuple);
   if (!ntuple) return;

   TExec *ex = new TExec("ex", "HighlightSelectGraph()");
   const char *cut = "pz > 3.0";

   TCanvas *c1 = new TCanvas("c1", "c1", 0, 0, 700, 500);
   c1->Divide(1, 2);

   // case1, histo1, pz distribution
   c1->cd(1);
   ntuple->Draw("pz>>histo1(100, 2.0, 12.0)", cut);
   TH1F *histo1 = (TH1F *)gPad->FindObject("histo1");
   hgraph1 = new TGraph(histo1); // graph from histo
   hgraph1->Draw("P");
   hgraph1->SetHighlight();
   hgraph1->GetListOfFunctions()->Add(ex);
   TText *info1 = new TText(7.0, histo1->GetMaximum()*0.6,
                            "please move the mouse over this histo");
   info1->SetTextColor(histo1->GetLineColor());
   info1->SetBit(kCannotPick);
   info1->Draw();

   // case2, histo2, px*py*pz distribution
   c1->cd(2);
   ntuple->Draw("(px*py*pz)>>histo2(100, -50.0, 50.0)", cut);
   TH1F *histo2 = (TH1F *)gPad->FindObject("histo2");
   histo2->SetLineColor(kGreen+2);
   hgraph2 = new TGraph(histo2); // graph from histo
   hgraph2->Draw("P");
   hgraph2->SetHighlight();
   hgraph2->GetListOfFunctions()->Add(ex);
   TText *info2 = new TText(10.0, histo2->GetMaximum()*0.7,
                            "please move the mouse over this histo");
   info2->SetTextColor(histo2->GetLineColor());
   info2->SetBit(kCannotPick);
   info2->Draw();

   TCanvas *c2 = new TCanvas("c2", "c2", 705, 0, 500, 500);

   // common graph (all entries, all histo bins)
   ntuple->Draw("px:py", cut);
   TGraph *gcommon = (TGraph *)gPad->FindObject("Graph");
   gcommon->SetBit(kCanDelete, kFALSE); // will be redraw
   TH2F *htemp = (TH2F *)gPad->FindObject("htemp");
   gcommon->SetTitle(htemp->GetTitle());
   gcommon->GetXaxis()->SetTitle(htemp->GetXaxis()->GetTitle());
   gcommon->GetYaxis()->SetTitle(htemp->GetYaxis()->GetTitle());
   gcommon->Draw("AP");

   // must be as last
   ntuple->Draw("px:py:pz", cut, "goff");
   InitGraphs(ntuple, histo1, 1);
   InitGraphs(ntuple, histo2, 2);
}

void InitGraphs(TNtuple *nt, TH1F *histo, Int_t hcase)
{
   Long64_t nev = nt->GetSelectedRows();
   Double_t *px = nt->GetV1();
   Double_t *py = nt->GetV2();
   Double_t *pz = nt->GetV3();

   TList *list = new TList();
   if      (hcase == 1) list1 = list;
   else if (hcase == 2) list2 = list;
   else  return;

   Int_t nbins = histo->GetNbinsX();
   Int_t bin;
   TGraph *g;
   for (bin = 0; bin < nbins; bin++) {
      g = new TGraph();
      g->SetName(TString::Format("g%sbin_%d", histo->GetName(), bin+1));
      g->SetBit(kCannotPick);
      g->SetMarkerStyle(25);
      g->SetMarkerColor(histo->GetLineColor());
      list->Add(g);
   }

   Double_t value = 0.0;
   for (Long64_t ie = 0; ie < nev; ie++) {
      if (hcase == 1) value = pz[ie];
      if (hcase == 2) value = px[ie]*py[ie]*pz[ie];
      bin = histo->FindBin(value) - 1;
      g = (TGraph *)list->At(bin);
      if (!g) continue; // under/overflow
      g->SetPoint(g->GetN(), py[ie], px[ie]); // reverse as px:py
   }
}

void HighlightSelectGraph()
{
   TCanvas *c2 = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("c2");
   if (!c2) return;
   TGraph *gcommon = (TGraph *)c2->FindObject("Graph");
   if (!gcommon) return;

   TGraph *g = 0;
   if (hgraph1->GetHighlightPoint() != -1)      // case1
      g = (TGraph *)list1->At(hgraph1->GetHighlightPoint());
   else if (hgraph2->GetHighlightPoint() != -1) // case2
      g = (TGraph *)list2->At(hgraph2->GetHighlightPoint());
   if (!g) return;

   TVirtualPad *savepad = gPad;
   c2->cd();
   gcommon->Draw("AP");
   if (g->GetN() > 0) g->Draw("P");
   //gcommon->SetTitle(TString::Format("%d / %d", g->GetN(), gcommon->GetN()));
   c2->Update();
   savepad->cd();
}
