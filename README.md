Function `SetHighlight()` is implemented for **`TH1`** and **`TGraph`** class.
This function switches on/off highlight mode, by default it is disabled.

Highlight mode in ROOT enables dynamic interaction by mouse movement when there
is change of bin (for histogram) or change of point (for graph). If you move
the mouse through the histogram (graph) is a change bin (point) and bin (point)
is graphically highlight (hence the name Highlight). Highlight bin for histogram
will be presented by TBox object and highlight point for graph by TMarker object
(as open circle). Moreover, any change of bin or point emits signal
**`TCanvas`**`::Highlighted()` which allows the user to react and call their own
function.

![Highlight mode for histogram](https://raw.githubusercontent.com/musinsky/ROOTHighlight/master/hlsimple.gif)

