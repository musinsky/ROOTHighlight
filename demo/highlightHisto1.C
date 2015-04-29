// Author: Jan Musinsky
// 29/04/2015

#include <TCanvas.h>
#include <TH2.h>
#include <TRandom.h>
#include <TText.h>

void HighlightTitle(TPad *pad, TObject *obj, Int_t event);

void highlightHisto1()
{
   TCanvas *c1 = new TCanvas();
   TH2F *h2 = new TH2F("h2", "", 50, -5.0, 5.0, 50, -5.0, 5.0);
   for (Int_t i = 0; i < 10000; i++) h2->Fill(gRandom->Gaus(), gRandom->Gaus());
   h2->Draw();

   TText *info = new TText(0.0, -4.0, "please move the mouse over the frame");
   info->SetTextAlign(22);
   info->SetTextColor(kRed+1);
   info->SetBit(kCannotPick);
   info->Draw();
   c1->Update();

   h2->SetHighlight();
   TCanvas::Connect(c1, "Picked(TPad*,TObject*,Int_t)",
                    0, 0, "HighlightTitle(TPad*,TObject*,Int_t)");
}

void HighlightTitle(TPad *pad, TObject *obj, Int_t event)
{
   const Int_t kHighlightEvent = 70;
   if (event != kHighlightEvent) return;

   TH2 *h2 = (TH2 *)obj;
   if (!h2->IsHighlight()) { // after disabled
      h2->SetTitle("");
      return;
   }

   Int_t xhb = h2->GetXHighlightBin();
   Int_t yhb = h2->GetYHighlightBin();
   TString t;
   t.Form("bin[%02d, %02d] (%5.2f, %5.2f) content %g", xhb, yhb,
          h2->GetXaxis()->GetBinCenter(xhb), h2->GetYaxis()->GetBinCenter(yhb),
          h2->GetBinContent(xhb, yhb));
   h2->SetTitle(t.Data());
   pad->Modified();
   pad->Update();
}
