// Draw a graph with text attached to current point.
// The text is drawn in a TExec function, therefore if the text is
// moved interactively, it will be automatically updated.
// Author: Olivier Couet
void hgraphtext() {
   TCanvas *c = new TCanvas("c","A Simple Graph Example with Text",700,500);
   c->SetGrid();

   const Int_t n = 10;
   TGraph *gr = new TGraph(n);
   gr->SetTitle("A Simple Graph Example with Text");
   gr->SetMarkerStyle(20);

   Double_t x, y;
   for (Int_t i=0;i<n;i++) {
      x = i*0.1;
      y = 10*sin(x+0.2);
      gr->SetPoint(i,x,y);

   }
   gr->Draw("ALP");

   TText *info = new TText(0.3, 2.5, "please move the mouse over the graph");
   info->SetTextSize(0.04);
   info->SetTextColor(kRed+1);
   info->SetBit(kCannotPick);
   info->Draw();

   gr->SetHighlight();
   c->HighlightConnect("drawtext(TVirtualPad*,TObject*,Int_t,Int_t)");
}

void drawtext(TVirtualPad *pad, TObject *obj, Int_t ihp, Int_t /*y*/)
{
   TLatex *l = (TLatex *)pad->FindObject("TLatex");
   if (!l) {
      l = new TLatex();
      l->SetName("TLatex");
      l->SetTextSize(0.025);
      l->SetTextFont(42);
      l->SetTextAlign(21);
      l->Draw();
   }

   Double_t x,y;
   TGraph *g = (TGraph *)obj;
   g->GetPoint(ihp,x,y);
   l->SetText(x,y+0.2,Form("%4.2f",y));

   if (ihp == -1) { // after disabled
      delete l;
   }
   pad->Modified();
   pad->Update();
}
