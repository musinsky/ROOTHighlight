void test2()
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
}
