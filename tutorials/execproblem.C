// Author: Jan Musinsky
// 04/04/2018

#include <TROOT.h>
#include <TSystem.h>
#include <TExec.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TGraphPainter.h>
#include <TCanvas.h>

void PrintMem();
void execproblem(Bool_t asexec = kTRUE)
{
   gROOT->Macro("$ROOTSYS/tutorials/graphs/gerrors2.C");

   TH1F *hframe = (TH1F *)gPad->GetListOfPrimitives()->At(1);
   hframe->SetTitle("please move the mouse over the graph");
   TGraphErrors *gr1 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(2);
   TGraphErrors *gr2 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(3);

   // or switch with TGraph context menu
   gr1->SetHighlight();
   gr2->SetHighlight();

   if (asexec) {
      TExec *ex = new TExec("ex", "ProcessExec()");
      gr1->GetListOfFunctions()->Add(ex);
      //   gr2->GetListOfFunctions()->Add(ex);
   } else {
      gPad->GetCanvas()->HighlightConnect("Highlight(TVirtualPad*,TObject*,Int_t,Int_t)");
   }
}

void ProcessExec()
{
   TGraphErrors *gr1 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(2);
   TGraphErrors *gr2 = (TGraphErrors *)gPad->GetListOfPrimitives()->At(3);

   TGraphPainter *gp = (TGraphPainter *)TVirtualGraphPainter::GetPainter();
   Int_t hp1 = gp->GetHighlightPoint(gr1);
   Int_t hp2 = gp->GetHighlightPoint(gr2);
   if (hp1 != -1) Printf("EXEC graph: %p\tpoint: %d", (void *)gr1, hp1);
   if (hp2 != -1) Printf("EXEC graph: %p\tpoint: %d", (void *)gr2, hp2);

   // TExec::Exec() call gROOT->ProcessLine() => memory leak problem
   // https://sft.its.cern.ch/jira/browse/ROOT-8069
   PrintMem();
}

void Highlight(TVirtualPad *pad, TObject *obj, Int_t xhb, Int_t yhb)
{
   Printf("HIGHLIGHT graph: %p\tpoint: %d", (void *)obj, xhb);
   PrintMem();
}

void PrintMem()
{
   ProcInfo_t info;
   gSystem->GetProcInfo(&info);
   Printf("virtual = %ld KB, resident = %ld KB", info.fMemVirtual, info.fMemResident);
}
