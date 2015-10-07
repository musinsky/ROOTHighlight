#ifndef ROOT_THighlighter
#define ROOT_THighlighter

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

class THighlighter {

protected:
   Bool_t       fHighlight;
   Int_t        fXHighlight;
   Int_t        fYHighlight;

   Bool_t       fHighlightSwitch;

public:
   THighlighter();
   virtual ~THighlighter();

   virtual Bool_t   IsHighlight() const { return fHighlight; }
   virtual Int_t    GetXHighlight() const { return fXHighlight; }
   virtual Int_t    GetYHighlight() const { return fYHighlight; }
   virtual void     SetHighlight(Bool_t set = kTRUE); // *TOGGLE* *GETTER=IsHighlight
   virtual void     SetXHighlight(Int_t x) { fXHighlight = x; }
   virtual void     SetYHighlight(Int_t y) { fYHighlight = y; }

   virtual Bool_t   HighlightNeedSwitch() const { return fHighlightSwitch; }
   virtual void     HighlightSwitched() { fHighlightSwitch = kFALSE; }

   ClassDef(THighlighter, 0)  // Highlighter class
};

#endif
