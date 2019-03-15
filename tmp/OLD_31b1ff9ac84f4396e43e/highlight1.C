// Author: Jan Musinsky
// 30/09/2014

#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TList.h>
#include <TExec.h>
#include <TText.h>

TList *l = 0;
TGraph *g = 0;

void HighlightHisto();

void highlight1()
{
   TCanvas *ch = new TCanvas("ch", "ch", 0, 0, 700, 500);
   const Int_t n = 500;
   Double_t x[n], y[n];
   TH1F *h;
   l = new TList();

   for (Int_t i = 0; i < n; i++) {
      h = new TH1F(TString::Format("h_%03d", i), "", 100, -3.0, 3.0);
      // in practice gaus need reset parameters
      h->FillRandom("gaus", 1000);
      h->Fit("gaus", "Q");
      h->SetMaximum(250.0); // for n > 200
      l->Add(h);
      x[i] = i;
      y[i] = h->GetFunction("gaus")->GetParameter(2);
   }

   g = new TGraph(n, x, y);
   g->SetMarkerStyle(6);
   g->Draw("AP");
   g->SetHighlight();
   TExec *ex = new TExec("ex", "HighlightHisto()");
   g->GetListOfFunctions()->Add(ex);

   TPad *ph = new TPad("ph", "ph", 0.3, 0.4, 1.0, 1.0);
   ph->SetFillColor(kBlue-10);
   ph->Draw();
   ph->cd();
   TText *info = new TText(0.5, 0.5, "please move the mouse over the graph");
   info->SetTextAlign(22);
   info->Draw();
   ch->cd();
}

void HighlightHisto()
{
   TVirtualPad *ph = (TVirtualPad *)gPad->FindObject("ph");
   if (!ph) return;
   Int_t ih = g->GetHighlightPoint();
   if (ih == -1) return;

   TVirtualPad *savepad = gPad;
   ph->cd();
   l->At(ih)->Draw();
   savepad->cd();
}
