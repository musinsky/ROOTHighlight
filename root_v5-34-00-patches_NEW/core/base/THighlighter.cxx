#include "THighlighter.h"
#include "TVirtualPad.h"

ClassImp(THighlighter)

//______________________________________________________________________________
THighlighter::THighlighter()
: fHighlight(kFALSE),
  fHighlightSwitch(kFALSE)
{
   // Default constructor
}
//______________________________________________________________________________
THighlighter::~THighlighter()
{
   // Destructor
}
//______________________________________________________________________________
void THighlighter::SetHighlight(Bool_t set)
{
   if (fHighlight == set) return;

   fHighlight = set;
   fHighlightSwitch = kTRUE;
   if (gPad) gPad->Modified(kTRUE);

   //   TObject *o = dynamic_cast<TObject *>(this); // OK
   //   TObject *o = (TObject *)this;               // wrong
}
