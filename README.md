Function `SetHighlight()` is implemented for **`TH1`** and **`TGraph`** class.
This function switches on/off highlight mode, by default it is disabled.

Highlight mode in ROOT enables dynamic interaction by mouse movement when there
is change of bin (for histogram) or change of point (for graph). If you move
the mouse through the histogram (graph) is a change bin (point) and bin (point)
is graphically highlight (hence the name Highlight). Highlight bin for histogram
will be presented by TBox object and highlight point for graph by TMarker object
(as open circle). Moreover, any change of bin or point emits signal
**`TCanvas`**`::Highlighted()` which allows the user to react and call their own
function. For a better understanding please see this [demo video](https://youtu.be/_kWh53Q87Ew).

![Highlight mode for histogram](https://raw.githubusercontent.com/musinsky/ROOTHighlight/master/hlsimple.gif)

Highlight mode is switched by function **`TH1`**`::SetHighlight()` for histogram
or **`TGraph`**`::SetHighlight()` for graph. **`TH1`**`::IsHighlight()`
(or **`TGraph`**`::IsHighlight()`) function to verify whether the highlight mode
enabled or disabled.

``` {.cpp}
root [] .x $ROOTSYS/tutorials/hsimple.C
root [] hpx->SetHighlight(kTRUE)   // or interactively from TH1 context menu
```

The user can connect **`TCanvas`**`::Highlighted()` signal, which is always emitted
if there is a change bin (or point) and react in this way (call user function) to this
change by moving the mouse.

``` {.cpp}
void TCanvas::Highlighted(TVirtualPad *pad, TObject *obj, Int_t x, Int_t y)
```
- `pad` is pointer to pad with highlighted histogram or graph
- `obj` is pointer to highlighted histogram or graph
- `x` is highlighted x bin for 1D histogram or highlighted x-th point for graph
- `y` is highlighted y bin for 2D histogram (for 1D histogram or graph not in use)

``` {.cpp}
root [] .x $ROOTSYS/tutorials/hsimple.C
root [] hpx->SetHighlight(kTRUE)
root [] .x hlsimple.C
```

``` {.cpp}
#include <TCanvas.h>
#include <TH1.h>

void HighlightSimple(TVirtualPad *p, TObject *o, Int_t x, Int_t y)
{
   TH1F *h = (TH1F *)o;

   if (!h->IsHighlight()) // after disabled
      h->SetTitle("highlight disable");
   else
      h->SetTitle(TString::Format("bin[%03d] (%5.2f) content %g", x,
                                  h->GetBinCenter(x), h->GetBinContent(x)));
   p->Update();
}

void hlsimple()
{
   TQObject::Connect("TCanvas", "Highlighted(TVirtualPad*,TObject*,Int_t,Int_t)",
                     0, 0, "HighlightSimple(TVirtualPad*,TObject*,Int_t,Int_t)");
}
```
