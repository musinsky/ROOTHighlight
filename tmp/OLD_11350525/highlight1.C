// Author: Jan Musinsky
// 30/09/2014

#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TText.h>

void highlight1()
{
   TCanvas *ch = new TCanvas("ch", "ch", 0, 0, 700, 500);
   const Int_t n = 500;
   Double_t x[n], y[n];
   TH1F *h[n];

   for (Int_t i = 0; i < n; i++) {
      h[i] = new TH1F(TString::Format("h_%02d", i), "", 100, -3.0, 3.0);
      // in practice gaus need reset parameters
      h[i]->FillRandom("gaus", 1000);
      h[i]->Fit("gaus", "Q");
      h[i]->SetMaximum(250); // for n > 200
      x[i] = i;
      y[i] = h[i]->GetFunction("gaus")->GetParameter(2);
   }

   TGraph *g = new TGraph(n, x, y);
   g->SetMarkerStyle(6);
   for (Int_t i = 0; i < n; i++) g->AddHighlight(i, h[i]);
   g->Draw("AP");
   g->SetHighlight();

   TPad *ph = new TPad("ph", "ph", 0.3, 0.4, 1.0, 1.0);
   ph->SetFillColor(kBlue - 10);
   ph->Draw();
   ph->cd();
   TText *info = new TText(0.5, 0.5, "please move the mouse over the graph");
   info->SetTextAlign(22);
   info->Draw();
   ch->cd();

   TGraph::SetHighlightPad(ph);
}
