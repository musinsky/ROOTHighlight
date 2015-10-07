#ifndef ROOT_THighlighter
#define ROOT_THighlighter

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

class THighlighter {

protected:
   Bool_t           fHighlight;
   Bool_t           fHighlightSwitch;

public:
   THighlighter();
   virtual ~THighlighter();

   virtual Bool_t   IsHighlight() const { return fHighlight; }
   virtual void     SetHighlight(Bool_t set = kTRUE); // *TOGGLE* *GETTER=IsHighlight
   virtual Bool_t   HighlightNeedSwitch() const { return fHighlightSwitch; }
   virtual void     HighlightSwitched() { fHighlightSwitch = kFALSE; }

   ClassDef(THighlighter, 0)  // Highlighter class
};

#endif
