#include <TCanvas.h>

void HighlightPrint(TVirtualPad *p, TObject *o, Int_t x, Int_t y)
{
   Printf("pad: %p '%s'", (void *)p, p->GetName());
   Printf("obj: %p '%s'", (void *)o, o->GetName());
   Printf("x = %3d, y = %3d", x, y);
}

void hlinfo()
{
   TQObject::Connect("TCanvas", "Highlighted(TVirtualPad*,TObject*,Int_t,Int_t)",
                     0, 0, "HighlightPrint(TVirtualPad*,TObject*,Int_t,Int_t)");
}
