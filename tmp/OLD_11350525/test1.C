void test1()
{
   gROOT->Macro("$ROOTSYS/tutorials/graphs/gerrors2.C");
   TGraphErrors *gr1 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(2);
   TGraphErrors *gr2 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(3);

   // or switch with TGraph context menu
   gr1->SetHighlight();
   gr2->SetHighlight();
}
