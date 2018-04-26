#include <TCanvas.h>
#include <TSystem.h>

void HighlightPrint(TVirtualPad *p, TObject *o, Int_t x, Int_t y)
{
   Printf("pad: %p '%s'", (void *)p, p->GetName());
   Printf("obj: %p '%s'", (void *)o, o->GetName());
   Printf("x = %3d, y = %3d", x, y);

   ProcInfo_t info;
   gSystem->GetProcInfo(&info);
   Printf("virtual = %ld KB, resident = %ld KB",
          info.fMemVirtual, info.fMemResident);
}

void hlinfo2()
{
   TQObject::Connect("TCanvas", "Highlighted(TVirtualPad*,TObject*,Int_t,Int_t)",
                     0, 0, "HighlightPrint(TVirtualPad*,TObject*,Int_t,Int_t)");
}
