#include <TCanvas.h>
#include <TH1.h>

void PrintInfo(TVirtualPad *pad, TObject *obj, Int_t x, Int_t y)
{
   TH1F *h = (TH1F *)obj;
   if (!h->IsHighlight()) // after highlight disabled
      h->SetTitle("highlight disable");
   else
      h->SetTitle(TString::Format("bin[%03d] (%5.2f) content %g", x,
                                  h->GetBinCenter(x), h->GetBinContent(x)));
   pad->Update();
}

void hlprint()
{
   if (!gPad) return;
   // TQObject::Connect("TCanvas", "Highlighted(TVirtualPad*,TObject*,Int_t,Int_t)",
   //                   0, 0, "PrintInfo(TVirtualPad*,TObject*,Int_t,Int_t)");
   // or simplified
   gPad->GetCanvas()->HighlightConnect("PrintInfo(TVirtualPad*,TObject*,Int_t,Int_t)");
}
