void test5()
{
   TCanvas *c1 = new TCanvas("c1", "c1", 0,   0, 600, 300);
   TCanvas *c2 = new TCanvas("c2", "c2", 605, 0, 600, 300);

   const Int_t n = 20;
   Double_t x[n], y[n];
   for (Int_t i = 0; i < n; i++) {
      x[i] = i;
      y[i] = i;
   }

   TGraph *gr = new TGraph(n, x, y);
   gr->SetMarkerStyle(20);
   c1->cd();
   c1->SetGrid();
   gr->Draw("APC");
   gr->SetHighlight();
   TGraph::SetHighlightPad(c2);

   TH1F *h[n];
   for (Int_t i = 0; i < n; i++) {
      h[i] = new TH1F(TString::Format("h_%02d", i), "", 100, -5.0, 5.0);
      h[i]->SetTitle(h[i]->GetName());
      h[i]->FillRandom("gaus");
      gr->AddHighlight(i, h[i]);
   }

   gr->GetListOfHighlights()->SetOwner(kTRUE);
   gr->RemovePoint(5);
   gr->RemovePoint(9); // point with x = 10 (after remove previous point)
   delete h[14];
   delete h[17];
}
