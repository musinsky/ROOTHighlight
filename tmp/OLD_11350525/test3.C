void test3()
{

   gROOT->Macro("$ROOTSYS/tutorials/graphs/gerrors2.C");
   TGraphErrors *gr1 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(2);
   TGraphErrors *gr2 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(3);

   // or switch with TGraph context menu
   gr1->SetHighlight();
   gr2->SetHighlight();

   TPad *c1_1 = new TPad("c1_1", "c1_1", 0.40, 0.15, 0.70 ,0.40);
   //c1_1->SetFillStyle(0);
   c1_1->Draw();

   TGraph::SetHighlightPad(c1_1);
   //TGraph::SetHighlightPad(); // or make default canvas

   gr2->AddHighlight(1, new TText(0.5, 0.5, "point 1"));
   gr2->AddHighlight(3, new TText(0.5, 0.5, "point 3"));
   gr2->AddHighlight(5, new TText(0.5, 0.5, "point 5"));
   gr2->AddHighlight(7, new TText(0.5, 0.5, "point 7"));
}
