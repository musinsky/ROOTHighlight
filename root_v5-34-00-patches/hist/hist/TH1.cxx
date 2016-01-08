// @(#)root/hist:$Id$
// Author: Rene Brun   26/12/94

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TClass.h"
#include "TMath.h"
#include "THashList.h"
#include "TH1.h"
#include "TH2.h"
#include "TF2.h"
#include "TF3.h"
#include "TPluginManager.h"
#include "TVirtualPad.h"
#include "TRandom.h"
#include "TVirtualFitter.h"
#include "THLimitsFinder.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TVectorF.h"
#include "TVectorD.h"
#include "TBrowser.h"
#include "TObjString.h"
#include "TError.h"
#include "TVirtualHistPainter.h"
#include "TVirtualFFT.h"
#include "TSystem.h"

#include "HFitInterface.h"
#include "Fit/DataRange.h"
#include "Fit/BinData.h"
#include "Math/GoFTest.h"
#include "Math/MinimizerOptions.h"
#include "Math/QuantFuncMathCore.h"

//______________________________________________________________________________
/* Begin_Html
<center><h2>The Histogram classes</h2></center>
ROOT supports the following histogram types:
<ul>
  <li>1-D histograms:
   <ul>
         <li>TH1C : histograms with one byte per channel.   Maximum bin content = 127
         <li>TH1S : histograms with one short per channel.  Maximum bin content = 32767
         <li>TH1I : histograms with one int per channel.    Maximum bin content = 2147483647
         <li>TH1F : histograms with one float per channel.  Maximum precision 7 digits
         <li>TH1D : histograms with one double per channel. Maximum precision 14 digits
   </ul>

  <li>2-D histograms:
   <ul>
         <li>TH2C : histograms with one byte per channel.   Maximum bin content = 127
         <li>TH2S : histograms with one short per channel.  Maximum bin content = 32767
         <li>TH2I : histograms with one int per channel.    Maximum bin content = 2147483647
         <li>TH2F : histograms with one float per channel.  Maximum precision 7 digits
         <li>TH2D : histograms with one double per channel. Maximum precision 14 digits
   </ul>

  <li>3-D histograms:
   <ul>
         <li>TH3C : histograms with one byte per channel.   Maximum bin content = 127
         <li>TH3S : histograms with one short per channel.  Maximum bin content = 32767
         <li>TH3I : histograms with one int per channel.    Maximum bin content = 2147483647
         <li>TH3F : histograms with one float per channel.  Maximum precision 7 digits
         <li>TH3D : histograms with one double per channel. Maximum precision 14 digits
   </ul>
  <li>Profile histograms: See classes  TProfile, TProfile2D and TProfile3D.
      Profile histograms are used to display the mean value of Y and its RMS
      for each bin in X. Profile histograms are in many cases an elegant
      replacement of two-dimensional histograms : the inter-relation of two
      measured quantities X and Y can always be visualized by a two-dimensional
      histogram or scatter-plot; If Y is an unknown (but single-valued)
      approximate function of X, this function is displayed by a profile
      histogram with much better precision than by a scatter-plot.
</ul>

All histogram classes are derived from the base class TH1
<pre>
                                TH1
                                 ^
                                 |
                                 |
                                 |
         -----------------------------------------------------------
                |                |       |      |      |     |     |
                |                |      TH1C   TH1S   TH1I  TH1F  TH1D
                |                |                                 |
                |                |                                 |
                |               TH2                             TProfile
                |                |
                |                |
                |                ----------------------------------
                |                        |      |      |     |     |
                |                       TH2C   TH2S   TH2I  TH2F  TH2D
                |                                                  |
               TH3                                                 |
                |                                               TProfile2D
                |
                -------------------------------------
                        |      |      |      |      |
                       TH3C   TH3S   TH3I   TH3F   TH3D
                                                    |
                                                    |
                                                 TProfile3D

      The TH*C classes also inherit from the array class TArrayC.
      The TH*S classes also inherit from the array class TArrayS.
      The TH*I classes also inherit from the array class TArrayI.
      The TH*F classes also inherit from the array class TArrayF.
      The TH*D classes also inherit from the array class TArrayD.
</pre>

<h4>Creating histograms</h4>
<p>
     Histograms are created by invoking one of the constructors, e.g.
<pre>
       TH1F *h1 = new TH1F("h1", "h1 title", 100, 0, 4.4);
       TH2F *h2 = new TH2F("h2", "h2 title", 40, 0, 4, 30, -3, 3);
</pre>
<p>  Histograms may also be created by:
  <ul>
      <li> calling the Clone function, see below
      <li> making a projection from a 2-D or 3-D histogram, see below
      <li> reading an histogram from a file
   </ul>
<p>  When an histogram is created, a reference to it is automatically added
     to the list of in-memory objects for the current file or directory.
     This default behaviour can be changed by:
<pre>
       h->SetDirectory(0);          for the current histogram h
       TH1::AddDirectory(kFALSE);   sets a global switch disabling the reference
</pre>
     When the histogram is deleted, the reference to it is removed from
     the list of objects in memory.
     When a file is closed, all histograms in memory associated with this file
     are automatically deleted.

<h4>Fix or variable bin size</h4>

     All histogram types support either fix or variable bin sizes.
     2-D histograms may have fix size bins along X and variable size bins
     along Y or vice-versa. The functions to fill, manipulate, draw or access
     histograms are identical in both cases.
<p>     Each histogram always contains 3 objects TAxis: fXaxis, fYaxis and fZaxis
     To access the axis parameters, do:
<pre>
        TAxis *xaxis = h->GetXaxis(); etc.
        Double_t binCenter = xaxis->GetBinCenter(bin), etc.
</pre>
     See class TAxis for a description of all the access functions.
     The axis range is always stored internally in double precision.

<h4>Convention for numbering bins</h4>

      For all histogram types: nbins, xlow, xup
<pre>
        bin = 0;       underflow bin
        bin = 1;       first bin with low-edge xlow INCLUDED
        bin = nbins;   last bin with upper-edge xup EXCLUDED
        bin = nbins+1; overflow bin
</pre>
<p>      In case of 2-D or 3-D histograms, a "global bin" number is defined.
      For example, assuming a 3-D histogram with (binx, biny, binz), the function
<pre>
        Int_t gbin = h->GetBin(binx, biny, binz);
</pre>
      returns a global/linearized gbin number. This global gbin is useful
      to access the bin content/error information independently of the dimension.
      Note that to access the information other than bin content and errors
      one should use the TAxis object directly with e.g.:
<pre>
         Double_t xcenter = h3->GetZaxis()->GetBinCenter(27);
</pre>
       returns the center along z of bin number 27 (not the global bin)
       in the 3-D histogram h3.

<h4>Alphanumeric Bin Labels</h4>

     By default, an histogram axis is drawn with its numeric bin labels.
     One can specify alphanumeric labels instead with:
<ul>
       <li> call TAxis::SetBinLabel(bin, label);
           This can always be done before or after filling.
           When the histogram is drawn, bin labels will be automatically drawn.
           See example in $ROOTSYS/tutorials/graphs/labels1.C, labels2.C
       <li> call to a Fill function with one of the arguments being a string, e.g.
<pre>
           hist1->Fill(somename, weigth);
           hist2->Fill(x, somename, weight);
           hist2->Fill(somename, y, weight);
           hist2->Fill(somenamex, somenamey, weight);
</pre>
           See example in $ROOTSYS/tutorials/hist/hlabels1.C, hlabels2.C
       <li> via TTree::Draw.
           see for example $ROOTSYS/tutorials/tree/cernstaff.C
<pre>
           tree.Draw("Nation::Division");
</pre>
           where "Nation" and "Division" are two branches of a Tree.
</ul>

<p>     When using the options 2 or 3 above, the labels are automatically
     added to the list (THashList) of labels for a given axis.
     By default, an axis is drawn with the order of bins corresponding
     to the filling sequence. It is possible to reorder the axis

<ul>
          <li>alphabetically
          <li>by increasing or decreasing values
</ul>

<p>     The reordering can be triggered via the TAxis context menu by selecting
     the menu item "LabelsOption" or by calling directly
        TH1::LabelsOption(option, axis) where
<ul>
          <li>axis may be "X", "Y" or "Z"
          <li>option may be:
           <ul>
             <li>"a" sort by alphabetic order
             <li>">" sort by decreasing values
             <li>"<" sort by increasing values
             <li>"h" draw labels horizontal
             <li>"v" draw labels vertical
             <li>"u" draw labels up (end of label right adjusted)
             <li>"d" draw labels down (start of label left adjusted)
           </ul>
</ul>
<p>     When using the option 2 above, new labels are added by doubling the current
     number of bins in case one label does not exist yet.
     When the Filling is terminated, it is possible to trim the number
     of bins to match the number of active labels by calling
<pre>
           TH1::LabelsDeflate(axis) with axis = "X", "Y" or "Z"
</pre>
     This operation is automatic when using TTree::Draw.
     Once bin labels have been created, they become persistent if the histogram
     is written to a file or when generating the C++ code via SavePrimitive.

<h4>Histograms with automatic bins</h4>

     When an histogram is created with an axis lower limit greater or equal
     to its upper limit, the SetBuffer is automatically called with an
     argument fBufferSize equal to fgBufferSize (default value=1000).
     fgBufferSize may be reset via the static function TH1::SetDefaultBufferSize.
     The axis limits will be automatically computed when the buffer will
     be full or when the function BufferEmpty is called.

<h4>Filling histograms</h4>

     An histogram is typically filled with statements like:
<pre>
       h1->Fill(x);
       h1->Fill(x, w); //fill with weight
       h2->Fill(x, y)
       h2->Fill(x, y, w)
       h3->Fill(x, y, z)
       h3->Fill(x, y, z, w)
</pre>
     or via one of the Fill functions accepting names described above.
     The Fill functions compute the bin number corresponding to the given
     x, y or z argument and increment this bin by the given weight.
     The Fill functions return the bin number for 1-D histograms or global
     bin number for 2-D and 3-D histograms.
<p>     If TH1::Sumw2 has been called before filling, the sum of squares of
     weights is also stored.
     One can also increment directly a bin number via TH1::AddBinContent
     or replace the existing content via TH1::SetBinContent.
     To access the bin content of a given bin, do:
<pre>
       Double_t binContent = h->GetBinContent(bin);
</pre>

<p>     By default, the bin number is computed using the current axis ranges.
     If the automatic binning option has been set via
<pre>
       h->SetBit(TH1::kCanRebin);
</pre>
     then, the Fill Function will automatically extend the axis range to
     accomodate the new value specified in the Fill argument. The method
     used is to double the bin size until the new value fits in the range,
     merging bins two by two. This automatic binning options is extensively
     used by the TTree::Draw function when histogramming Tree variables
     with an unknown range.
<p>     This automatic binning option is supported for 1-D, 2-D and 3-D histograms.

     During filling, some statistics parameters are incremented to compute
     the mean value and Root Mean Square with the maximum precision.

<p>     In case of histograms of type TH1C, TH1S, TH2C, TH2S, TH3C, TH3S
     a check is made that the bin contents do not exceed the maximum positive
     capacity (127 or 32767). Histograms of all types may have positive
     or/and negative bin contents.

<h4>Rebinning</h4>

     At any time, an histogram can be rebinned via TH1::Rebin. This function
     returns a new histogram with the rebinned contents.
     If bin errors were stored, they are recomputed during the rebinning.

<h4>Associated errors</h4>

     By default, for each bin, the sum of weights is computed at fill time.
     One can also call TH1::Sumw2 to force the storage and computation
     of the sum of the square of weights per bin.
     If Sumw2 has been called, the error per bin is computed as the
     sqrt(sum of squares of weights), otherwise the error is set equal
     to the sqrt(bin content).
     To return the error for a given bin number, do:
<pre>
        Double_t error = h->GetBinError(bin);
</pre>

<h4>Associated functions</h4>

     One or more object (typically a TF1*) can be added to the list
     of functions (fFunctions) associated to each histogram.
     When TH1::Fit is invoked, the fitted function is added to this list.
     Given an histogram h, one can retrieve an associated function
     with:
<pre>
        TF1 *myfunc = h->GetFunction("myfunc");
</pre>

<h4>Operations on histograms</h4>


     Many types of operations are supported on histograms or between histograms
<ul>
     <li> Addition of an histogram to the current histogram.
     <li> Additions of two histograms with coefficients and storage into the current
       histogram.
     <li> Multiplications and Divisions are supported in the same way as additions.
     <li> The Add, Divide and Multiply functions also exist to add, divide or multiply
       an histogram by a function.
</ul>
     If an histogram has associated error bars (TH1::Sumw2 has been called),
     the resulting error bars are also computed assuming independent histograms.
     In case of divisions, Binomial errors are also supported.
     One can mark a histogram to be an "average" histogram by setting its bit kIsAverage via
       myhist.SetBit(TH1::kIsAverage);
     When adding (see TH1::Add) average histograms, the histograms are averaged and not summed.



<h4>Fitting histograms</h4>

     Histograms (1-D, 2-D, 3-D and Profiles) can be fitted with a user
     specified function via TH1::Fit. When an histogram is fitted, the
     resulting function with its parameters is added to the list of functions
     of this histogram. If the histogram is made persistent, the list of
     associated functions is also persistent. Given a pointer (see above)
     to an associated function myfunc, one can retrieve the function/fit
     parameters with calls such as:
<pre>
       Double_t chi2 = myfunc->GetChisquare();
       Double_t par0 = myfunc->GetParameter(0); value of 1st parameter
       Double_t err0 = myfunc->GetParError(0);  error on first parameter
</pre>

<h4>Projections of histograms</h4>

<p>     One can:
<ul>
      <li> make a 1-D projection of a 2-D histogram or Profile
        see functions TH2::ProjectionX,Y, TH2::ProfileX,Y, TProfile::ProjectionX
      <li> make a 1-D, 2-D or profile out of a 3-D histogram
        see functions TH3::ProjectionZ, TH3::Project3D.
</ul>

<p>     One can fit these projections via:
<pre>
      TH2::FitSlicesX,Y, TH3::FitSlicesZ.
</pre>

<h4>Random Numbers and histograms</h4>

     TH1::FillRandom can be used to randomly fill an histogram using
                    the contents of an existing TF1 function or another
                    TH1 histogram (for all dimensions).
<p>     For example the following two statements create and fill an histogram
     10000 times with a default gaussian distribution of mean 0 and sigma 1:
<pre>
       TH1F h1("h1", "histo from a gaussian", 100, -3, 3);
       h1.FillRandom("gaus", 10000);
</pre>
     TH1::GetRandom can be used to return a random number distributed
                    according the contents of an histogram.

<h4>Making a copy of an histogram</h4>

     Like for any other ROOT object derived from TObject, one can use
     the Clone() function. This makes an identical copy of the original
     histogram including all associated errors and functions, e.g.:
<pre>
       TH1F *hnew = (TH1F*)h->Clone("hnew");
</pre>

<h4>Normalizing histograms</h4>

     One can scale an histogram such that the bins integral is equal to
     the normalization parameter via TH1::Scale(Double_t norm), where norm
     is the desired normalization divided by the integral of the histogram.

<h4>Drawing histograms</h4>

     Histograms are drawn via the THistPainter class. Each histogram has
     a pointer to its own painter (to be usable in a multithreaded program).
     Many drawing options are supported.
     See THistPainter::Paint() for more details.
<p>
    The same histogram can be drawn with different options in different pads.
     When an histogram drawn in a pad is deleted, the histogram is
     automatically removed from the pad or pads where it was drawn.
     If an histogram is drawn in a pad, then filled again, the new status
     of the histogram will be automatically shown in the pad next time
     the pad is updated. One does not need to redraw the histogram.
     To draw the current version of an histogram in a pad, one can use
<pre>
        h->DrawCopy();
</pre>
     This makes a clone (see Clone below) of the histogram. Once the clone
     is drawn, the original histogram may be modified or deleted without
     affecting the aspect of the clone.
<p>
     One can use TH1::SetMaximum() and TH1::SetMinimum() to force a particular
     value for the maximum or the minimum scale on the plot. (For 1-D
     histograms this means the y-axis, while for 2-D histograms these
     functions affect the z-axis).
<p>
     TH1::UseCurrentStyle() can be used to change all histogram graphics
     attributes to correspond to the current selected style.
     This function must be called for each histogram.
     In case one reads and draws many histograms from a file, one can force
     the histograms to inherit automatically the current graphics style
     by calling before gROOT->ForceStyle().


<h4>Setting Drawing histogram contour levels (2-D hists only)</h4>

     By default contours are automatically generated at equidistant
     intervals. A default value of 20 levels is used. This can be modified
     via TH1::SetContour() or TH1::SetContourLevel().
     the contours level info is used by the drawing options "cont", "surf",
     and "lego".

<h4>Setting histogram graphics attributes</h4>

     The histogram classes inherit from the attribute classes:
       TAttLine, TAttFill, and TAttMarker.
     See the member functions of these classes for the list of options.

<h4>Giving titles to the X, Y and Z axis</h4>
<pre>
       h->GetXaxis()->SetTitle("X axis title");
       h->GetYaxis()->SetTitle("Y axis title");
</pre>
     The histogram title and the axis titles can be any TLatex string.
     The titles are part of the persistent histogram.
     It is also possible to specify the histogram title and the axis
     titles at creation time. These titles can be given in the "title"
     parameter. They must be separated by ";":
<pre>
        TH1F* h=new TH1F("h", "Histogram title;X Axis;Y Axis;Z Axis", 100, 0, 1);
</pre>
     Any title can be omitted:
<pre>
        TH1F* h=new TH1F("h", "Histogram title;;Y Axis", 100, 0, 1);
        TH1F* h=new TH1F("h", ";;Y Axis", 100, 0, 1);
</pre>
     The method SetTitle has the same syntax:
<pre>
</pre>
        h->SetTitle("Histogram title;Another X title Axis");

<h4>Saving/Reading histograms to/from a ROOT file</h4>

     The following statements create a ROOT file and store an histogram
     on the file. Because TH1 derives from TNamed, the key identifier on
     the file is the histogram name:
<pre>
        TFile f("histos.root", "new");
        TH1F h1("hgaus", "histo from a gaussian", 100, -3, 3);
        h1.FillRandom("gaus", 10000);
        h1->Write();
</pre>
     To read this histogram in another Root session, do:
<pre>
        TFile f("histos.root");
        TH1F *h = (TH1F*)f.Get("hgaus");
</pre>
     One can save all histograms in memory to the file by:
<pre>
        file->Write();
</pre>

<h4>Miscelaneous operations</h4>

<pre>
        TH1::KolmogorovTest(): statistical test of compatibility in shape
                             between two histograms
        TH1::Smooth() smooths the bin contents of a 1-d histogram
        TH1::Integral() returns the integral of bin contents in a given bin range
        TH1::GetMean(int axis) returns the mean value along axis
        TH1::GetRMS(int axis)  returns the sigma distribution along axis
        TH1::GetEntries() returns the number of entries
        TH1::Reset() resets the bin contents and errors of an histogram
</pre>
End_Html */



TF1 *gF1=0;  //left for back compatibility (use TVirtualFitter::GetUserFunc instead)

Int_t  TH1::fgBufferSize   = 1000;
Bool_t TH1::fgAddDirectory = kTRUE;
Bool_t TH1::fgDefaultSumw2 = kFALSE;
Bool_t TH1::fgStatOverflows= kFALSE;

extern void H1InitGaus();
extern void H1InitExpo();
extern void H1InitPolynom();
extern void H1LeastSquareFit(Int_t n, Int_t m, Double_t *a);
extern void H1LeastSquareLinearFit(Int_t ndata, Double_t &a0, Double_t &a1, Int_t &ifail);
extern void H1LeastSquareSeqnd(Int_t n, Double_t *a, Int_t idim, Int_t &ifail, Int_t k, Double_t *b);

// Internal exceptions for the CheckConsistency method
class DifferentDimension: public std::exception {};
class DifferentNumberOfBins: public std::exception {};
class DifferentAxisLimits: public std::exception {};
class DifferentBinLimits: public std::exception {};
class DifferentLabels: public std::exception {};

ClassImp(TH1)

//______________________________________________________________________________
TH1::TH1(): TNamed(), TAttLine(), TAttFill(), TAttMarker()
{
//   -*-*-*-*-*-*-*-*-*Histogram default constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//                     =============================
   fDirectory     = 0;
   fFunctions     = new TList;
   fNcells        = 0;
   fIntegral      = 0;
   fPainter       = 0;
   fEntries       = 0;
   fNormFactor    = 0;
   fTsumw         = fTsumw2=fTsumwx=fTsumwx2=0;
   fMaximum       = -1111;
   fMinimum       = -1111;
   fBufferSize    = 0;
   fBuffer        = 0;
   fBinStatErrOpt = kNormal;
   fXaxis.SetName("xaxis");
   fYaxis.SetName("yaxis");
   fZaxis.SetName("zaxis");
   fXaxis.SetParent(this);
   fYaxis.SetParent(this);
   fZaxis.SetParent(this);
   UseCurrentStyle();
}

//______________________________________________________________________________
TH1::~TH1()
{
//   -*-*-*-*-*-*-*-*-*Histogram default destructor*-*-*-*-*-*-*-*-*-*-*-*-*-*
//                     ============================

   if (!TestBit(kNotDeleted)) {
      return;
   }
   delete[] fIntegral;
   fIntegral = 0;
   delete[] fBuffer;
   fBuffer = 0;
   if (fFunctions) {
      fFunctions->SetBit(kInvalidObject);
      TObject* obj = 0;
      //special logic to support the case where the same object is
      //added multiple times in fFunctions.
      //This case happens when the same object is added with different
      //drawing modes
      //In the loop below we must be careful with objects (eg TCutG) that may
      // have been added to the list of functions of several histograms
      //and may have been already deleted.
      while ((obj  = fFunctions->First())) {
         while(fFunctions->Remove(obj)) { }
         if (!obj->TestBit(kNotDeleted)) {
            break;
         }
         delete obj;
         obj = 0;
      }
      delete fFunctions;
      fFunctions = 0;
   }
   if (fDirectory) {
      fDirectory->Remove(this);
      fDirectory = 0;
   }
   delete fPainter;
   fPainter = 0;
}

//______________________________________________________________________________
TH1::TH1(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
    :TNamed(name,title), TAttLine(), TAttFill(), TAttMarker()
{
//   -*-*-*-*-*-*-*Normal constructor for fix bin size histograms*-*-*-*-*-*-*
//                 ==============================================
//
//     Creates the main histogram structure:
//        name   : name of histogram (avoid blanks)
//        title  : histogram title
//                 if title is of the form "stringt;stringx;stringy;stringz"
//                 the histogram title is set to stringt,
//                 the x axis title to stringy, the y axis title to stringy, etc.
//        nbins  : number of bins
//        xlow   : low edge of first bin
//        xup    : upper edge of last bin (not included in last bin)
//
//      When an histogram is created, it is automatically added to the list
//      of special objects in the current directory.
//      To find the pointer to this histogram in the current directory
//      by its name, do:
//      TH1F *h1 = (TH1F*)gDirectory->FindObject(name);
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   Build();
   if (nbins <= 0) {Warning("TH1","nbins is <=0 - set to nbins = 1"); nbins = 1; }
   fXaxis.Set(nbins,xlow,xup);
   fNcells = fXaxis.GetNbins()+2;
}

//______________________________________________________________________________
TH1::TH1(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
    :TNamed(name,title), TAttLine(), TAttFill(), TAttMarker()
{
//   -*-*-*-*-*Normal constructor for variable bin size histograms*-*-*-*-*-*-*
//             ===================================================
//
//  Creates the main histogram structure:
//     name   : name of histogram (avoid blanks)
//     title  : histogram title
//              if title is of the form "stringt;stringx;stringy;stringz"
//              the histogram title is set to stringt,
//              the x axis title to stringx, the y axis title to stringy, etc.
//     nbins  : number of bins
//     xbins  : array of low-edges for each bin
//              This is an array of size nbins+1
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Build();
   if (nbins <= 0) {Warning("TH1","nbins is <=0 - set to nbins = 1"); nbins = 1; }
   if (xbins) fXaxis.Set(nbins,xbins);
   else       fXaxis.Set(nbins,0,1);
   fNcells = fXaxis.GetNbins()+2;
}

//______________________________________________________________________________
TH1::TH1(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
    :TNamed(name,title), TAttLine(), TAttFill(), TAttMarker()
{
//   -*-*-*-*-*Normal constructor for variable bin size histograms*-*-*-*-*-*-*
//             ===================================================
//
//  Creates the main histogram structure:
//     name   : name of histogram (avoid blanks)
//     title  : histogram title
//              if title is of the form "stringt;stringx;stringy;stringz"
//              the histogram title is set to stringt,
//              the x axis title to stringx, the y axis title to stringy, etc.
//     nbins  : number of bins
//     xbins  : array of low-edges for each bin
//              This is an array of size nbins+1
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Build();
   if (nbins <= 0) {Warning("TH1","nbins is <=0 - set to nbins = 1"); nbins = 1; }
   if (xbins) fXaxis.Set(nbins,xbins);
   else       fXaxis.Set(nbins,0,1);
   fNcells = fXaxis.GetNbins()+2;
}

//______________________________________________________________________________
TH1::TH1(const TH1 &h) : TNamed(), TAttLine(), TAttFill(), TAttMarker()
{
   // Copy constructor.
   // The list of functions is not copied. (Use Clone if needed)

   ((TH1&)h).Copy(*this);
}

//______________________________________________________________________________
Bool_t TH1::AddDirectoryStatus()
{
   //static function: cannot be inlined on Windows/NT
   return fgAddDirectory;
}

//______________________________________________________________________________
void TH1::Browse(TBrowser *b)
{
   // Browe the Histogram object.

   Draw(b ? b->GetDrawOption() : "");
   gPad->Update();
}


//______________________________________________________________________________
void TH1::Build()
{
//   -*-*-*-*-*-*-*-*Creates histogram basic data structure*-*-*-*-*-*-*-*-*-*
//                   ======================================

   fDirectory     = 0;
   fPainter       = 0;
   fIntegral      = 0;
   fEntries       = 0;
   fNormFactor    = 0;
   fTsumw         = fTsumw2=fTsumwx=fTsumwx2=0;
   fMaximum       = -1111;
   fMinimum       = -1111;
   fBufferSize    = 0;
   fBuffer        = 0;
   fBinStatErrOpt = kNormal;
   fXaxis.SetName("xaxis");
   fYaxis.SetName("yaxis");
   fZaxis.SetName("zaxis");
   fYaxis.Set(1,0.,1.);
   fZaxis.Set(1,0.,1.);
   fXaxis.SetParent(this);
   fYaxis.SetParent(this);
   fZaxis.SetParent(this);

   SetTitle(fTitle.Data());

   fFunctions = new TList;

   UseCurrentStyle();

   if (TH1::AddDirectoryStatus()) {
      fDirectory = gDirectory;
      if (fDirectory) {
         fDirectory->Append(this,kTRUE);
      }
   }
}

//______________________________________________________________________________
Bool_t TH1::Add(TF1 *f1, Double_t c1, Option_t *option)
{
// Performs the operation: this = this + c1*f1
// if errors are defined (see TH1::Sumw2), errors are also recalculated.
//
// By default, the function is computed at the centre of the bin.
// if option "I" is specified (1-d histogram only), the integral of the
// function in each bin is used instead of the value of the function at
// the centre of the bin.
// Only bins inside the function range are recomputed.
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Add
//
// The function return kFALSE if the Add operation failed

   if (!f1) {
      Error("Add","Attempt to add a non-existing function");
      return kFALSE;
   }

   TString opt = option;
   opt.ToLower();
   Bool_t integral = kFALSE;
   if (opt.Contains("i") && fDimension ==1) integral = kTRUE;

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();
   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

//   - Add statistics
   Double_t s1[10];
   Int_t i;
   for (i=0;i<10;i++) {s1[i] = 0;}
   PutStats(s1);
   SetMinimum();
   SetMaximum();

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t cu=0;
   Double_t xx[3];
   Double_t *params = 0;
   f1->InitArgs(xx,params);
   for (binz=0;binz<=nbinsz+1;binz++) {
      xx[2] = fZaxis.GetBinCenter(binz);
      for (biny=0;biny<=nbinsy+1;biny++) {
         xx[1] = fYaxis.GetBinCenter(biny);
         for (binx=0;binx<=nbinsx+1;binx++) {
            xx[0] = fXaxis.GetBinCenter(binx);
            if (!f1->IsInside(xx)) continue;
            TF1::RejectPoint(kFALSE);
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            if (integral) {
               xx[0] = fXaxis.GetBinLowEdge(binx);
               cu  = c1*f1->EvalPar(xx);
               cu += c1*f1->Integral(fXaxis.GetBinLowEdge(binx),fXaxis.GetBinUpEdge(binx))*fXaxis.GetBinWidth(binx);
            } else {
               cu  = c1*f1->EvalPar(xx);
            }
            if (TF1::RejectedPoint()) continue;
            Double_t error1 = GetBinError(bin);
            AddBinContent(bin,cu);
            if (fSumw2.fN) {
               //errors are unchanged: error on f1 assumed 0
               fSumw2.fArray[bin] = error1*error1;
            }
         }
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TH1::Add(const TH1 *h1, Double_t c1)
{
// Performs the operation: this = this + c1*h1
// if errors are defined (see TH1::Sumw2), errors are also recalculated.
// Note that if h1 has Sumw2 set, Sumw2 is automatically called for this
// if not already set.
// Note also that adding histogram with labels is not supported, histogram will be
// added merging them by bin number independently of the labels.
// For adding histogram with labels one should use TH1::Merge
//
// SPECIAL CASE (Average/Efficiency histograms)
// For histograms representing averages or efficiencies, one should compute the average
// of the two histograms and not the sum. One can mark a histogram to be an average
// histogram by setting its bit kIsAverage with
//    myhist.SetBit(TH1::kIsAverage);
// Note that the two histograms must have their kIsAverage bit set
//
// IMPORTANT NOTE1: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Add
//
// IMPORTANT NOTE2: if h1 has a normalisation factor, the normalisation factor
// is used , ie  this = this + c1*factor*h1
// Use the other TH1::Add function if you do not want this feature
//
// The function return kFALSE if the Add operation failed

   if (!h1) {
      Error("Add","Attempt to add a non-existing histogram");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();

   try {
      CheckConsistency(this,h1);
   } catch(DifferentNumberOfBins&) {
      Error("Add","Attempt to add histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Add","Attempt to add histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Add","Attempt to add histograms with different bin limits");
   } catch(DifferentLabels&) {
      Warning("Add","Attempt to add histograms with different labels");
   }

   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

//    Create Sumw2 if h1 has Sumw2 set
   if (fSumw2.fN == 0 && h1->GetSumw2N() != 0) Sumw2();

//   - Add statistics
   Double_t entries = TMath::Abs( GetEntries() + c1 * h1->GetEntries() );

// statistics can be preserbed only in case of positive coefficients
// otherwise with negative c1 (histogram subtraction) one risks to get negative variances
   Bool_t resetStats = (c1 < 0);
   Double_t s1[kNstat] = {0};
   Double_t s2[kNstat] = {0};
   if (!resetStats) {
      // need to initialize to zero s1 and s2 since
      // GetStats fills only used elements depending on dimension and type
      GetStats(s1);
      h1->GetStats(s2);
   }

   SetMinimum();
   SetMaximum();

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t cu;
   Double_t factor =1;
   if (h1->GetNormFactor() != 0) factor = h1->GetNormFactor()/h1->GetSumOfWeights();;
   for (binz=0;binz<=nbinsz+1;binz++) {
      for (biny=0;biny<=nbinsy+1;biny++) {
         for (binx=0;binx<=nbinsx+1;binx++) {
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            //special case where histograms have the kIsAverage bit set
            if (this->TestBit(kIsAverage) && h1->TestBit(kIsAverage)) {
               Double_t y1 = h1->GetBinContent(bin);
               Double_t y2 = this->GetBinContent(bin);
               Double_t e1 = h1->GetBinError(bin);
               Double_t e2 = this->GetBinError(bin);
               Double_t w1 = 1., w2 = 1.;
               // consider all special cases  when bin errors are zero
               // see http://root.cern.ch/phpBB3//viewtopic.php?f=3&t=13299
               if (e1 > 0 )
                  w1 = 1./(e1*e1);
               else if (h1->fSumw2.fN) {
                  w1 = 1.E200; // use an arbitrary huge value
                  if (y1 == 0) {
                     // use an estimated error from the global histogram scale
                     double sf = (s2[0] != 0) ? s2[1]/s2[0] : 1;
                     w1 = 1./(sf*sf);
                  }
               }
               if (e2 > 0)
                  w2 = 1./(e2*e2);
               else if (fSumw2.fN) {
                  w2 = 1.E200; // use an arbitrary huge value
                  if (y2 == 0) {
                     // use an estimated error from the global histogram scale
                     double sf = (s1[0] != 0) ? s1[1]/s1[0] : 1;
                     w2 = 1./(sf*sf);
                  }
               }
               double y =  (w1*y1 + w2*y2)/(w1 + w2);
               SetBinContent(bin, y);
               if (fSumw2.fN) {
                  double err2 =  1./(w1 + w2);
                  if (err2 < 1.E-200) err2 = 0;  // to remove arbitrary value when e1=0 AND e2=0
                  fSumw2.fArray[bin] = err2;
               }
            }
            //normal case of addition between histograms
            else {
               cu  = c1*factor*h1->GetBinContent(bin);
               AddBinContent(bin,cu);
               if (fSumw2.fN) {
                  Double_t e1 = factor*h1->GetBinError(bin);
                  fSumw2.fArray[bin] += c1*c1*e1*e1;
               }
            }
         }
      }
   }

   // update statistics (do here to avoid changes by SetBinContent)
   if (resetStats)  {
      // statistics need to be reset in case coefficient are negative
      ResetStats();
   }
   else {
      for (Int_t i=0;i<kNstat;i++) {
         if (i == 1) s1[i] += c1*c1*s2[i];
         else        s1[i] += c1*s2[i];
      }
      PutStats(s1);
      SetEntries(entries);
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TH1::Add(const TH1 *h1, const TH1 *h2, Double_t c1, Double_t c2)
{
//   -*-*-*Replace contents of this histogram by the addition of h1 and h2*-*-*
//         ===============================================================
//
//   this = c1*h1 + c2*h2
//   if errors are defined (see TH1::Sumw2), errors are also recalculated
//   Note that if h1 or h2 have Sumw2 set, Sumw2 is automatically called for this
//   if not already set.
//   Note also that adding histogram with labels is not supported, histogram will be
//   added merging them by bin number independently of the labels.
//   For adding histogram ith labels one should use TH1::Merge
//
// SPECIAL CASE (Average/Efficiency histograms)
// For histograms representing averages or efficiencies, one should compute the average
// of the two histograms and not the sum. One can mark a histogram to be an average
// histogram by setting its bit kIsAverage with
//    myhist.SetBit(TH1::kIsAverage);
// Note that the two histograms must have their kIsAverage bit set
//
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Add
//
//ANOTHER SPECIAL CASE : h1 = h2 and c2 < 0
// do a scaling   this = c1 * h1 / (bin Volume)
//
// The function returns kFALSE if the Add operation failed


   if (!h1 || !h2) {
      Error("Add","Attempt to add a non-existing histogram");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Bool_t normWidth = kFALSE;
   if (h1 == h2 && c2 < 0) {c2 = 0; normWidth = kTRUE;}
   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();

   if (h1 != h2) {
      try {
         CheckConsistency(h1,h2);
         CheckConsistency(this,h1);
      } catch(DifferentNumberOfBins&) {
         Error("Add","Attempt to add histograms with different number of bins");
         return kFALSE;
      } catch(DifferentAxisLimits&) {
         Warning("Add","Attempt to add histograms with different axis limits");
      } catch(DifferentBinLimits&) {
         Warning("Add","Attempt to add histograms with different bin limits");
      } catch(DifferentLabels&) {
         Warning("Add","Attempt to add histograms with different labels");
      }
   }

   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

//    Create Sumw2 if h1 or h2 have Sumw2 set
   if (fSumw2.fN == 0 && (h1->GetSumw2N() != 0 || h2->GetSumw2N() != 0)) Sumw2();

//   - Add statistics
   Double_t nEntries = TMath::Abs( c1*h1->GetEntries() + c2*h2->GetEntries() );

// statistics can be preserved only in case of positive coefficients
// otherwise with negative c1 (histogram subtraction) one risks to get negative variances
// also in case of scaling with the width we cannot preserve the statistics
   Double_t s1[kNstat] = {0};
   Double_t s2[kNstat] = {0};
   Double_t s3[kNstat];
   Bool_t resetStats = (c1*c2 < 0) || normWidth;
   if (!resetStats) {
      // need to initialize to zero s1 and s2 since
      // GetStats fills only used elements depending on dimension and type
      h1->GetStats(s1);
      h2->GetStats(s2);
      for (Int_t i=0;i<kNstat;i++) {
         if (i == 1) s3[i] = c1*c1*s1[i] + c2*c2*s2[i];
         //else        s3[i] = TMath::Abs(c1)*s1[i] + TMath::Abs(c2)*s2[i];
         else        s3[i] = c1*s1[i] + c2*s2[i];
      }
   }

   SetMinimum();
   SetMaximum();

//    Reset the kCanRebin and time display option. Otherwise SetBinContent on the overflow bin
//    would resize the axis limits!
// we need to do for only X axis since only TH1x::SetBinContent resize the axis
   Bool_t canRebin = TestBit(kCanRebin);
   if (canRebin) ResetBit(kCanRebin);

   Bool_t timeDisplayX = fXaxis.GetTimeDisplay();
   if (timeDisplayX)  fXaxis.SetTimeDisplay(0);

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t cu;
   for (binz=0;binz<=nbinsz+1;binz++) {
      Double_t wz = h1->GetZaxis()->GetBinWidth(binz);
      for (biny=0;biny<=nbinsy+1;biny++) {
         Double_t wy = h1->GetYaxis()->GetBinWidth(biny);
         for (binx=0;binx<=nbinsx+1;binx++) {
            Double_t wx = h1->GetXaxis()->GetBinWidth(binx);
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            //special case where histograms have the kIsAverage bit set
            if (h1->TestBit(kIsAverage) && h2->TestBit(kIsAverage)) {
               Double_t y1 = h1->GetBinContent(bin);
               Double_t y2 = h2->GetBinContent(bin);
               Double_t e1 = h1->GetBinError(bin);
               Double_t e2 = h2->GetBinError(bin);
               Double_t w1 = 1., w2 = 1.;
               // consider all special cases  when bin errors are zero
               // see http://root.cern.ch/phpBB3//viewtopic.php?f=3&t=13299
               if (e1 > 0 )
                  w1 = 1./(e1*e1);
               else if (h1->fSumw2.fN) {
                  w1 = 1.E200; // use an arbitrary huge value
                  if (y1 == 0 ) {
                     // use an estimated error from the global histogram scale
                     double sf = (s1[0] != 0) ? s1[1]/s1[0] : 1;
                     w1 = 1./(sf*sf);
                  }
               }
               if (e2 > 0)
                  w2 = 1./(e2*e2);
               else if (h2->fSumw2.fN) {
                  w2 = 1.E200; // use an arbitrary huge value
                  if (y2 == 0) {
                     // use an estimated error from the global histogram scale
                     double sf = (s2[0] != 0) ? s2[1]/s2[0] : 1;
                     w2 = 1./(sf*sf);
                  }
               }
               double y =  (w1*y1 + w2*y2)/(w1 + w2);
               SetBinContent(bin, y);
               if (fSumw2.fN) {
                  double err2 =  1./(w1 + w2);
                  if (err2 < 1.E-200) err2 = 0;  // to remove arbitrary value when e1=0 AND e2=0
                  fSumw2.fArray[bin] = err2;
               }
            }

            // case of histogram Addition
            else {
               if (normWidth) {
                  Double_t w = wx*wy*wz;
                  cu  = c1*h1->GetBinContent(bin)/w;
                  SetBinContent(bin,cu);
                  if (fSumw2.fN) {
                     Double_t e1 = h1->GetBinError(bin)/w;
                     fSumw2.fArray[bin] = c1*c1*e1*e1;
                  }
               } else {
                  cu  = c1*h1->GetBinContent(bin)+ c2*h2->GetBinContent(bin);
                  SetBinContent(bin,cu);
                  if (fSumw2.fN) {
                     Double_t e1 = h1->GetBinError(bin);
                     Double_t e2 = h2->GetBinError(bin);
                     fSumw2.fArray[bin] = c1*c1*e1*e1 + c2*c2*e2*e2;
                  }
               }
            }
         }
      }
   }
   if (resetStats)  {
      // statistics need to be reset in case coefficient are negative
      ResetStats();
   }
   else {
      // update statistics (do here to avoid changes by SetBinContent)
      PutStats(s3);
      SetEntries(nEntries);
   }

   if (canRebin) SetBit(kCanRebin);
   if (timeDisplayX)  fXaxis.SetTimeDisplay(1);

   return kTRUE;
}


//______________________________________________________________________________
void TH1::AddBinContent(Int_t)
{
//   -*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
//                   ==========================
   AbstractMethod("AddBinContent");
}

//______________________________________________________________________________
void TH1::AddBinContent(Int_t, Double_t)
{
//   -*-*-*-*-*-*-*-*Increment bin content by a weight w*-*-*-*-*-*-*-*-*-*-*
//                   ===================================
   AbstractMethod("AddBinContent");
}

//______________________________________________________________________________
void TH1::AddDirectory(Bool_t add)
{
// Sets the flag controlling the automatic add of histograms in memory
//
// By default (fAddDirectory = kTRUE), histograms are automatically added
// to the list of objects in memory.
// Note that one histogram can be removed from its support directory
// by calling h->SetDirectory(0) or h->SetDirectory(dir) to add it
// to the list of objects in the directory dir.
//
//  NOTE that this is a static function. To call it, use;
//     TH1::AddDirectory

   fgAddDirectory = add;
}


//______________________________________________________________________________
Int_t TH1::BufferEmpty(Int_t action)
{
// Fill histogram with all entries in the buffer.
// action = -1 histogram is reset and refilled from the buffer (called by THistPainter::Paint)
// action =  0 histogram is reset and filled from the buffer. When the histogram is filled from the
//             buffer the value fBuffer[0] is set to a negative number (= - number of entries)
//             When calling with action == 0 the histogram is NOT refilled when fBuffer[0] is < 0
//             While when calling with action = -1 the histogram is reset and ALWAYS refilled independently if
//             the histogram was filled before. This is needed when drawing the histogram
//
// action =  1 histogram is filled and buffer is deleted
//             The buffer is automatically deleted when filling the histogram and the entries is
//             larger than the buffer size
//

   // do we need to compute the bin size?
   if (!fBuffer) return 0;
   Int_t nbentries = (Int_t)fBuffer[0];

   // nbentries correspond to the number of entries of histogram

   if (nbentries == 0) return 0;
   if (nbentries < 0 && action == 0) return 0;    // case histogram has been already filled from the buffer

   Double_t *buffer = fBuffer;
   if (nbentries < 0) {
      nbentries  = -nbentries;
      //  a reset might call BufferEmpty() giving an infinite loop
      // Protect it by setting fBuffer = 0
      fBuffer=0;
       //do not reset the list of functions
      Reset("ICES");
      fBuffer = buffer;
   }
   if (TestBit(kCanRebin) || (fXaxis.GetXmax() <= fXaxis.GetXmin())) {
      //find min, max of entries in buffer
      Double_t xmin = fBuffer[2];
      Double_t xmax = xmin;
      for (Int_t i=1;i<nbentries;i++) {
         Double_t x = fBuffer[2*i+2];
         if (x < xmin) xmin = x;
         if (x > xmax) xmax = x;
      }
      if (fXaxis.GetXmax() <= fXaxis.GetXmin()) {
         THLimitsFinder::GetLimitsFinder()->FindGoodLimits(this,xmin,xmax);
      } else {
         fBuffer = 0;
         Int_t keep = fBufferSize; fBufferSize = 0;
         if (xmin <  fXaxis.GetXmin()) RebinAxis(xmin,&fXaxis);
         if (xmax >= fXaxis.GetXmax()) RebinAxis(xmax,&fXaxis);
         fBuffer = buffer;
         fBufferSize = keep;
      }
   }

   // call DoFillN which will not put entries in the buffer as FillN does
   // to avoid infinite loops
   // caused by FindBin->ReBinAxis->GetBinCOntent->BufferEmpy
   buffer = fBuffer; fBuffer = 0; 
   DoFillN(nbentries,&buffer[2],&buffer[1],2);
   fBuffer = buffer; 

   // if action == 1 - delete the buffer
   if (action > 0) {
      delete [] fBuffer;
      fBuffer = 0;
      fBufferSize = 0;}
   else {
      // if number of entries is consistent with buffer - set it negative to avoid
      // refilling the histogram every time BufferEmpty(0) is called
      // In case it is not consistent, by setting fBuffer[0]=0 is like resetting the buffer
      // (it will not be used anymore the next time BufferEmpty is called)
      if (nbentries == (Int_t)fEntries)
         fBuffer[0] = -nbentries;
      else
         fBuffer[0] = 0;
   }
   return nbentries;
}

//______________________________________________________________________________
Int_t TH1::BufferFill(Double_t x, Double_t w)
{
// accumulate arguments in buffer. When buffer is full, empty the buffer
// fBuffer[0] = number of entries in buffer
// fBuffer[1] = w of first entry
// fBuffer[2] = x of first entry

   if (!fBuffer) return -2;
   Int_t nbentries = (Int_t)fBuffer[0];


   if (nbentries < 0) {
      // reset nbentries to a positive value so next time BufferEmpty()  is called
      // the histogram will be refilled
      nbentries  = -nbentries;
      fBuffer[0] =  nbentries;
      if (fEntries > 0) {
         // set fBuffer to zero to avoid calling BufferEmpty in Reset
         Double_t *buffer = fBuffer; fBuffer=0;
         Reset("ICES");  // do not reset list of functions
         fBuffer = buffer;
      }
   }
   if (2*nbentries+2 >= fBufferSize) {
      BufferEmpty(1);
      return Fill(x,w);
   }
   fBuffer[2*nbentries+1] = w;
   fBuffer[2*nbentries+2] = x;
   fBuffer[0] += 1;
   return -2;
}

bool TH1::CheckBinLimits(const TAxis* a1, const TAxis * a2)
{

   const TArrayD * h1Array = a1->GetXbins();
   const TArrayD * h2Array = a2->GetXbins();
   Int_t fN = h1Array->fN;
   if ( fN != 0 ) {
      if ( h2Array->fN != fN ) {
         throw DifferentBinLimits();
         return false;
      }
      else {
         for ( int i = 0; i < fN; ++i ) {
            if ( ! TMath::AreEqualRel( h1Array->GetAt(i), h2Array->GetAt(i), 1E-10 ) ) {
               throw DifferentBinLimits();
               return false;
            }
         }
      }
   }

   return true;
}

bool TH1::CheckBinLabels(const TAxis* a1, const TAxis * a2)
{
   // check that axis have same labels
   THashList *l1 = (const_cast<TAxis*>(a1))->GetLabels();
   THashList *l2 = (const_cast<TAxis*>(a2))->GetLabels();

   if (!l1 && !l2 )
      return true;
   if (!l1 ||  !l2 ) {
      throw DifferentLabels();
      return false;
   }
   // check now labels sizes  are the same
   if (l1->GetSize() != l2->GetSize() ) {
      throw DifferentLabels();
      return false;
   }
   for (int i = 1; i <= a1->GetNbins(); ++i) {
      TString label1 = a1->GetBinLabel(i);
      TString label2 = a2->GetBinLabel(i);
      if (label1 != label2) {
         throw DifferentLabels();
         return false;
      }
   }

   return true;
}


bool TH1::CheckAxisLimits(const TAxis *a1, const TAxis *a2 )
{
   // Check that the axis limits of the histograms are the same
   // if a first and last bin is passed the axis is compared between the given range

   if ( ! TMath::AreEqualRel(a1->GetXmin(), a2->GetXmin(),1.E-12) ||
        ! TMath::AreEqualRel(a1->GetXmax(), a2->GetXmax(),1.E-12) ) {
      throw DifferentAxisLimits();
      return false;
   }
   return true;
}

bool TH1::CheckEqualAxes(const TAxis *a1, const TAxis *a2 )
{
   // Check that the axis are the same
   if (a1->GetNbins() != a2->GetNbins() ) {
      //throw DifferentNumberOfBins();
      ::Info("CheckEqualAxes","Axes have different number of bins : nbin1 = %d nbin2 = %d",a1->GetNbins(),a2->GetNbins() );
      return false;
   }
   try {
      CheckAxisLimits(a1,a2);
   } catch (DifferentAxisLimits&) {
      ::Info("CheckEqualAxes","Axes have different limits");
      return false;
   }
   try {
      CheckBinLimits(a1,a2);
   } catch (DifferentBinLimits&) {
      ::Info("CheckEqualAxes","Axes have different bin limits");
      return false;
   }

   // check labels
   try {
      CheckBinLabels(a1,a2);
   } catch (DifferentLabels&) {
      ::Info("CheckEqualAxes","Axes have different labels");
      return false;
   }

   return true;
}

bool TH1::CheckConsistentSubAxes(const TAxis *a1, Int_t firstBin1, Int_t lastBin1, const TAxis * a2, Int_t firstBin2, Int_t lastBin2 )
{
   // Check that two sub axis are the same
   // the limits are defined by first bin and last bin
   // N.B. no check is done in this case for variable bins

   // By default is assumed that no bins are given for the second axis
   Int_t nbins1   = lastBin1-firstBin1 + 1;
   Double_t xmin1 = a1->GetBinLowEdge(firstBin1);
   Double_t xmax1 = a1->GetBinUpEdge(lastBin1);

   Int_t nbins2 = a2->GetNbins();
   Double_t xmin2 = a2->GetXmin();
   Double_t xmax2 = a2->GetXmax();

   if (firstBin2 <  lastBin2) {
      // in this case assume no bins are given for the second axis
      nbins2   = lastBin1-firstBin1 + 1;
      xmin2 = a1->GetBinLowEdge(firstBin1);
      xmax2 = a1->GetBinUpEdge(lastBin1);
   }

   if (nbins1 != nbins2 ) {
      ::Info("CheckConsistentSubAxes","Axes have different number of bins");
      return false;
   }

   if ( ! TMath::AreEqualRel(xmin1,xmin2,1.E-12) ||
        ! TMath::AreEqualRel(xmax1,xmax2,1.E-12) ) {
      ::Info("CheckConsistentSubAxes","Axes have different limits");
      return false;
   }

   return true;
}

//___________________________________________________________________________
bool TH1::CheckConsistency(const TH1* h1, const TH1* h2)
{
   // Check histogram compatibility
   if (h1 == h2) return true;

   if (h1->GetDimension() != h2->GetDimension() ) { 
      throw DifferentDimension();
      return false;
   }
   Int_t dim = h1->GetDimension(); 
 
   // returns kTRUE if number of bins and bin limits are identical
   Int_t nbinsx = h1->GetNbinsX();
   Int_t nbinsy = h1->GetNbinsY();
   Int_t nbinsz = h1->GetNbinsZ();

   // Check whether the histograms have the same number of bins.
   if (nbinsx != h2->GetNbinsX() || 
       (dim > 1 && nbinsy != h2->GetNbinsY())  || 
       (dim > 2 && nbinsz != h2->GetNbinsZ()) ) {
      throw DifferentNumberOfBins();
      return false;
   }

   bool ret = true;

   // check axis limits
   ret &= CheckAxisLimits(h1->GetXaxis(), h2->GetXaxis());
   if (dim > 1) ret &= CheckAxisLimits(h1->GetYaxis(), h2->GetYaxis());
   if (dim > 2) ret &= CheckAxisLimits(h1->GetZaxis(), h2->GetZaxis());

   // check bin limits
   ret &= CheckBinLimits(h1->GetXaxis(), h2->GetXaxis());
   if (dim > 1) ret &= CheckBinLimits(h1->GetYaxis(), h2->GetYaxis());
   if (dim > 2) ret &= CheckBinLimits(h1->GetZaxis(), h2->GetZaxis());

   // check labels if histograms are both not empty
   if ( (h1->fTsumw != 0 || h1->GetEntries() != 0) &&
        (h2->fTsumw != 0 || h2->GetEntries() != 0) ) {
      ret &= CheckBinLabels(h1->GetXaxis(), h2->GetXaxis());
      if (dim > 1) ret &= CheckBinLabels(h1->GetYaxis(), h2->GetYaxis());
      if (dim > 2) ret &= CheckBinLabels(h1->GetZaxis(), h2->GetZaxis());
   }

   return ret;
}

//___________________________________________________________________________
Double_t TH1::Chi2Test(const TH1* h2, Option_t *option, Double_t *res) const
{
   // Begin_Latex #chi^{2} End_Latex test for comparing weighted and unweighted histograms
   //
   // Function: Returns p-value. Other return values are specified by the 3rd parameter <br>
   //
   // Parameters:
   //
   //    - h2: the second histogram
   //    - option:
   //       o "UU" = experiment experiment comparison (unweighted-unweighted)
   //       o "UW" = experiment MC comparison (unweighted-weighted). Note that
   //          the first histogram should be unweighted
   //       o "WW" = MC MC comparison (weighted-weighted)
   //       o "NORM" = to be used when one or both of the histograms is scaled
   //                  but the histogram originally was unweighted
   //       o by default underflows and overlows are not included:
   //          * "OF" = overflows included
   //          * "UF" = underflows included
   //       o "P" = print chi2, ndf, p_value, igood
   //       o "CHI2" = returns chi2 instead of p-value
   //       o "CHI2/NDF" = returns Begin_Latex #chi^{2}/ndf End_Latex
   //    - res: not empty - computes normalized residuals and returns them in
   //      this array
   //
   // The current implementation is based on the papers Begin_Latex #chi^{2} End_Latex test for comparison
   // of weighted and unweighted histograms" in Proceedings of PHYSTAT05 and
   // "Comparison weighted and unweighted histograms", arXiv:physics/0605123
   // by N.Gagunashvili. This function has been implemented by Daniel Haertl in August 2006.
   //
   // Introduction:
   //
   //   A frequently used technique in data analysis is the comparison of
   //   histograms. First suggested by Pearson [1] the Begin_Latex #chi^{2} End_Latex test of
   //   homogeneity is used widely for comparing usual (unweighted) histograms.
   //   This paper describes the implementation modified Begin_Latex #chi^{2} End_Latex tests
   //   for comparison of weighted and unweighted  histograms and two weighted
   //   histograms [2] as well as usual Pearson's Begin_Latex #chi^{2} End_Latex test for
   //   comparison two usual (unweighted) histograms.
   //
   // Overview:
   //
   //   Comparison of two histograms expect hypotheses that two histograms
   //   represent identical distributions. To make a decision p-value should
   //   be calculated. The hypotheses of identity is rejected if the p-value is
   //   lower then some significance level. Traditionally significance levels
   //   0.1, 0.05 and 0.01 are used. The comparison procedure should include an
   //   analysis of the residuals which is often helpful in identifying the
   //   bins of histograms responsible for a significant overall Begin_Latex #chi^{2} End_Latex value.
   //   Residuals are the difference between bin contents and expected bin
   //   contents. Most convenient for analysis are the normalized residuals. If
   //   hypotheses of identity are valid then normalized residuals are
   //   approximately independent and identically distributed random variables
   //   having N(0,1) distribution. Analysis of residuals expect test of above
   //   mentioned properties of residuals. Notice that indirectly the analysis
   //   of residuals increase the power of Begin_Latex #chi^{2} End_Latex test.
   //
   // Methods of comparison:
   //
   //  Begin_Latex #chi^{2} End_Latex test for comparison two (unweighted) histograms:
   //   Let us consider two  histograms with the  same binning and the  number
   //   of bins equal to r. Let us denote the number of events in the ith bin
   //   in the first histogram as ni and as mi in the second one. The total
   //   number of events in the first histogram is equal to:
   //Begin_Latex
   //   N = #sum_{i=1}^{r} n_{i}
   //End_Latex
   //   and
   //Begin_Latex
   //   M = #sum_{i=1}^{r} m_{i}
   //End_Latex
   //   in the second histogram. The hypothesis of identity (homogeneity) [3]
   //   is that the two histograms represent random values with identical
   //   distributions. It is equivalent that there exist r constants p1,...,pr,
   //   such that
   //Begin_Latex
   //   #sum_{i=1}^{r} p_{i}=1
   //End_Latex
   //    and the probability of belonging to the ith bin for some measured value
   //    in both experiments is equal to pi. The number of events in the ith
   //    bin is a random variable with a distribution approximated by a Poisson
   //    probability distribution
   //Begin_Latex
   //   #frac{e^{-Np_{i}}(Np_{i})^{n_{i}}}{n_{i}!}
   //End_Latex
   //   for the first histogram and with distribution
   //Begin_Latex
   //   #frac{e^{-Mp_{i}}(Mp_{i})^{m_{i}}}{m_{i}!}
   //End_Latex
   //   for the second histogram. If the hypothesis of homogeneity is valid,
   //   then the  maximum likelihood estimator of pi, i=1,...,r, is
   //Begin_Latex
   //   #hat{p}_{i}= #frac{n_{i}+m_{i}}{N+M}
   //End_Latex
   //   and then
   //Begin_Latex
   //   X^{2} = #sum_{i=1}^{r}#frac{(n_{i}-N#hat{p}_{i})^{2}}{N#hat{p}_{i}} + #sum_{i=1}^{r}#frac{(m_{i}-M#hat{p}_{i})^{2}}{M#hat{p}_{i}} = #frac{1}{MN} #sum_{i=1}^{r}#frac{(Mn_{i}-Nm_{i})^{2}}{n_{i}+m_{i}}
   //End_Latex
   //   has approximately a Begin_Latex #chi^{2}_{(r-1)} End_Latex distribution [3].
   //   The comparison procedure can include an analysis of the residuals which
   //   is often helpful in identifying the bins of histograms responsible for
   //   a significant overall Begin_Latex #chi^{2} End_Latexvalue. Most convenient for
   //   analysis are the adjusted (normalized) residuals [4]
   //Begin_Latex
   //   r_{i} = #frac{n_{i}-N#hat{p}_{i}}{#sqrt{N#hat{p}_{i}}#sqrt{(1-N/(N+M))(1-(n_{i}+m_{i})/(N+M))}}
   //End_Latex
   //   If hypotheses of  homogeneity are valid then residuals ri are
   //   approximately independent and identically distributed random variables
   //   having N(0,1) distribution. The application of the Begin_Latex #chi^{2} End_latex test has
   //   restrictions related to the value of the expected frequencies Npi,
   //   Mpi, i=1,...,r. A conservative rule formulated in [5] is that all the
   //   expectations must be 1 or greater for both histograms. In practical
   //   cases when expected frequencies are not known the estimated expected
   //   frequencies Begin_Latex M#hat{p}_{i}, N#hat{p}_{i}, i=1,...,r End_Latex  can be used.
   //
   //  Unweighted and weighted histograms comparison:
   //
   //   A simple modification of the ideas described above can be used for the
   //   comparison of the usual (unweighted) and weighted histograms. Let us
   //   denote the number of events in the ith bin in the unweighted
   //   histogram as ni and the common weight of events in the ith bin of the
   //   weighted histogram as wi. The total number of events in the
   //   unweighted histogram is equal to
   //Begin_Latex
   //   N = #sum_{i=1}^{r} n_{i}
   //End_Latex
   //   and the total weight of events in the weighted histogram is equal to
   //Begin_Latex
   //   W = #sum_{i=1}^{r} w_{i}
   //End_Latex
   //   Let us formulate the hypothesis of identity of an unweighted histogram
   //   to a weighted histogram so that there exist r constants p1,...,pr, such
   //   that
   //Begin_Latex
   //   #sum_{i=1}^{r} p_{i} = 1
   //End_Latex
   //   for the unweighted histogram. The weight wi is a random variable with a
   //   distribution approximated by the normal probability distribution
   //   Begin_Latex N(Wp_{i},#sigma_{i}^{2}) End_Latex where Begin_Latex #sigma_{i}^{2} End_Latex is the variance of the weight wi.
   //   If we replace the variance Begin_Latex #sigma_{i}^{2} End_Latex
   //   with estimate Begin_Latex s_{i}^{2} End_Latex (sum of squares of weights of
   //   events in the ith bin) and the hypothesis of identity is valid, then the
   //   maximum likelihood estimator of  pi,i=1,...,r, is
   //Begin_Latex
   //   #hat{p}_{i} = #frac{Ww_{i}-Ns_{i}^{2}+#sqrt{(Ww_{i}-Ns_{i}^{2})^{2}+4W^{2}s_{i}^{2}n_{i}}}{2W^{2}}
   //End_Latex
   //   We may then use the test statistic
   //Begin_Latex
   //   X^{2} = #sum_{i=1}^{r} #frac{(n_{i}-N#hat{p}_{i})^{2}}{N#hat{p}_{i}} + #sum_{i=1}^{r} #frac{(w_{i}-W#hat{p}_{i})^{2}}{s_{i}^{2}}
   //End_Latex
   //   and it has approximately a Begin_Latex #chi^{2}_{(r-1)} End_Latex distribution [2]. This test, as well
   //   as the original one [3], has a restriction on the expected frequencies. The
   //   expected frequencies recommended for the weighted histogram is more than 25.
   //   The value of the minimal expected frequency can be decreased down to 10 for
   //   the case when the weights of the events are close to constant. In the case
   //   of a weighted histogram if the number of events is unknown, then we can
   //   apply this recommendation for the equivalent number of events as
   //Begin_Latex
   //   n_{i}^{equiv} = #frac{ w_{i}^{2} }{ s_{i}^{2} }
   //End_Latex
   //   The minimal expected frequency for an unweighted histogram must be 1. Notice
   //   that any usual (unweighted) histogram can be considered as a weighted
   //   histogram with events that have constant weights equal to 1.
   //   The variance Begin_Latex z_{i}^{2} End_Latex of the difference between the weight wi
   //   and the estimated expectation value of the weight is approximately equal to:
   //Begin_Latex
   //   z_{i}^{2} = Var(w_{i}-W#hat{p}_{i}) = N#hat{p}_{i}(1-N#hat{p}_{i})#left(#frac{Ws_{i}^{2}}{#sqrt{(Ns_{i}^{2}-w_{i}W)^{2}+4W^{2}s_{i}^{2}n_{i}}}#right)^{2}+#frac{s_{i}^{2}}{4}#left(1+#frac{Ns_{i}^{2}-w_{i}W}{#sqrt{(Ns_{i}^{2}-w_{i}W)^{2}+4W^{2}s_{i}^{2}n_{i}}}#right)^{2}
   //End_Latex
   //   The  residuals
   //Begin_Latex
   //   r_{i} = #frac{w_{i}-W#hat{p}_{i}}{z_{i}}
   //End_Latex
   //   have approximately a normal distribution with mean equal to 0 and standard
   //   deviation  equal to 1.
   //
   //  Two weighted histograms comparison:
   //
   //   Let us denote the common  weight of events of the ith bin in the first
   //   histogram as w1i and as w2i in the second one. The total weight of events
   //   in the first histogram is equal to
   //Begin_Latex
   //   W_{1} = #sum_{i=1}^{r} w_{1i}
   //End_Latex
   //   and
   //Begin_Latex
   //   W_{2} = #sum_{i=1}^{r} w_{2i}
   //End_Latex
   //   in the second histogram. Let us formulate the hypothesis of identity of
   //   weighted histograms so that there exist r constants p1,...,pr, such that
   //Begin_Latex
   //   #sum_{i=1}^{r} p_{i} = 1
   //End_Latex
   //   and also expectation value of weight w1i equal to W1pi and expectation value
   //   of weight w2i equal to W2pi. Weights in both the histograms are random
   //   variables with distributions which can be approximated by a normal
   //   probability distribution Begin_Latex N(W_{1}p_{i},#sigma_{1i}^{2}) End_Latex for the first histogram
   //   and by a distribution Begin_Latex N(W_{2}p_{i},#sigma_{2i}^{2}) End_Latex for the second.
   //   Here Begin_Latex #sigma_{1i}^{2} End_Latex and Begin_Latex #sigma_{2i}^{2} End_Latex are the variances
   //   of w1i and w2i with estimators Begin_Latex s_{1i}^{2} End_Latex and Begin_Latex s_{2i}^{2} End_Latex respectively.
   //   If the hypothesis of identity is valid, then the maximum likelihood and
   //   Least Square Method estimator of pi,i=1,...,r, is
   //Begin_Latex
   //   #hat{p}_{i} = #frac{w_{1i}W_{1}/s_{1i}^{2}+w_{2i}W_{2} /s_{2i}^{2}}{W_{1}^{2}/s_{1i}^{2}+W_{2}^{2}/s_{2i}^{2}}
   //End_Latex
   //   We may then use the test statistic
   //Begin_Latex
   //   X^{2} = #sum_{i=1}^{r} #frac{(w_{1i}-W_{1}#hat{p}_{i})^{2}}{s_{1i}^{2}} + #sum_{i=1}^{r} #frac{(w_{2i}-W_{2}#hat{p}_{i})^{2}}{s_{2i}^{2}} = #sum_{i=1}^{r} #frac{(W_{1}w_{2i}-W_{2}w_{1i})^{2}}{W_{1}^{2}s_{2i}^{2}+W_{2}^{2}s_{1i}^{2}}
   //End_Latex
   //   and it has approximately a Begin_Latex #chi^{2}_{(r-1)} End_Latex distribution [2].
   //   The normalized or studentised residuals [6]
   //Begin_Latex
   //   r_{i} = #frac{w_{1i}-W_{1}#hat{p}_{i}}{s_{1i}#sqrt{1 - #frac{1}{(1+W_{2}^{2}s_{1i}^{2}/W_{1}^{2}s_{2i}^{2})}}}
   //End_Latex
   //   have approximately a normal distribution with mean equal to 0 and standard
   //   deviation 1. A recommended minimal expected frequency is equal to 10 for
   //   the proposed test.
   //
   // Numerical examples:
   //
   //   The method described herein is now illustrated with an example.
   //   We take a distribution
   //Begin_Latex
   //   #phi(x) = #frac{2}{(x-10)^{2}+1} + #frac{1}{(x-14)^{2}+1}       (1)
   //End_Latex
   //   defined on the interval [4,16]. Events distributed according to the formula
   //   (1) are simulated to create the unweighted histogram. Uniformly distributed
   //   events are simulated for the weighted histogram with weights calculated by
   //   formula (1). Each histogram has the same number of bins: 20. Fig.1 shows
   //   the result of comparison of the unweighted histogram with 200 events
   //   (minimal expected frequency equal to one) and the weighted histogram with
   //   500 events (minimal expected frequency equal to 25)
   //Begin_Macro
   // ../../../tutorials/math/chi2test.C
   //End_Macro
   //   Fig 1. An example of comparison of the unweighted histogram with 200 events
   //   and the weighted histogram with 500 events:
   //      a) unweighted histogram;
   //      b) weighted histogram;
   //      c) normalized residuals plot;
   //      d) normal Q-Q plot of residuals.
   //
   //   The value of the test statistic Begin_Latex #chi^{2} End_Latex is equal to
   //   21.09 with p-value equal to 0.33, therefore the hypothesis of identity of
   //   the two histograms can be accepted for 0.05 significant level. The behavior
   //   of the normalized residuals plot (see Fig. 1c) and the normal Q-Q plot
   //   (see Fig. 1d) of residuals are regular and we cannot identify the outliers
   //   or bins with a big influence on Begin_Latex #chi^{2} End_Latex.
   //
   //   The second example presents the same two histograms but 17 events was added
   //   to content of bin number 15 in unweighted histogram. Fig.2 shows the result
   //   of comparison of the unweighted histogram with 217 events (minimal expected
   //   frequency equal to one) and the weighted histogram with 500 events (minimal
   //   expected frequency equal to 25)
   //Begin_Macro
   // ../../../tutorials/math/chi2test.C(17)
   //End_Macro
   //   Fig 2. An example of comparison of the unweighted histogram with 217 events
   //   and the weighted histogram with 500 events:
   //      a) unweighted histogram;
   //      b) weighted histogram;
   //      c) normalized residuals plot;
   //      d) normal Q-Q plot of residuals.
   //
   //   The value of the test statistic Begin_Latex #chi^{2} End_Latex is equal to
   //   32.33 with p-value equal to 0.029, therefore the hypothesis of identity of
   //   the two histograms is rejected for 0.05 significant level. The behavior of
   //   the normalized residuals plot (see Fig. 2c) and the normal Q-Q plot (see
   //   Fig. 2d) of residuals are not regular and we can identify the outlier or
   //   bin with a big influence on Begin_Latex #chi^{2} End_Latex.
   //
   // References:
   //
   // [1] Pearson, K., 1904. On the Theory of Contingency and Its Relation to
   //     Association and Normal Correlation. Drapers' Co. Memoirs, Biometric
   //     Series No. 1, London.
   // [2] Gagunashvili, N., 2006. Begin_Latex #chi^{2} End_Latex test for comparison
   //     of weighted and unweighted histograms. Statistical Problems in Particle
   //     Physics, Astrophysics and Cosmology, Proceedings of PHYSTAT05,
   //     Oxford, UK, 12-15 September 2005, Imperial College Press, London, 43-44.
   //     Gagunashvili,N., Comparison of weighted and unweighted histograms,
   //     arXiv:physics/0605123, 2006.
   // [3] Cramer, H., 1946. Mathematical methods of statistics.
   //     Princeton University Press, Princeton.
   // [4] Haberman, S.J., 1973. The analysis of residuals in cross-classified tables.
   //     Biometrics 29, 205-220.
   // [5] Lewontin, R.C. and Felsenstein, J., 1965. The robustness of homogeneity
   //     test in 2xN tables. Biometrics 21, 19-33.
   // [6] Seber, G.A.F., Lee, A.J., 2003, Linear Regression Analysis.
   //     John Wiley & Sons Inc., New York.

   Double_t chi2 = 0;
   Int_t ndf = 0, igood = 0;

   TString opt = option;
   opt.ToUpper();

   Double_t prob = Chi2TestX(h2,chi2,ndf,igood,option,res);

   if(opt.Contains("P")) {
      printf("Chi2 = %f, Prob = %g, NDF = %d, igood = %d\n", chi2,prob,ndf,igood);
   }
   if(opt.Contains("CHI2/NDF")) {
      if (ndf == 0) return 0;
      return chi2/ndf;
   }
   if(opt.Contains("CHI2")) {
      return chi2;
   }

   return prob;
}

//___________________________________________________________________________
Double_t TH1::Chi2TestX(const TH1* h2,  Double_t &chi2, Int_t &ndf, Int_t &igood, Option_t *option,  Double_t *res) const
{
   // The computation routine of the Chisquare test. For the method description,
   // see Chi2Test() function.
   // Returns p-value
   // parameters:
   //  - h2-second histogram
   //  - option:
   //     "UU" = experiment experiment comparison (unweighted-unweighted)
   //     "UW" = experiment MC comparison (unweighted-weighted). Note that the first
   //           histogram should be unweighted
   //     "WW" = MC MC comparison (weighted-weighted)
   //
   //     "NORM" = if one or both histograms is scaled
   //
   //     "OF" = overflows included
   //     "UF" = underflows included
   //         by default underflows and overflows are not included
   //
   //  - igood:
   //       igood=0 - no problems
   //        For unweighted unweighted  comparison
   //       igood=1'There is a bin in the 1st histogram with less than 1 event'
   //       igood=2'There is a bin in the 2nd histogram with less than 1 event'
   //       igood=3'when the conditions for igood=1 and igood=2 are satisfied'
   //        For  unweighted weighted  comparison
   //       igood=1'There is a bin in the 1st histogram with less then 1 event'
   //       igood=2'There is a bin in the 2nd histogram with less then 10 effective number of events'
   //       igood=3'when the conditions for igood=1 and igood=2 are satisfied'
   //        For  weighted weighted  comparison
   //       igood=1'There is a bin in the 1st  histogram with less then 10 effective
   //        number of events'
   //       igood=2'There is a bin in the 2nd  histogram with less then 10 effective
   //               number of events'
   //       igood=3'when the conditions for igood=1 and igood=2 are satisfied'
   //
   //  - chi2 - chisquare of the test
   //  - ndf  - number of degrees of freedom (important, when both histograms have the same
   //         empty bins)
   //  - res -  normalized residuals for further analysis


   Int_t i = 0, j=0, k = 0;
   Int_t i_start, i_end;
   Int_t j_start, j_end;
   Int_t k_start, k_end;

   Double_t bin1, bin2;
   Double_t err1,err2;
   Double_t sum1=0, sum2=0;
   Double_t sumw1=0, sumw2=0;


   chi2 = 0;
   ndf = 0;

   TString opt = option;
   opt.ToUpper();

   TAxis *xaxis1 = this->GetXaxis();
   TAxis *xaxis2 = h2->GetXaxis();
   TAxis *yaxis1 = this->GetYaxis();
   TAxis *yaxis2 = h2->GetYaxis();
   TAxis *zaxis1 = this->GetZaxis();
   TAxis *zaxis2 = h2->GetZaxis();
   
   Int_t nbinx1 = xaxis1->GetNbins();
   Int_t nbinx2 = xaxis2->GetNbins();
   Int_t nbiny1 = yaxis1->GetNbins();
   Int_t nbiny2 = yaxis2->GetNbins();
   Int_t nbinz1 = zaxis1->GetNbins();
   Int_t nbinz2 = zaxis2->GetNbins();

   //check dimensions
   if (this->GetDimension() != h2->GetDimension() ){
      Error("Chi2TestX","Histograms have different dimensions.");
      return 0;
   }

   //check number of channels
   if (nbinx1 != nbinx2) {
      Error("Chi2TestX","different number of x channels");
   }
   if (nbiny1 != nbiny2) {
      Error("Chi2TestX","different number of y channels");
   }
   if (nbinz1 != nbinz2) {
      Error("Chi2TestX","different number of z channels");
   }

   //check for ranges
   i_start = j_start = k_start = 1;
   i_end = nbinx1;
   j_end = nbiny1;
   k_end = nbinz1;

   if (xaxis1->TestBit(TAxis::kAxisRange)) {
      i_start = xaxis1->GetFirst();
      i_end   = xaxis1->GetLast();
   }
   if (yaxis1->TestBit(TAxis::kAxisRange)) {
      j_start = yaxis1->GetFirst();
      j_end   = yaxis1->GetLast();
   }
   if (zaxis1->TestBit(TAxis::kAxisRange)) {
      k_start = zaxis1->GetFirst();
      k_end   = zaxis1->GetLast();
   }


   if (opt.Contains("OF")) {
      if (this->GetDimension() == 3) k_end = ++nbinz1;
      if (this->GetDimension() >= 2) j_end = ++nbiny1;
      if (this->GetDimension() >= 1) i_end = ++nbinx1;
   }

   if (opt.Contains("UF")) {
      if (this->GetDimension() == 3) k_start = 0;
      if (this->GetDimension() >= 2) j_start = 0;
      if (this->GetDimension() >= 1) i_start = 0;
   }

   ndf = (i_end - i_start + 1)*(j_end - j_start + 1)*(k_end - k_start + 1) - 1;

   Bool_t comparisonUU = opt.Contains("UU");
   Bool_t comparisonUW = opt.Contains("UW");
   Bool_t comparisonWW = opt.Contains("WW");
   Bool_t scaledHistogram  = opt.Contains("NORM");
   if (scaledHistogram && !comparisonUU) {
      Info("Chi2TestX","NORM option should be used together with UU option. It is ignored");
   }
   // look at histo global bin content and effective entries
   Stat_t s[kNstat];
   this->GetStats(s);// s[1] sum of squares of weights, s[0] sum of weights
   double sumBinContent1 = s[0];
   double effEntries1 = (s[1] ? s[0]*s[0]/s[1] : 0.);

   h2->GetStats(s);// s[1] sum of squares of weights, s[0] sum of weights
   double sumBinContent2 = s[0];
   double effEntries2 = (s[1] ? s[0]*s[0]/s[1] : 0.);

   if (!comparisonUU && !comparisonUW && !comparisonWW ) {
      // deduce automatically from type of histogram
      if (TMath::Abs(sumBinContent1 - effEntries1) < 1) {
         if ( TMath::Abs(sumBinContent2 - effEntries2) < 1) comparisonUU = true;
         else comparisonUW = true;
      }
      else comparisonWW = true;
   }
   // check unweighted histogram
   if (comparisonUW) {
      if (TMath::Abs(sumBinContent1 - effEntries1) >= 1) {
         Warning("Chi2TestX","First histogram is not unweighted and option UW has been requested");
      }
   }
   if ( (!scaledHistogram && comparisonUU)   ) {
      if ( ( TMath::Abs(sumBinContent1 - effEntries1) >= 1) || (TMath::Abs(sumBinContent2 - effEntries2) >= 1) ) {
         Warning("Chi2TestX","Both histograms are not unweighted and option UU has been requested");
      }
   }


   //get number of events in histogram
   if (comparisonUU && scaledHistogram) {
      for (i=i_start; i<=i_end; i++) {
         for (j=j_start; j<=j_end; j++) {
            for (k=k_start; k<=k_end; k++) {
               bin1 = this->GetBinContent(i,j,k);
               bin2 = h2->GetBinContent(i,j,k);
               err1 = this->GetBinError(i,j,k);
               err2 = h2->GetBinError(i,j,k);
               if (err1 > 0 ) {
                  bin1 *= bin1/(err1*err1);
                  //avoid rounding errors
                  bin1 = TMath::Floor(bin1+0.5);
               }
               else
                  bin1 = 0;

               if (err2 > 0) {
                  bin2 *= bin2/(err2*err2);
                  //avoid rounding errors
                  bin2 = TMath::Floor(bin2+0.5);
               }
               else
                  bin2 = 0;

               // sum contents
               sum1 += bin1;
               sum2 += bin2;
               sumw1 += err1*err1;
               sumw2 += err2*err2;
            }
         }
      }
      if (sumw1 <= 0 || sumw2 <= 0) {
         Error("Chi2TestX","Cannot use option NORM when one histogram has all zero errors");
         return 0;
      }

   } else {
      for (i=i_start; i<=i_end; i++) {
         for (j=j_start; j<=j_end; j++) {
            for (k=k_start; k<=k_end; k++) {
               sum1 += this->GetBinContent(i,j,k);
               sum2 += h2->GetBinContent(i,j,k);
               if ( comparisonWW ) {
                  err1 = this->GetBinError(i,j,k);
                  sumw1 += err1*err1;
               }
               if ( comparisonUW || comparisonWW ) {
                  err2 = h2->GetBinError(i,j,k);
                  sumw2 += err2*err2;
               }
            }
         }
      }
   }
   //checks that the histograms are not empty
   if (sum1 == 0 || sum2 == 0) {
      Error("Chi2TestX","one histogram is empty");
      return 0;
   }

   if ( comparisonWW  && ( sumw1 <= 0 && sumw2 <=0 ) ){
      Error("Chi2TestX","Hist1 and Hist2 have both all zero errors\n");
      return 0;
   }

   //THE TEST
   Int_t m=0, n=0;

   //Experiment - experiment comparison
   if (comparisonUU) {
      Double_t sum = sum1 + sum2;
      for (i=i_start; i<=i_end; i++) {
         for (j=j_start; j<=j_end; j++) {
            for (k=k_start; k<=k_end; k++) {
               bin1 = this->GetBinContent(i,j,k);
               bin2 = h2->GetBinContent(i,j,k);


               if (scaledHistogram) {
                  // scale bin value to effective bin entries
                  err1 = this->GetBinError(i,j,k);
                  if (err1 > 0 ) {
                     bin1 *= bin1/(err1*err1);
                     //avoid rounding errors
                     bin1 = TMath::Floor(bin1+0.5);
                  }
                  else
                     bin1 = 0;

                  err2 = h2->GetBinError(i,j,k);
                  if (err2 > 0) {
                     bin2 *= bin2/(err2*err2);
                     //avoid rounding errors
                     bin2 = TMath::Floor(bin2+0.5);
                  }
                  else
                     bin2 = 0;

               }

               if ( (int(bin1) == 0)  && (int(bin2) == 0) ) {
                  --ndf;  //no data means one degree of freedom less
               } else {


                  Double_t binsum = bin1 + bin2;
                  Double_t nexp1 = binsum*sum1/sum;
                  //Double_t nexp2 = binsum*sum2/sum;

//                  if(opt.Contains("P")) printf("bin %d p = %g\t",i,binsum/sum);

                  if (res)
                     res[i-i_start] = (bin1-nexp1)/TMath::Sqrt(nexp1);

                  if (bin1 < 1) {
                     m++;
                  }
                  if (bin2 < 1) {
                     n++;
                  }

                  //Habermann correction for residuals
                  Double_t correc = (1-sum1/sum)*(1-binsum/sum);
                  if (res) {
                     res[i-i_start] /= TMath::Sqrt(correc);
                  }

                  Double_t delta = sum2*bin1-sum1*bin2;
                  chi2 += delta*delta/binsum;

               }
            }
         }
      }

      chi2 /= (sum1*sum2);
      // flag error only when of the two histogram is zero
      if (m) {
         igood += 1;
         Info("Chi2TestX","There is a bin in h1 with less than 1 event.\n");
      }
      if (n) {
         igood += 2;
         Info("Chi2TestX","There is a bin in h2 with less than 1 event.\n");
      }

      Double_t prob = TMath::Prob(chi2,ndf);
      return prob;

   }


   //unweighted - weighted  comparison
   // case of err2 = 0 and bin2 not zero is treated without problems
   // by excluding second chi2 sum
   // and can be considered as a comparison data-theory
   if ( comparisonUW ) {
      for (i=i_start; i<=i_end; i++) {
         for (j=j_start; j<=j_end; j++) {
            for (k=k_start; k<=k_end; k++) {
               Int_t x=0;
               bin1 = this->GetBinContent(i,j,k);
               bin2 = h2->GetBinContent(i,j,k);
               err2 = h2->GetBinError(i,j,k);

               err2 *= err2;

               // case both histogram have zero bin contents
               if ( (int(bin1) == 0) && (bin2*bin2 == 0) ) {
                  --ndf;  //no data means one degree of freedom less
                  continue;
               }

               // case weighted histogram has zero bin content and error
               if (bin2*bin2 == 0 && err2 == 0) {
                  if (sumw2 > 0) {
                     // use as approximated  error as 1 scaled by a scaling ratio
                     // estimated from the total sum weight and sum weight squared
                     err2 = sumw2/sum2;
                  }
                  else {
                     // return error because infinite discrepancy here:
                     // bin1 != 0 and bin2 =0 in a histogram with all errors zero
                     Error("Chi2TestX","Hist2 has in bin %d,%d,%d zero content and all zero errors\n", i,j,k);
                     chi2 = 0; return 0;
                  }
               }

               if (bin1 < 1)  m++;
               if (err2 > 0 && bin2*bin2/err2 < 10) n++;

               Double_t var1 = sum2*bin2 - sum1*err2;
               Double_t var2 = var1*var1 + 4*sum2*sum2*bin1*err2;
               // if bin1 is zero and bin2=1 and sum1=sum2 var1=0 && var2 ==0
               // approximate by adding +1 to bin1
               // LM (this need to be fixed for numerical errors)
               while (var1*var1+bin1 == 0 || var1+var2 == 0) {
                  sum1++;
                  bin1++;
                  x++;
                  var1 = sum2*bin2 - sum1*err2;
                  var2 = var1*var1 + 4*sum2*sum2*bin1*err2;
               }
               var2 = TMath::Sqrt(var2);
               while (var1+var2 == 0) {
                  sum1++;
                  bin1++;
                  x++;
                  var1 = sum2*bin2 - sum1*err2;
                  var2 = var1*var1 + 4*sum2*sum2*bin1*err2;
                  while (var1*var1+bin1 == 0 || var1+var2 == 0) {
                     sum1++;
                     bin1++;
                     x++;
                     var1 = sum2*bin2 - sum1*err2;
                     var2 = var1*var1 + 4*sum2*sum2*bin1*err2;
                  }
                  var2 = TMath::Sqrt(var2);
               }

               Double_t probb = (var1+var2)/(2*sum2*sum2);

               Double_t nexp1 = probb * sum1;
               Double_t nexp2 = probb * sum2;

//               if(opt.Contains("P")) printf("bin %d p = %g\t",i,probb);

               Double_t delta1 = bin1 - nexp1;
               Double_t delta2 = bin2 - nexp2;

               chi2 += delta1*delta1/nexp1;

               if (err2 > 0) {
                  chi2 += delta2*delta2/err2;
               }

               if (res) {
                  if (err2 > 0) {
                     Double_t temp1 = sum2*err2/var2;
                     Double_t temp2 = 1 + (sum1*err2 - sum2*bin2)/var2;
                     temp2 = temp1*temp1*sum1*probb*(1-probb) + temp2*temp2*err2/4;
                     // invert sign here
                     res[i-i_start] = - delta2/TMath::Sqrt(temp2);
                  }
                  else
                     res[i-i_start] = delta1/TMath::Sqrt(nexp1);

               }
            }
         }
      }

      if (m) {
         igood += 1;
         Info("Chi2TestX","There is a bin in h1 with less than 1 event.\n");
      }
      if (n) {
         igood += 2;
         Info("Chi2TestX","There is a bin in h2 with less than 10 effective events.\n");
      }

      Double_t prob = TMath::Prob(chi2,ndf);

      return prob;
   }

   // weighted - weighted  comparison
   if (comparisonWW) {
      for (i=i_start; i<=i_end; i++) {
         for (j=j_start; j<=j_end; j++) {
            for (k=k_start; k<=k_end; k++) {
               bin1 = this->GetBinContent(i,j,k);
               bin2 = h2->GetBinContent(i,j,k);
               err1 = this->GetBinError(i,j,k);
               err2 = h2->GetBinError(i,j,k);
               err1 *= err1;
               err2 *= err2;

               // case both histogram have zero bin contents
               // (use square of bin1 to avoid numerical errors)
                if ( (bin1*bin1 == 0) && (bin2*bin2 == 0) ) {
                   --ndf;  //no data means one degree of freedom less
                   continue;
                }

                if ( (err1 == 0) && (err2 == 0) ) {
                   // cannot treat case of booth histogram have zero zero errors
                  Error("Chi2TestX","h1 and h2 both have bin %d,%d,%d with all zero errors\n", i,j,k);
                  chi2 = 0; return 0;
               }

               Double_t sigma  = sum1*sum1*err2 + sum2*sum2*err1;
               Double_t delta = sum2*bin1 - sum1*bin2;
               chi2 += delta*delta/sigma;

//               if(opt.Contains("P")) printf("bin %d p = %g\t",i, (bin1*sum1/err1 + bin2*sum2/err2)/(sum1*sum1/err1 + sum2*sum2/err2));

               if (res) {
                  Double_t temp = bin1*sum1*err2 + bin2*sum2*err1;
                  Double_t probb = temp/sigma;
                  Double_t z = 0;
                  if (err1 > err2 ) {
                     Double_t d1 = (bin1 - sum1 * probb);
                     Double_t s1 = err1* ( 1. - err2 * sum1 * sum1 / sigma );
                     z = d1/ TMath::Sqrt(s1);
                  }
                  else {
                     Double_t d2 = (bin2 - sum2 * probb);
                     Double_t s2 = err2* ( 1. - err1 * sum2 * sum2 / sigma );
                     z = -d2/ TMath::Sqrt(s2);
                  }

                  res[i-i_start] = z;
               }

               if (err1 > 0 && bin1*bin1/err1 < 10) m++;
               if (err2 > 0 && bin2*bin2/err2 < 10) n++;
            }
         }
      }
      if (m) {
         igood += 1;
         Info("Chi2TestX","There is a bin in h1 with less than 10 effective events.\n");
      }
      if (n) {
         igood += 2;
         Info("Chi2TestX","There is a bin in h2 with less than 10 effective events.\n");
      }
      Double_t prob = TMath::Prob(chi2,ndf);
      return prob;
   }
   return 0;
}
//______________________________________________________________________________
Double_t TH1::Chisquare(TF1 * func, Option_t *option) const
{
   // Compute and return the chisquare of this histogram with respect to a function
   // The chisquare is computed by weighting each histogram point by the bin error
   // By default the full range of the histogram is used. 
   // Use option "R" for restricting the chisquare calculation to the given range of the function

   if (!func) { 
      Error("Chisquare","Function pointer is Null - return -1");
      return -1;
   }

   TString opt(option); opt.ToUpper(); 
   bool useRange = opt.Contains("R");
   
   return ROOT::Fit::Chisquare(*this, *func, useRange);

}
//______________________________________________________________________________
Double_t TH1::ComputeIntegral(Bool_t onlyPositive)
{
   //  Compute integral (cumulative sum of bins)
   //  The result stored in fIntegral is used by the GetRandom functions.
   //  This function is automatically called by GetRandom when the fIntegral
   //  array does not exist or when the number of entries in the histogram
   //  has changed since the previous call to GetRandom.
   //  The resulting integral is normalized to 1
   //  If the routine is called with the onlyPositive flag set an error will 
   //  be produced in case of negative bin content and a NaN value returned

   Int_t bin, binx, biny, binz, ibin;

   // delete previously computed integral (if any)
   if (fIntegral) delete [] fIntegral;

   //   - Allocate space to store the integral and compute integral
   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();
   Int_t nxy    = nbinsx*nbinsy;
   Int_t nbins  = nxy*nbinsz;

   fIntegral = new Double_t[nbins+2];
   ibin = 0;
   fIntegral[ibin] = 0;
   for (binz=1;binz<=nbinsz;binz++) {
      for (biny=1;biny<=nbinsy;biny++) {
         for (binx=1;binx<=nbinsx;binx++) {
            ibin++;
            bin  = GetBin(binx, biny, binz);
            Double_t y = GetBinContent(bin); 
            if (onlyPositive && y < 0) { 
               Error("ComputeIntegral","Bin content is negative - return a NaN value");
               fIntegral[nbins] = TMath::QuietNaN();
               break;
            }
            fIntegral[ibin] = fIntegral[ibin-1] + y;
         }
      }
   }

   //   - Normalize integral to 1
   if (fIntegral[nbins] == 0 ) {
      Error("ComputeIntegral", "Integral = zero"); return 0;
   }
   for (bin=1;bin<=nbins;bin++)  fIntegral[bin] /= fIntegral[nbins];
   fIntegral[nbins+1] = fEntries;
   return fIntegral[nbins];
}

//______________________________________________________________________________
Double_t *TH1::GetIntegral()
{
   //  Return a pointer to the array of bins integral.
   //  if the pointer fIntegral is null, TH1::ComputeIntegral is called
   // The array dimension is the number of bins in the histograms
   // including underflow and overflow (fNCells)
   // the last value integral[fNCells] is set to the number of entries of
   // the histogram

   if (!fIntegral) ComputeIntegral();
   return fIntegral;
}

//______________________________________________________________________________
TH1 *TH1::GetCumulative(Bool_t forward, const char* suffix) const
{
   //  Return a pointer to an histogram containing the cumulative The
   //  cumulative can be computed both in the forward (default) or backward
   //  direction; the name of the new histogram is constructed from
   //  the name of this histogram with the suffix suffix appended.
   //
   // The cumulative distribution is formed by filling each bin of the
   // resulting histogram with the sum of that bin and all previous
   // (forward == kTRUE) or following (forward = kFALSE) bins.
   //
   // note: while cumulative distributions make sense in one dimension, you
   // may not be getting what you expect in more than 1D because the concept
   // of a cumulative distribution is much trickier to define; make sure you
   // understand the order of summation before you use this method with
   // histograms of dimension >= 2.

   const Int_t nbinsx = GetNbinsX();
   const Int_t nbinsy = GetNbinsY();
   const Int_t nbinsz = GetNbinsZ();
   TH1* hintegrated = (TH1*) Clone(fName + suffix);
   hintegrated->Reset();
   if (forward) { // Forward computation
      Double_t sum = 0.;
      for (Int_t binz = 1; binz <= nbinsz; ++binz) {
	 for (Int_t biny = 1; biny <= nbinsy; ++biny) {
	    for (Int_t binx = 1; binx <= nbinsx; ++binx) {
	       const Int_t bin = hintegrated->GetBin(binx, biny, binz);
	       sum += GetBinContent(bin);
	       hintegrated->SetBinContent(bin, sum);
	    }
	 }
      }
   } else { // Backward computation
      Double_t sum = 0.;
      for (Int_t binz = nbinsz; binz >= 1; --binz) {
	 for (Int_t biny = nbinsy; biny >= 1; --biny) {
	    for (Int_t binx = nbinsx; binx >= 1; --binx) {
	       const Int_t bin = hintegrated->GetBin(binx, biny, binz);
	       sum += GetBinContent(bin);
	       hintegrated->SetBinContent(bin, sum);
	    }
	 }
      }
   }
   return hintegrated;
}

//______________________________________________________________________________
void TH1::Copy(TObject &obj) const
{
   //   -*-*-*-*-*Copy this histogram structure to newth1*-*-*-*-*-*-*-*-*-*-*-*
   //             =======================================
   //
   // Note that this function does not copy the list of associated functions.
   // Use TObject::Clone to make a full copy of an histogram.

   if (((TH1&)obj).fDirectory) {
      // We are likely to change the hash value of this object
      // with TNamed::Copy, to keep things correct, we need to
      // clean up its existing entries.
      ((TH1&)obj).fDirectory->Remove(&obj);
      ((TH1&)obj).fDirectory = 0;
   }
   TNamed::Copy(obj);
   ((TH1&)obj).fDimension = fDimension;
   ((TH1&)obj).fNormFactor= fNormFactor;
   ((TH1&)obj).fNcells    = fNcells;
   ((TH1&)obj).fBarOffset = fBarOffset;
   ((TH1&)obj).fBarWidth  = fBarWidth;
   ((TH1&)obj).fOption    = fOption;
   ((TH1&)obj).fBufferSize= fBufferSize;
   // copy the Buffer
   // delete first a previously existing buffer
   if (((TH1&)obj).fBuffer != 0)  {
      delete []  ((TH1&)obj).fBuffer;
      ((TH1&)obj).fBuffer = 0;
   }
   if (fBuffer) {
      Double_t *buf = new Double_t[fBufferSize];
      for (Int_t i=0;i<fBufferSize;i++) buf[i] = fBuffer[i];
      // obj.fBuffer has been deleted before
      ((TH1&)obj).fBuffer    = buf;
   }


   TArray* a = dynamic_cast<TArray*>(&obj);
   if (a) a->Set(fNcells);
   Int_t canRebin = ((TH1&)obj).TestBit(kCanRebin);
   ((TH1&)obj).ResetBit(kCanRebin);  //we want to avoid the call to LabelsInflate
   // we need to set fBuffer to zero to avoid calling BufferEmpty in GetBinContent
   Double_t * buffer = 0;
   if (fBuffer) {
      buffer = fBuffer;
      ((TH1*)this)->fBuffer = 0;
   }
   for (Int_t i=0;i<fNcells;i++) ((TH1&)obj).SetBinContent(i,this->GetBinContent(i));
   // restore rebin bit and buffer pointer
   if (canRebin) ((TH1&)obj).SetBit(kCanRebin);
   if (buffer) ((TH1*)this)->fBuffer  = buffer;
   ((TH1&)obj).fEntries   = fEntries;

   // which will call BufferEmpty(0) and set fBuffer[0] to a  Maybe one should call
   // assignment operator on the TArrayD

   ((TH1&)obj).fTsumw     = fTsumw;
   ((TH1&)obj).fTsumw2    = fTsumw2;
   ((TH1&)obj).fTsumwx    = fTsumwx;
   ((TH1&)obj).fTsumwx2   = fTsumwx2;
   ((TH1&)obj).fMaximum   = fMaximum;
   ((TH1&)obj).fMinimum   = fMinimum;

   TAttLine::Copy(((TH1&)obj));
   TAttFill::Copy(((TH1&)obj));
   TAttMarker::Copy(((TH1&)obj));
   fXaxis.Copy(((TH1&)obj).fXaxis);
   fYaxis.Copy(((TH1&)obj).fYaxis);
   fZaxis.Copy(((TH1&)obj).fZaxis);
   ((TH1&)obj).fXaxis.SetParent(&obj);
   ((TH1&)obj).fYaxis.SetParent(&obj);
   ((TH1&)obj).fZaxis.SetParent(&obj);
   fContour.Copy(((TH1&)obj).fContour);
   fSumw2.Copy(((TH1&)obj).fSumw2);
   //   fFunctions->Copy(((TH1&)obj).fFunctions);
   if (fgAddDirectory && gDirectory) {
      gDirectory->Append(&obj);
      ((TH1&)obj).fDirectory = gDirectory;
   }

}

//______________________________________________________________________________
void TH1::DirectoryAutoAdd(TDirectory *dir)
{
   // Perform the automatic addition of the histogram to the given directory
   //
   // Note this function is called in place when the semantic requires
   // this object to be added to a directory (I.e. when being read from
   // a TKey or being Cloned)
   //

   Bool_t addStatus = TH1::AddDirectoryStatus();
   if (addStatus) {
      SetDirectory(dir);
      if (dir) {
         ResetBit(kCanDelete);
      }
   }
}

//______________________________________________________________________________
Int_t TH1::DistancetoPrimitive(Int_t px, Int_t py)
{
   //   -*-*-*-*-*-*-*-*-*Compute distance from point px,py to a line*-*-*-*-*-*
   //                     ===========================================
   //     Compute the closest distance of approach from point px,py to elements
   //     of an histogram.
   //     The distance is computed in pixels units.
   //
   //     Algorithm:
   //     Currently, this simple model computes the distance from the mouse
   //     to the histogram contour only.
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (!fPainter) return 9999;
   return fPainter->DistancetoPrimitive(px,py);
}

//______________________________________________________________________________
Bool_t TH1::Divide(TF1 *f1, Double_t c1)
{
// Performs the operation: this = this/(c1*f1)
// if errors are defined (see TH1::Sumw2), errors are also recalculated.
//
// Only bins inside the function range are recomputed.
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Divide
//
// The function return kFALSE if the divide operation failed

   if (!f1) {
      Error("Add","Attempt to divide by a non-existing function");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();
   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

   SetMinimum();
   SetMaximum();

//    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
//    would resize the axis limits!
   ResetBit(kCanRebin);

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t cu,w;
   Double_t xx[3];
   Double_t *params = 0;
   f1->InitArgs(xx,params);
   for (binz=0;binz<=nbinsz+1;binz++) {
      xx[2] = fZaxis.GetBinCenter(binz);
      for (biny=0;biny<=nbinsy+1;biny++) {
         xx[1] = fYaxis.GetBinCenter(biny);
         for (binx=0;binx<=nbinsx+1;binx++) {
            xx[0] = fXaxis.GetBinCenter(binx);
            if (!f1->IsInside(xx)) continue;
            TF1::RejectPoint(kFALSE);
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            Double_t error1 = GetBinError(bin);
            cu  = c1*f1->EvalPar(xx);
            if (TF1::RejectedPoint()) continue;
            if (cu) w = GetBinContent(bin)/cu;
            else    w = 0;
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               if (cu != 0) fSumw2.fArray[bin] = error1*error1/(cu*cu);
               else         fSumw2.fArray[bin] = 0;
            }
         }
      }
   }
   ResetStats();
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TH1::Divide(const TH1 *h1)
{
//   -*-*-*-*-*-*-*-*-*Divide this histogram by h1*-*-*-*-*-*-*-*-*-*-*-*-*
//                     ===========================
//
//   this = this/h1
//   if errors are defined (see TH1::Sumw2), errors are also recalculated.
//   Note that if h1 has Sumw2 set, Sumw2 is automatically called for this
//   if not already set.
//   The resulting errors are calculated assuming uncorrelated histograms.
//   See the other TH1::Divide that gives the possibility to optionally
//   compute binomial errors.
//
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Scale
//
// The function return kFALSE if the divide operation failed

   if (!h1) {
      Error("Divide","Attempt to divide by a non-existing histogram");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();


   try {
      CheckConsistency(this,h1);
   } catch(DifferentNumberOfBins&) {
      Error("Divide","Attempt to divide histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Divide","Attempt to divide histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Divide","Attempt to divide histograms with different bin limits");
   } catch(DifferentLabels&) {
      Warning("Divide","Attempt to divide histograms with different labels");
   }


   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

//    Create Sumw2 if h1 has Sumw2 set
   if (fSumw2.fN == 0 && h1->GetSumw2N() != 0) Sumw2();


//    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
//    would resize the axis limits!
   ResetBit(kCanRebin);

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t c0,c1,w;
   for (binz=0;binz<=nbinsz+1;binz++) {
      for (biny=0;biny<=nbinsy+1;biny++) {
         for (binx=0;binx<=nbinsx+1;binx++) {
            bin = GetBin(binx,biny,binz);
            c0  = GetBinContent(bin);
            c1  = h1->GetBinContent(bin);
            if (c1) w = c0/c1;
            else    w = 0;
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               Double_t e0 = GetBinError(bin);
               Double_t e1 = h1->GetBinError(bin);
               Double_t c12= c1*c1;
               if (!c1) { fSumw2.fArray[bin] = 0; continue;}
               fSumw2.fArray[bin] = (e0*e0*c1*c1 + e1*e1*c0*c0)/(c12*c12);
            }
         }
      }
   }
   ResetStats();
   return kTRUE;
}


//______________________________________________________________________________
Bool_t TH1::Divide(const TH1 *h1, const TH1 *h2, Double_t c1, Double_t c2, Option_t *option)
{
//   -*-*-*Replace contents of this histogram by the division of h1 by h2*-*-*
//         ==============================================================
//
//   this = c1*h1/(c2*h2)
//
//   if errors are defined (see TH1::Sumw2), errors are also recalculated
//   Note that if h1 or h2 have Sumw2 set, Sumw2 is automatically called for this
//   if not already set.
//   The resulting errors are calculated assuming uncorrelated histograms.
//   However, if option ="B" is specified, Binomial errors are computed.
//   In this case c1 and c2 do not make real sense and they are ignored.
//
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Divide
//
//  Please note also that in the binomial case errors are calculated using standard
//  binomial statistics, which means when b1 = b2, the error is zero.
//  If you prefer to have efficiency errors not going to zero when the efficiency is 1, you must
//  use the function TGraphAsymmErrors::BayesDivide, which will return an asymmetric and non-zero lower
//  error for the case b1=b2.
//
// The function return kFALSE if the divide operation failed


   TString opt = option;
   opt.ToLower();
   Bool_t binomial = kFALSE;
   if (opt.Contains("b")) binomial = kTRUE;
   if (!h1 || !h2) {
      Error("Divide","Attempt to divide by a non-existing histogram");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();

   try {
      CheckConsistency(h1,h2);
      CheckConsistency(this,h1);
   } catch(DifferentNumberOfBins&) {
      Error("Divide","Attempt to divide histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Divide","Attempt to divide histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Divide","Attempt to divide histograms with different bin limits");
   }  catch(DifferentLabels&) {
      Warning("Divide","Attempt to divide histograms with different labels");
   }


   if (!c2) {
      Error("Divide","Coefficient of dividing histogram cannot be zero");
      return kFALSE;
   }

   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

//    Create Sumw2 if h1 or h2 have Sumw2 set
   if (fSumw2.fN == 0 && (h1->GetSumw2N() != 0 || h2->GetSumw2N() != 0)) Sumw2();

   SetMinimum();
   SetMaximum();

//    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
//    would resize the axis limits!
   ResetBit(kCanRebin);

//   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t b1,b2,w,d1,d2;
   d1 = c1*c1;
   d2 = c2*c2;
   for (binz=0;binz<=nbinsz+1;binz++) {
      for (biny=0;biny<=nbinsy+1;biny++) {
         for (binx=0;binx<=nbinsx+1;binx++) {
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            b1  = h1->GetBinContent(bin);
            b2  = h2->GetBinContent(bin);
            if (b2) w = c1*b1/(c2*b2);
            else    w = 0;
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               Double_t e1 = h1->GetBinError(bin);
               Double_t e2 = h2->GetBinError(bin);
               Double_t b22= b2*b2*d2;
               if (!b2) { fSumw2.fArray[bin] = 0; continue;}
               if (binomial) {
                  if (b1 != b2) {
                     // in the case of binomial statistics c1 and c2 must be 1 otherwise it does not make sense
                     w = b1/b2;    // c1 and c2 are ignored
                     //fSumw2.fArray[bin] = TMath::Abs(w*(1-w)/(c2*b2));//this is the formula in Hbook/Hoper1
                     //fSumw2.fArray[bin] = TMath::Abs(w*(1-w)/b2);     // old formula from G. Flucke
                     // formula which works also for weighted histogram (see http://root.cern.ch/phpBB2/viewtopic.php?t=3753 )
                     fSumw2.fArray[bin] = TMath::Abs( ( (1.-2.*w)*e1*e1 + w*w*e2*e2 )/(b2*b2) );
                  } else {
                     //in case b1=b2 error is zero
                     //use  TGraphAsymmErrors::BayesDivide for getting the asymmetric error not equal to zero
                     fSumw2.fArray[bin] = 0;
                  }
               } else {
                  fSumw2.fArray[bin] = d1*d2*(e1*e1*b2*b2 + e2*e2*b1*b1)/(b22*b22);
               }
            }
         }
      }
   }
   ResetStats();
   if (binomial)
      // in case of binomial division use denominator for number of entries
      SetEntries ( h2->GetEntries() );

   return kTRUE;
}

//______________________________________________________________________________
void TH1::Draw(Option_t *option)
{
   // Draw this histogram with options.
   //
   // Histograms are drawn via the THistPainter class. Each histogram has
   // a pointer to its own painter (to be usable in a multithreaded program).
   // The same histogram can be drawn with different options in different pads.
   // When an histogram drawn in a pad is deleted, the histogram is
   // automatically removed from the pad or pads where it was drawn.
   // If an histogram is drawn in a pad, then filled again, the new status
   // of the histogram will be automatically shown in the pad next time
   // the pad is updated. One does not need to redraw the histogram.
   // To draw the current version of an histogram in a pad, one can use
   //      h->DrawCopy();
   // This makes a clone of the histogram. Once the clone is drawn, the original
   // histogram may be modified or deleted without affecting the aspect of the
   // clone.
   // By default, TH1::Draw clears the current pad.
   //
   // One can use TH1::SetMaximum and TH1::SetMinimum to force a particular
   // value for the maximum or the minimum scale on the plot.
   //
   // TH1::UseCurrentStyle can be used to change all histogram graphics
   // attributes to correspond to the current selected style.
   // This function must be called for each histogram.
   // In case one reads and draws many histograms from a file, one can force
   // the histograms to inherit automatically the current graphics style
   // by calling before gROOT->ForceStyle();
   //
   // See the THistPainter class for a description of all the drawing options.

   TString opt1 = option; opt1.ToLower();
   TString opt2 = option;
   Int_t index  = opt1.Index("same");

   // Check if the string "same" is part of a TCutg name.
   if (index>=0) {
      Int_t indb = opt1.Index("[");
      if (indb>=0) {
         Int_t indk = opt1.Index("]");
         if (index>indb && index<indk) index = -1;
      }
   }

   // If there is no pad or an empty pad the the "same" is ignored.
   if (gPad) {
      if (!gPad->IsEditable()) gROOT->MakeDefCanvas();
      if (index>=0) {
         if (gPad->GetX1() == 0   && gPad->GetX2() == 1 &&
             gPad->GetY1() == 0   && gPad->GetY2() == 1 &&
             gPad->GetListOfPrimitives()->GetSize()==0) opt2.Remove(index,4);
      } else {
         //the following statement is necessary in case one attempts to draw
         //a temporary histogram already in the current pad
         if (TestBit(kCanDelete)) gPad->GetListOfPrimitives()->Remove(this);
         gPad->Clear();
      }
   } else {
      if (index>=0) opt2.Remove(index,4);
   }

   AppendPad(opt2.Data());
}

//______________________________________________________________________________
TH1 *TH1::DrawCopy(Option_t *) const
{
//   -*-*-*-*-*Copy this histogram and Draw in the current pad*-*-*-*-*-*-*-*
//             ===============================================
//
//     Once the histogram is drawn into the pad, any further modification
//     using graphics input will be made on the copy of the histogram,
//     and not to the original object.
//
//     See Draw for the list of options
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   AbstractMethod("DrawCopy");
   return 0;
}

//______________________________________________________________________________
TH1 *TH1::DrawNormalized(Option_t *option, Double_t norm) const
{
//  Draw a normalized copy of this histogram.
//
//  A clone of this histogram is normalized to norm and drawn with option.
//  A pointer to the normalized histogram is returned.
//  The contents of the histogram copy are scaled such that the new
//  sum of weights (excluding under and overflow) is equal to norm.
//  Note that the returned normalized histogram is not added to the list
//  of histograms in the current directory in memory.
//  It is the user's responsability to delete this histogram.
//  The kCanDelete bit is set for the returned object. If a pad containing
//  this copy is cleared, the histogram will be automatically deleted
//
//     See Draw for the list of options
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   Double_t sum = GetSumOfWeights();
   if (sum == 0) {
      Error("DrawNormalized","Sum of weights is null. Cannot normalize histogram: %s",GetName());
      return 0;
   }
   Bool_t addStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TH1 *h = (TH1*)Clone();
   h->SetBit(kCanDelete);
   // in case of drawing with error options - scale correctly the error
   TString opt(option); opt.ToUpper(); 
   if (fSumw2.fN == 0) { 
      h->Sumw2();
      // do not use in this case the "Error option " for drawing which is enabled by default since the normalized histogram has now errors
      if (opt.IsNull() || opt == "SAME") opt += "HIST";
   }
   h->Scale(norm/sum);
   if (TMath::Abs(fMaximum+1111) > 1e-3) h->SetMaximum(fMaximum*norm/sum);
   if (TMath::Abs(fMinimum+1111) > 1e-3) h->SetMinimum(fMinimum*norm/sum);
   h->Draw(opt);
   TH1::AddDirectory(addStatus);
   return h;
}

//______________________________________________________________________________
void TH1::DrawPanel()
{
//   -*-*-*-*-*Display a panel with all histogram drawing options*-*-*-*-*-*
//             ==================================================
//
//      See class TDrawPanelHist for example

   if (!fPainter) {Draw(); if (gPad) gPad->Update();}
   if (fPainter) fPainter->DrawPanel();
}

//______________________________________________________________________________
void TH1::Eval(TF1 *f1, Option_t *option)
{
//   -*-*-*Evaluate function f1 at the center of bins of this histogram-*-*-*-*
//         ============================================================
//
//     If option "R" is specified, the function is evaluated only
//     for the bins included in the function range.
//     If option "A" is specified, the value of the function is added to the
//     existing bin contents
//     If option "S" is specified, the value of the function is used to
//     generate a value, distributed according to the Poisson
//     distribution, with f1 as the mean.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Double_t x[3];
   Int_t range,stat,add,bin,binx,biny,binz,nbinsx, nbinsy, nbinsz;
   if (!f1) return;
   Double_t fu;
   Double_t e=0;
   TString opt = option;
   opt.ToLower();
   if (opt.Contains("a")) add   = 1;
   else                   add   = 0;
   if (opt.Contains("s")) stat  = 1;
   else                   stat  = 0;
   if (opt.Contains("r")) range = 1;
   else                   range = 0;

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   nbinsx  = fXaxis.GetNbins();
   nbinsy  = fYaxis.GetNbins();
   nbinsz  = fZaxis.GetNbins();
   if (!add) Reset();

   for (binz=1;binz<=nbinsz;binz++) {
      x[2]  = fZaxis.GetBinCenter(binz);
      for (biny=1;biny<=nbinsy;biny++) {
         x[1]  = fYaxis.GetBinCenter(biny);
         for (binx=1;binx<=nbinsx;binx++) {
            bin = GetBin(binx,biny,binz);
            x[0]  = fXaxis.GetBinCenter(binx);
            if (range && !f1->IsInside(x)) continue;
            fu = f1->Eval(x[0],x[1],x[2]);
            if (stat) fu = gRandom->PoissonD(fu);
            if (fSumw2.fN) e = fSumw2.fArray[bin];
            AddBinContent(bin,fu);
            if (fSumw2.fN) fSumw2.fArray[bin] = e+ TMath::Abs(fu);
         }
      }
   }
}

//______________________________________________________________________________
void TH1::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//   -*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//                     =========================================
//     This member function is called when a histogram is clicked with the locator
//
//     If Left button clicked on the bin top value, then the content of this bin
//     is modified according to the new position of the mouse when it is released.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (fPainter) fPainter->ExecuteEvent(event, px, py);
}

//______________________________________________________________________________
TH1* TH1::FFT(TH1* h_output, Option_t *option)
{
// This function allows to do discrete Fourier transforms of TH1 and TH2.
// Available transform types and flags are described below.
//
// To extract more information about the transform, use the function
//  TVirtualFFT::GetCurrentTransform() to get a pointer to the current
//  transform object.
//
// Parameters:
//  1st - histogram for the output. If a null pointer is passed, a new histogram is created
//  and returned, otherwise, the provided histogram is used and should be big enough
//
//  Options: option parameters consists of 3 parts:
//    - option on what to return
//   "RE" - returns a histogram of the real part of the output
//   "IM" - returns a histogram of the imaginary part of the output
//   "MAG"- returns a histogram of the magnitude of the output
//   "PH" - returns a histogram of the phase of the output
//
//    - option of transform type
//   "R2C"  - real to complex transforms - default
//   "R2HC" - real to halfcomplex (special format of storing output data,
//          results the same as for R2C)
//   "DHT" - discrete Hartley transform
//         real to real transforms (sine and cosine):
//   "R2R_0", "R2R_1", "R2R_2", "R2R_3" - discrete cosine transforms of types I-IV
//   "R2R_4", "R2R_5", "R2R_6", "R2R_7" - discrete sine transforms of types I-IV
//    To specify the type of each dimension of a 2-dimensional real to real
//    transform, use options of form "R2R_XX", for example, "R2R_02" for a transform,
//    which is of type "R2R_0" in 1st dimension and  "R2R_2" in the 2nd.
//
//    - option of transform flag
//    "ES" (from "estimate") - no time in preparing the transform, but probably sub-optimal
//       performance
//    "M" (from "measure")   - some time spend in finding the optimal way to do the transform
//    "P" (from "patient")   - more time spend in finding the optimal way to do the transform
//    "EX" (from "exhaustive") - the most optimal way is found
//     This option should be chosen depending on how many transforms of the same size and
//     type are going to be done. Planning is only done once, for the first transform of this
//     size and type. Default is "ES".
//   Examples of valid options: "Mag R2C M" "Re R2R_11" "Im R2C ES" "PH R2HC EX"


   Int_t ndim[3];
   ndim[0] = this->GetNbinsX();
   ndim[1] = this->GetNbinsY();
   ndim[2] = this->GetNbinsZ();

   TVirtualFFT *fft;
   TString opt = option;
   opt.ToUpper();
   if (!opt.Contains("2R")){
      if (!opt.Contains("2C") && !opt.Contains("2HC") && !opt.Contains("DHT")) {
         //no type specified, "R2C" by default
         opt.Append("R2C");
      }
      fft = TVirtualFFT::FFT(this->GetDimension(), ndim, opt.Data());
   }
   else {
      //find the kind of transform
      Int_t ind = opt.Index("R2R", 3);
      Int_t *kind = new Int_t[2];
      char t;
      t = opt[ind+4];
      kind[0] = atoi(&t);
      if (h_output->GetDimension()>1) {
         t = opt[ind+5];
         kind[1] = atoi(&t);
      }
      fft = TVirtualFFT::SineCosine(this->GetDimension(), ndim, kind, option);
      delete [] kind;
   }

   if (!fft) return 0;
   Int_t in=0;
   for (Int_t binx = 1; binx<=ndim[0]; binx++) {
      for (Int_t biny=1; biny<=ndim[1]; biny++) {
         for (Int_t binz=1; binz<=ndim[2]; binz++) {
            fft->SetPoint(in, this->GetBinContent(binx, biny, binz));
            in++;
         }
      }
   }
   fft->Transform();
   h_output = TransformHisto(fft, h_output, option);
   return h_output;
}

//______________________________________________________________________________
Int_t TH1::Fill(Double_t x)
{
//   -*-*-*-*-*-*-*-*Increment bin with abscissa X by 1*-*-*-*-*-*-*-*-*-*-*
//                   ==================================
//
//    if x is less than the low-edge of the first bin, the Underflow bin is incremented
//    if x is greater than the upper edge of last bin, the Overflow bin is incremented
//
//    If the storage of the sum of squares of weights has been triggered,
//    via the function Sumw2, then the sum of the squares of weights is incremented
//    by 1 in the bin corresponding to x.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (fBuffer)  return BufferFill(x,1);

   Int_t bin;
   fEntries++;
   bin =fXaxis.FindBin(x);
   if (bin <0) return -1;
   AddBinContent(bin);
   if (fSumw2.fN) ++fSumw2.fArray[bin];
   if (bin == 0 || bin > fXaxis.GetNbins()) {
      if (!fgStatOverflows) return -1;
   }
   ++fTsumw;
   ++fTsumw2;
   fTsumwx  += x;
   fTsumwx2 += x*x;
   return bin;
}

//______________________________________________________________________________
Int_t TH1::Fill(Double_t x, Double_t w)
{
//   -*-*-*-*-*-*Increment bin with abscissa X with a weight w*-*-*-*-*-*-*-*
//               =============================================
//
//    if x is less than the low-edge of the first bin, the Underflow bin is incremented
//    if x is greater than the upper edge of last bin, the Overflow bin is incremented
//
//    If the storage of the sum of squares of weights has been triggered,
//    via the function Sumw2, then the sum of the squares of weights is incremented
//    by w^2 in the bin corresponding to x.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   
   if (fBuffer) return BufferFill(x,w);

   Int_t bin;
   fEntries++;
   bin =fXaxis.FindBin(x);
   if (bin <0) return -1;
   AddBinContent(bin, w);
   if (fSumw2.fN) fSumw2.fArray[bin] += w*w;
   if (bin == 0 || bin > fXaxis.GetNbins()) {
      if (!fgStatOverflows) return -1;
   }
   //Double_t z= (w > 0 ? w : -w);
   Double_t z= w;
   fTsumw   += z;
   fTsumw2  += z*z;
   fTsumwx  += z*x;
   fTsumwx2 += z*x*x;
   return bin;
}

//______________________________________________________________________________
Int_t TH1::Fill(const char *namex, Double_t w)
{
// Increment bin with namex with a weight w
//
// if x is less than the low-edge of the first bin, the Underflow bin is incremented
// if x is greater than the upper edge of last bin, the Overflow bin is incremented
//
// If the storage of the sum of squares of weights has been triggered,
// via the function Sumw2, then the sum of the squares of weights is incremented
// by w^2 in the bin corresponding to x.
//

   Int_t bin;
   fEntries++;
   bin =fXaxis.FindBin(namex);
   if (bin <0) return -1;
   AddBinContent(bin, w);
   if (fSumw2.fN) fSumw2.fArray[bin] += w*w;
   if (bin == 0 || bin > fXaxis.GetNbins()) return -1;
   //Double_t z= (w > 0 ? w : -w);
   Double_t z= w;
   fTsumw   += z;
   fTsumw2  += z*z;
   // this make sense if the histogram is not expanding (kCanRebin is not set)
   if (!TestBit(TH1::kCanRebin)) {
      Double_t x = fXaxis.GetBinCenter(bin);
      fTsumwx  += z*x;
      fTsumwx2 += z*x*x;
   }
   return bin;
}

//______________________________________________________________________________
void TH1::FillN(Int_t ntimes, const Double_t *x, const Double_t *w, Int_t stride)
{
   // Fill this histogram with an array x and weights w.
   //
   //    ntimes:  number of entries in arrays x and w (array size must be ntimes*stride)
   //    x:       array of values to be histogrammed
   //    w:       array of weighs
   //    stride:  step size through arrays x and w
   //
   //    If the weight is not equal to 1, the storage of the sum of squares of
   //    weights is automatically triggered and the sum of the squares of weights is incremented
   //    by w^2 in the bin corresponding to x.
   //    if w is NULL each entry is assumed a weight=1

   
   //If a buffer is activated, fill buffer
   if (fBuffer) {
      ntimes *= stride;
      Int_t i = 0;
      for (i=0;i<ntimes;i+=stride) {
         if (!fBuffer) break;   // buffer can be deleted in BufferFill when is empty
         if (w) BufferFill(x[i],w[i]);
         else BufferFill(x[i], 1.);
      }
      // fill the remaining entries if the buffer has been deleted 
      if (i < ntimes && fBuffer==0) 
         DoFillN((ntimes-i)/stride,&x[i],&w[i],stride);
      return;
   }
   // call internal method 
   DoFillN(ntimes, x, w, stride);
}

//______________________________________________________________________________
void TH1::DoFillN(Int_t ntimes, const Double_t *x, const Double_t *w, Int_t stride)
{
   // internal method to fill histogram content from a vector
   // called directly by TH1::BufferEmpty

   Int_t bin,i;
   
   fEntries += ntimes;
   Double_t ww = 1;
   Int_t nbins   = fXaxis.GetNbins();
   ntimes *= stride;
   for (i=0;i<ntimes;i+=stride) {
      bin =fXaxis.FindBin(x[i]);
      if (bin <0) continue;
      if (w) ww = w[i];
      AddBinContent(bin, ww);
      if (fSumw2.fN) fSumw2.fArray[bin] += ww*ww;
      if (bin == 0 || bin > nbins) {
         if (!fgStatOverflows) continue;
      }
      //Double_t z= (ww > 0 ? ww : -ww);
      Double_t z= ww;
      fTsumw   += z;
      fTsumw2  += z*z;
      fTsumwx  += z*x[i];
      fTsumwx2 += z*x[i]*x[i];
   }
}

//______________________________________________________________________________
void TH1::FillRandom(const char *fname, Int_t ntimes)
{
//   -*-*-*-*-*Fill histogram following distribution in function fname*-*-*-*
//             =======================================================
//
//      The distribution contained in the function fname (TF1) is integrated
//      over the channel contents for the bin range of this histogram.
//      It is normalized to 1.
//      Getting one random number implies:
//        - Generating a random number between 0 and 1 (say r1)
//        - Look in which bin in the normalized integral r1 corresponds to
//        - Fill histogram channel
//      ntimes random numbers are generated
//
//     One can also call TF1::GetRandom to get a random variate from a function.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*

   Int_t bin, binx, ibin, loop;
   Double_t r1, x;
//   - Search for fname in the list of ROOT defined functions
   TF1 *f1 = (TF1*)gROOT->GetFunction(fname);
   if (!f1) { Error("FillRandom", "Unknown function: %s",fname); return; }

//   - Allocate temporary space to store the integral and compute integral
   Int_t first  = fXaxis.GetFirst();
   Int_t last   = fXaxis.GetLast();
   Int_t nbinsx = last-first+1;

   Double_t *integral = new Double_t[nbinsx+1];
   integral[0] = 0;
   for (binx=1;binx<=nbinsx;binx++) {
      Double_t fint = f1->Integral(fXaxis.GetBinLowEdge(binx+first-1),fXaxis.GetBinUpEdge(binx+first-1));
      integral[binx] = integral[binx-1] + fint;
   }

//   - Normalize integral to 1
   if (integral[nbinsx] == 0 ) {
      delete [] integral;
      Error("FillRandom", "Integral = zero"); return;
   }
   for (bin=1;bin<=nbinsx;bin++)  integral[bin] /= integral[nbinsx];

//   --------------Start main loop ntimes
   for (loop=0;loop<ntimes;loop++) {
      r1 = gRandom->Rndm(loop);
      ibin = TMath::BinarySearch(nbinsx,&integral[0],r1);
      //binx = 1 + ibin;
      //x    = fXaxis.GetBinCenter(binx); //this is not OK when SetBuffer is used
      x    = fXaxis.GetBinLowEdge(ibin+first)
             +fXaxis.GetBinWidth(ibin+first)*(r1-integral[ibin])/(integral[ibin+1] - integral[ibin]);
      Fill(x, 1.);
   }
   delete [] integral;
}

//______________________________________________________________________________
void TH1::FillRandom(TH1 *h, Int_t ntimes)
{
//   -*-*-*-*-*Fill histogram following distribution in histogram h*-*-*-*
//             ====================================================
//
//      The distribution contained in the histogram h (TH1) is integrated
//      over the channel contents for the bin range of this histogram.
//      It is normalized to 1.
//      Getting one random number implies:
//        - Generating a random number between 0 and 1 (say r1)
//        - Look in which bin in the normalized integral r1 corresponds to
//        - Fill histogram channel
//      ntimes random numbers are generated
//
//    SPECIAL CASE when the target histogram has the same binning as the source.
//   in this case we simply use a poisson distribution where
//   the mean value per bin = bincontent/integral.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*

   if (!h) { Error("FillRandom", "Null histogram"); return; }
   if (fDimension != h->GetDimension()) {
      Error("FillRandom", "Histograms with different dimensions"); return;
   }

   //in case the target histogram has the same binning and ntimes much greater
   //than the number of bins we can use a fast method
   Int_t first  = fXaxis.GetFirst();
   Int_t last   = fXaxis.GetLast();
   Int_t nbins = last-first+1;
   if (ntimes > 10*nbins) { 
      try {
         CheckConsistency(this,h);
         Double_t sumw = h->Integral(first,last);
         if (sumw == 0) return;
         Double_t sumgen = 0;
         for (Int_t bin=first;bin<=last;bin++) {
            Double_t mean = h->GetBinContent(bin)*ntimes/sumw;
            Double_t cont = (Double_t)gRandom->Poisson(mean);
            sumgen += cont;
            AddBinContent(bin,cont);
            if (fSumw2.fN) fSumw2.fArray[bin] += cont;
         }
         
         // fix for the fluctations in the total number n
         // since we use Poisson instead of multinomial
         // add a correction to have ntimes as generated entries
         Int_t i;
         if (sumgen < ntimes) {
            // add missing entries
            for (i = Int_t(sumgen+0.5); i < ntimes; ++i)
            {
               Double_t x = h->GetRandom();
               Fill(x);
            }
         }
         else if (sumgen > ntimes) {
            // remove extra entries
            i =  Int_t(sumgen+0.5);
            while( i > ntimes) {
               Double_t x = h->GetRandom();
               Int_t ibin = fXaxis.FindBin(x);
               Double_t y = GetBinContent(ibin);
               // skip in case bin is empty
               if (y > 0) {
                  SetBinContent(ibin, y-1.);
                  i--;
               }
            }
         }

         ResetStats();
         return;
      }
      catch(std::exception&) {}  // do nothing 
   }
   // case of different axis and not too large ntimes
   
   if (h->ComputeIntegral() ==0) return;
   Int_t loop;
   Double_t x;
   for (loop=0;loop<ntimes;loop++) {
      x = h->GetRandom();
      Fill(x);
   }
}


//______________________________________________________________________________
Int_t TH1::FindBin(Double_t x, Double_t y, Double_t z)
{
//   Return Global bin number corresponding to x,y,z
//   ===============================================
//
//      2-D and 3-D histograms are represented with a one dimensional
//      structure. This function tries to rebin the axis if the given point
//      belongs to an under-/overflow bin.
//      This has the advantage that all existing functions, such as
//        GetBinContent, GetBinError, GetBinFunction work for all dimensions.
//     See also TH1::GetBin, TAxis::FindBin and TAxis::FindFixBin
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (GetDimension() < 2) {
      return fXaxis.FindBin(x);
   }
   if (GetDimension() < 3) {
      Int_t nx   = fXaxis.GetNbins()+2;
      Int_t binx = fXaxis.FindBin(x);
      Int_t biny = fYaxis.FindBin(y);
      return  binx + nx*biny;
   }
   if (GetDimension() < 4) {
      Int_t nx   = fXaxis.GetNbins()+2;
      Int_t ny   = fYaxis.GetNbins()+2;
      Int_t binx = fXaxis.FindBin(x);
      Int_t biny = fYaxis.FindBin(y);
      Int_t binz = fZaxis.FindBin(z);
      return  binx + nx*(biny +ny*binz);
   }
   return -1;
}

//______________________________________________________________________________
Int_t TH1::FindFixBin(Double_t x, Double_t y, Double_t z) const
{
//   Return Global bin number corresponding to x,y,z
//   ===============================================
//
//      2-D and 3-D histograms are represented with a one dimensional
//      structure. This function DOES not try to rebin the axis if the given
//      point belongs to an under-/overflow bin.
//      This has the advantage that all existing functions, such as
//        GetBinContent, GetBinError, GetBinFunction work for all dimensions.
//     See also TH1::GetBin, TAxis::FindBin and TAxis::FindFixBin
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (GetDimension() < 2) {
      return fXaxis.FindFixBin(x);
   }
   if (GetDimension() < 3) {
      Int_t nx   = fXaxis.GetNbins()+2;
      Int_t binx = fXaxis.FindFixBin(x);
      Int_t biny = fYaxis.FindFixBin(y);
      return  binx + nx*biny;
   }
   if (GetDimension() < 4) {
      Int_t nx   = fXaxis.GetNbins()+2;
      Int_t ny   = fYaxis.GetNbins()+2;
      Int_t binx = fXaxis.FindFixBin(x);
      Int_t biny = fYaxis.FindFixBin(y);
      Int_t binz = fZaxis.FindFixBin(z);
      return  binx + nx*(biny +ny*binz);
   }
   return -1;
}

//______________________________________________________________________________
Int_t TH1::FindFirstBinAbove(Double_t threshold, Int_t axis) const
{
   //find first bin with content > threshold for axis (1=x, 2=y, 3=z)
   //if no bins with content > threshold is found the function returns -1.

   if (axis != 1) {
      Warning("FindFirstBinAbove","Invalid axis number : %d, axis x assumed\n",axis);
      axis = 1;
   }
   Int_t nbins = fXaxis.GetNbins();
   for (Int_t bin=1;bin<=nbins;bin++) {
      if (GetBinContent(bin) > threshold) return bin;
   }
   return -1;
}


//______________________________________________________________________________
Int_t TH1::FindLastBinAbove(Double_t threshold, Int_t axis) const
{
   //find last bin with content > threshold for axis (1=x, 2=y, 3=z)
   //if no bins with content > threshold is found the function returns -1.

   if (axis != 1) {
      Warning("FindLastBinAbove","Invalid axis number : %d, axis x assumed\n",axis);
      axis = 1;
   }
   Int_t nbins = fXaxis.GetNbins();
   for (Int_t bin=nbins;bin>=1;bin--) {
      if (GetBinContent(bin) > threshold) return bin;
   }
   return -1;
}

//______________________________________________________________________________
TObject *TH1::FindObject(const char *name) const
{
// search object named name in the list of functions

   if (fFunctions) return fFunctions->FindObject(name);
   return 0;
}

//______________________________________________________________________________
TObject *TH1::FindObject(const TObject *obj) const
{
// search object obj in the list of functions

   if (fFunctions) return fFunctions->FindObject(obj);
   return 0;
}

//______________________________________________________________________________
TFitResultPtr TH1::Fit(const char *fname ,Option_t *option ,Option_t *goption, Double_t xxmin, Double_t xxmax)
{
//                     Fit histogram with function fname
//                     =================================
//      fname is the name of an already predefined function created by TF1 or TF2
//      Predefined functions such as gaus, expo and poln are automatically
//      created by ROOT.
//      fname can also be a formula, accepted by the linear fitter (linear parts divided
//      by "++" sign), for example "x++sin(x)" for fitting "[0]*x+[1]*sin(x)"
//
//  This function finds a pointer to the TF1 object with name fname
//  and calls TH1::Fit(TF1 *f1,...)

   char *linear;
   linear= (char*)strstr(fname, "++");
   TF1 *f1=0;
   TF2 *f2=0;
   TF3 *f3=0;
   Int_t ndim=GetDimension();
   if (linear){
      if (ndim<2){
         f1=new TF1(fname, fname, xxmin, xxmax);
         return Fit(f1,option,goption,xxmin,xxmax);
      }
      else if (ndim<3){
         f2=new TF2(fname, fname);
         return Fit(f2,option,goption,xxmin,xxmax);
      }
      else{
         f3=new TF3(fname, fname);
         return Fit(f3,option,goption,xxmin,xxmax);
      }
   }

   else{
      f1 = (TF1*)gROOT->GetFunction(fname);
      if (!f1) { Printf("Unknown function: %s",fname); return -1; }
      return Fit(f1,option,goption,xxmin,xxmax);
   }
}

//______________________________________________________________________________
TFitResultPtr TH1::Fit(TF1 *f1 ,Option_t *option ,Option_t *goption, Double_t xxmin, Double_t xxmax)
{
//                     Fit histogram with function f1
//                     ==============================
//
//      Fit this histogram with function f1.
//
//      The list of fit options is given in parameter option.
//         option = "W"  Set all weights to 1 for non empty bins; ignore error bars
//                = "WW" Set all weights to 1 including empty bins; ignore error bars
//                = "I"  Use integral of function in bin, normalized by the bin volume,
//                       instead of value at bin center
//                = "L"  Use Loglikelihood method (default is chisquare method)
//                = "WL" Use Loglikelihood method and bin contents are not integer,
//                       i.e. histogram is weighted (must have Sumw2() set)
//                = "U"  Use a User specified fitting algorithm (via SetFCN)
//                = "Q"  Quiet mode (minimum printing)
//                = "V"  Verbose mode (default is between Q and V)
//                = "E"  Perform better Errors estimation using Minos technique
//                = "B"  User defined parameter settings are used for predefined functions
//                       like "gaus", "expo", "poln", "landau".
//                       Use this option when you want to fix one or more parameters for these functions.
//                = "M"  More. Improve fit results.
//                       It uses the IMPROVE command of TMinuit (see TMinuit::mnimpr).
//                       This algorithm attempts to improve the found local minimum by searching for a
//                       better one.
//                = "R"  Use the Range specified in the function range
//                = "N"  Do not store the graphics function, do not draw
//                = "0"  Do not plot the result of the fit. By default the fitted function
//                       is drawn unless the option"N" above is specified.
//                = "+"  Add this new fitted function to the list of fitted functions
//                       (by default, any previous function is deleted)
//                = "C"  In case of linear fitting, don't calculate the chisquare
//                       (saves time)
//                = "F"  If fitting a polN, switch to minuit fitter
//                = "S"  The result of the fit is returned in the TFitResultPtr
//                       (see below Access to the Fit Result)
//
//      When the fit is drawn (by default), the parameter goption may be used
//      to specify a list of graphics options. See TH1::Draw for a complete
//      list of these options.
//
//      In order to use the Range option, one must first create a function
//      with the expression to be fitted. For example, if your histogram
//      has a defined range between -4 and 4 and you want to fit a gaussian
//      only in the interval 1 to 3, you can do:
//           TF1 *f1 = new TF1("f1", "gaus", 1, 3);
//           histo->Fit("f1", "R");
//
//      Setting initial conditions
//      ==========================
//      Parameters must be initialized before invoking the Fit function.
//      The setting of the parameter initial values is automatic for the
//      predefined functions : poln, expo, gaus, landau. One can however disable
//      this automatic computation by specifying the option "B".
//      Note that if a predefined function is defined with an argument,
//      eg, gaus(0), expo(1), you must specify the initial values for
//      the parameters.
//      You can specify boundary limits for some or all parameters via
//           f1->SetParLimits(p_number, parmin, parmax);
//      if parmin>=parmax, the parameter is fixed
//      Note that you are not forced to fix the limits for all parameters.
//      For example, if you fit a function with 6 parameters, you can do:
//        func->SetParameters(0, 3.1, 1.e-6, -8, 0, 100);
//        func->SetParLimits(3, -10, -4);
//        func->FixParameter(4, 0);
//        func->SetParLimits(5, 1, 1);
//      With this setup, parameters 0->2 can vary freely
//      Parameter 3 has boundaries [-10,-4] with initial value -8
//      Parameter 4 is fixed to 0
//      Parameter 5 is fixed to 100.
//      When the lower limit and upper limit are equal, the parameter is fixed.
//      However to fix a parameter to 0, one must call the FixParameter function.
//
//      Note that option "I" gives better results but is slower.
//
//
//      Changing the fitting objective function
//      =============================
//     By default a chi square function is used for fitting. When option "L" (or "LL") is used
//     a Poisson likelihood function (see note below) is used.
//     The functions are defined in the header Fit/Chi2Func.h or Fit/PoissonLikelihoodFCN and they
//     are implemented using the routines FitUtil::EvaluateChi2 or FitUtil::EvaluatePoissonLogL in
//     the file math/mathcore/src/FitUtil.cxx.
//     To specify a User defined fitting function, specify option "U" and
//     call the following functions:
//       TVirtualFitter::Fitter(myhist)->SetFCN(MyFittingFunction)
//     where MyFittingFunction is of type:
//     extern void MyFittingFunction(Int_t &npar, Double_t *gin, Double_t &f, Double_t *u, Int_t flag);
//
//     Likelihood Fits
//     =================
//     When using option "L" a likelihood fit is used instead of the default chi2 square fit.
//     The likelihood is built assuming a Poisson probability density function for each bin.
//     This method can then be used only when the bin content represents counts (i.e. errors are sqrt(N) ).
//     The likelihood method has the advantage of treating correctly the empty bins and use them in the
//     fit procedure.
//     In the chi2 method the empty bins are skipped and not considered in the fit.
//     The likelihood method, although a bit slower, it is the recommended method in case of low
//     bin statistics, where the chi2 method may give incorrect results.
//
//      Fitting a histogram of dimension N with a function of dimension N-1
//      ===================================================================
//     It is possible to fit a TH2 with a TF1 or a TH3 with a TF2.
//     In this case the option "Integral" is not allowed and each cell has
//     equal weight.
//
//     Associated functions
//     ====================
//     One or more object (typically a TF1*) can be added to the list
//     of functions (fFunctions) associated to each histogram.
//     When TH1::Fit is invoked, the fitted function is added to this list.
//     Given an histogram h, one can retrieve an associated function
//     with:  TF1 *myfunc = h->GetFunction("myfunc");
//
//      Access to the fit result
//      ========================
//     The function returns a TFitResultPtr which can hold a  pointer to a TFitResult object.
//     By default the TFitResultPtr contains only the status of the fit which is return by an
//     automatic conversion of the TFitResultPtr to an integer. One can write in this case directly:
//     Int_t fitStatus =  h->Fit(myFunc)
//
//     If the option "S" is instead used, TFitResultPtr contains the TFitResult and behaves as a smart
//     pointer to it. For example one can do:
//     TFitResultPtr r = h->Fit(myFunc,"S");
//     TMatrixDSym cov = r->GetCovarianceMatrix();  //  to access the covariance matrix
//     Double_t chi2   = r->Chi2(); // to retrieve the fit chi2
//     Double_t par0   = r->Parameter(0); // retrieve the value for the parameter 0
//     Double_t err0   = r->ParError(0); // retrieve the error for the parameter 0
//     r->Print("V");     // print full information of fit including covariance matrix
//     r->Write();        // store the result in a file
//
//     The fit parameters, error and chi2 (but not covariance matrix) can be retrieved also
//     from the fitted function.
//     If the histogram is made persistent, the list of
//     associated functions is also persistent. Given a pointer (see above)
//     to an associated function myfunc, one can retrieve the function/fit
//     parameters with calls such as:
//       Double_t chi2 = myfunc->GetChisquare();
//       Double_t par0 = myfunc->GetParameter(0); //value of 1st parameter
//       Double_t err0 = myfunc->GetParError(0);  //error on first parameter
//
//
//     Access to the fit status
//     =====================
//     The status of the fit can be obtained converting the TFitResultPtr to an integer
//     independently if the fit option "S" is used or not:
//     TFitResultPtr r = h->Fit(myFunc,opt);
//     Int_t fitStatus = r;
//
//     The fitStatus is 0 if the fit is OK (i.e no error occurred).
//     The value of the fit status code is negative in case of an error not connected with the
//     minimization procedure, for example  when a wrong function is used.
//     Otherwise the return value is the one returned from the minimization procedure.
//     When TMinuit (default case) or Minuit2 are used as minimizer the status returned is :
//     fitStatus =  migradResult + 10*minosResult + 100*hesseResult + 1000*improveResult.
//     TMinuit will return 0 (for migrad, minos, hesse or improve) in case of success and 4 in
//     case of error (see the documentation of TMinuit::mnexcm). So for example, for an error
//     only in Minos but not in Migrad a fitStatus of 40 will be returned.
//     Minuit2 will return also 0 in case of success and different values in migrad minos or
//     hesse depending on the error. See in this case the documentation of
//     Minuit2Minimizer::Minimize for the migradResult, Minuit2Minimizer::GetMinosError for the
//     minosResult and Minuit2Minimizer::Hesse for the hesseResult.
//     If other minimizers are used see their specific documentation for the status code returned.
//     For example in the case of Fumili, for the status returned see TFumili::Minimize.
//
//      Excluding points
//      ================
//     Use TF1::RejectPoint inside your fitting function to exclude points
//     within a certain range from the fit. Example:
//     Double_t fline(Double_t *x, Double_t *par)
//     {
//         if (x[0] > 2.5 && x[0] < 3.5) {
//           TF1::RejectPoint();
//           return 0;
//        }
//        return par[0] + par[1]*x[0];
//     }
//
//     void exclude() {
//        TF1 *f1 = new TF1("f1", "[0] +[1]*x +gaus(2)", 0, 5);
//        f1->SetParameters(6, -1,5, 3, 0.2);
//        TH1F *h = new TH1F("h", "background + signal", 100, 0, 5);
//        h->FillRandom("f1", 2000);
//        TF1 *fline = new TF1("fline", fline, 0, 5, 2);
//        fline->SetParameters(2, -1);
//        h->Fit("fline", "l");
//     }
//
//      Warning when using the option "0"
//      =================================
//     When selecting the option "0", the fitted function is added to
//     the list of functions of the histogram, but it is not drawn.
//     You can undo what you disabled in the following way:
//       h.Fit("myFunction", "0"); // fit, store function but do not draw
//       h.Draw(); function is not drawn
//       const Int_t kNotDraw = 1<<9;
//       h.GetFunction("myFunction")->ResetBit(kNotDraw);
//       h.Draw();  // function is visible again
//
//      Access to the Minimizer information during fitting
//      ===============================================
//     This function calls, the ROOT::Fit::FitObject function implemented in HFitImpl.cxx
//     which uses the ROOT::Fit::Fitter class. The Fitter class creates the objective fuction
//     (e.g. chi2 or likelihood) and uses an implementation of the  Minimizer interface for minimizing
//     the function.
//     The default minimizer is Minuit (class TMinuitMinimizer which calls TMinuit).
//     The default  can be set in the resource file in etc/system.rootrc. For example
//     Root.Fitter:      Minuit2
//     A different fitter can also be set via ROOT::Math::MinimizerOptions::SetDefaultMinimizer
//     (or TVirtualFitter::SetDefaultFitter).
//     For example ROOT::Math::MinimizerOptions::SetDefaultMinimizer("GSLMultiMin","BFGS");
//     will set the usdage of the BFGS algorithm of the GSL multi-dimensional minimization
//     (implemented in libMathMore). ROOT::Math::MinimizerOptions can be used also to set other
//     default options, like maximum number of function calls, minimization tolerance or print
//     level. See the documentation of this class.
//
//     For fitting linear functions (containing the "++" sign" and polN functions,
//     the linear fitter is automatically initialized.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   // implementation of Fit method is in file hist/src/HFitImpl.cxx
   Foption_t fitOption;

   if (!FitOptionsMake(option,fitOption)) return 0;
   // create range and minimizer options with default values
   ROOT::Fit::DataRange range(xxmin,xxmax);
   ROOT::Math::MinimizerOptions minOption;

   // need to empty the buffer before
   // (t.b.d. do a ML unbinned fit with buffer data)
   if (fBuffer) BufferEmpty();

   return ROOT::Fit::FitObject(this, f1 , fitOption , minOption, goption, range);
}

//______________________________________________________________________________
void TH1::FitPanel()
{
//   -*-*-*-*-*Display a panel with all histogram fit options*-*-*-*-*-*
//             ==============================================
//
//      See class TFitPanel for example

   if (!gPad)
      gROOT->MakeDefCanvas();

   if (!gPad) {
      Error("FitPanel", "Unable to create a default canvas");
      return;
   }


   // use plugin manager to create instance of TFitEditor
   TPluginHandler *handler = gROOT->GetPluginManager()->FindHandler("TFitEditor");
   if (handler && handler->LoadPlugin() != -1) {
      if (handler->ExecPlugin(2, gPad, this) == 0)
         Error("FitPanel", "Unable to create the FitPanel");
   }
   else
         Error("FitPanel", "Unable to find the FitPanel plug-in");
}

//______________________________________________________________________________
TH1 *TH1::GetAsymmetry(TH1* h2, Double_t c2, Double_t dc2)
{
   //  return an histogram containing the asymmetry of this histogram with h2,
   //  where the asymmetry is defined as:
   //
   //  Asymmetry = (h1 - h2)/(h1 + h2)  where h1 = this
   //
   //  works for 1D, 2D, etc. histograms
   //  c2 is an optional argument that gives a relative weight between the two
   //  histograms, and dc2 is the error on this weight.  This is useful, for example,
   //  when forming an asymmetry between two histograms from 2 different data sets that
   //  need to be normalized to each other in some way.  The function calculates
   //  the errors asumming Poisson statistics on h1 and h2 (that is, dh = sqrt(h)).
   //
   //  example:  assuming 'h1' and 'h2' are already filled
   //
   //     h3 = h1->GetAsymmetry(h2)
   //
   //  then 'h3' is created and filled with the asymmetry between 'h1' and 'h2';
   //  h1 and h2 are left intact.
   //
   //  Note that it is the user's responsibility to manage the created histogram.
   //
   //  code proposed by Jason Seely (seely@mit.edu) and adapted by R.Brun
   //
   // clone the histograms so top and bottom will have the
   // correct dimensions:
   // Sumw2 just makes sure the errors will be computed properly
   // when we form sums and ratios below.
   Bool_t addStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TH1 *asym   = (TH1*)Clone();
   asym->Sumw2();
   TH1 *top    = (TH1*)asym->Clone();
   TH1 *bottom = (TH1*)asym->Clone();
   TH1::AddDirectory(addStatus);

   // form the top and bottom of the asymmetry, and then divide:
   TH1 *h1 = this;
   top->Add(h1,h2,1,-c2);
   bottom->Add(h1,h2,1,c2);
   asym->Divide(top,bottom);

   Int_t   xmax = asym->GetNbinsX();
   Int_t   ymax = asym->GetNbinsY();
   Int_t   zmax = asym->GetNbinsZ();
   Double_t bot, error, a, b, da, db;

   // now loop over bins to calculate the correct errors
   // the reason this error calculation looks complex is because of c2
   for(Int_t i=1; i<= xmax; i++){
      for(Int_t j=1; j<= ymax; j++){
         for(Int_t k=1; k<= zmax; k++){

            // here some bin contents are written into variables to make the error
            // calculation a little more legible:
            a   = h1->GetBinContent(i,j,k);
            b   = h2->GetBinContent(i,j,k);
            bot = bottom->GetBinContent(i,j,k);

            // make sure there are some events, if not, then the errors are set = 0
            // automatically.
            //if(bot < 1){} was changed to the next line from recommendation of Jason Seely (28 Nov 2005)
            if(bot < 1e-6){}
            else{
               // computation of errors by Christos Leonidopoulos
               da    = h1->GetBinError(i,j,k);
               db    = h2->GetBinError(i,j,k);
               error = 2*TMath::Sqrt(a*a*c2*c2*db*db + c2*c2*b*b*da*da+a*a*b*b*dc2*dc2)/(bot*bot);
               asym->SetBinError(i,j,k,error);
            }
         }
      }
   }
   delete top;
   delete bottom;
   return asym;
}

//______________________________________________________________________________
Int_t TH1::GetDefaultBufferSize()
{
   // static function
   // return the default buffer size for automatic histograms
   // the parameter fgBufferSize may be changed via SetDefaultBufferSize

   return fgBufferSize;
}


//______________________________________________________________________________
Bool_t TH1::GetDefaultSumw2()
{
   // static function
   // return kTRUE if TH1::Sumw2 must be called when creating new histograms.
   // see TH1::SetDefaultSumw2.

   return fgDefaultSumw2;
}


//______________________________________________________________________________
Double_t TH1::GetEntries() const
{
   // return the current number of entries

   if (fBuffer) {
      Int_t nentries = (Int_t) fBuffer[0];
      if (nentries > 0) return nentries;
   }

   return fEntries;
}

//______________________________________________________________________________
Double_t TH1::GetEffectiveEntries() const
{
   // number of effective entries of the histogram,
   // neff = (Sum of weights )^2 / (Sum of weight^2 )
   // In case of an unweighted histogram this number is equivalent to the
   // number of entries of the histogram. 
   // For a weighted histogram, this number corresponds to the hypotetical number of unweighted entries 
   // a histogram would need to have the same statistical power as this weighted histogram.
   // Note: The underflow/overflow are included if one has set the TH1::StatOverFlows flag
   // and if the statistics has been computed at filling time. 
   // If a range is set in the histogram the number is computed from the given range. 

   Stat_t s[kNstat];
   this->GetStats(s);// s[1] sum of squares of weights, s[0] sum of weights
   return (s[1] ? s[0]*s[0]/s[1] : TMath::Abs(s[0]) );
}

//______________________________________________________________________________
void TH1::SetHighlight(Bool_t set)
{
   // Set highlight (enable/disable) mode for the histogram
   // by default highlight mode is disable

   if (IsHighlight() == set) return;
   if (fDimension > 2) {
      Info("SetHighlight", "Supported only 1-D or 2-D histograms");
      return;
   }

   if (!fPainter) {
      Info("SetHighlight", "Need to draw histogram first");
      return;
   }
   SetBit(kIsHighlight, set);
   fPainter->SetHighlight();
}

//______________________________________________________________________________
char *TH1::GetObjectInfo(Int_t px, Int_t py) const
{
   //   Redefines TObject::GetObjectInfo.
   //   Displays the histogram info (bin number, contents, integral up to bin
   //   corresponding to cursor position px,py
   //
   return ((TH1*)this)->GetPainter()->GetObjectInfo(px,py);
}

//______________________________________________________________________________
TVirtualHistPainter *TH1::GetPainter(Option_t *option)
{
   // return pointer to painter
   // if painter does not exist, it is created
   if (!fPainter) {
      TString opt = option;
      opt.ToLower();
      if (opt.Contains("gl") || gStyle->GetCanvasPreferGL()) {
         //try to create TGLHistPainter
         TPluginHandler *handler = gROOT->GetPluginManager()->FindHandler("TGLHistPainter");

         if (handler && handler->LoadPlugin() != -1)
            fPainter = reinterpret_cast<TVirtualHistPainter *>(handler->ExecPlugin(1, this));
      }
   }

   if (!fPainter) fPainter = TVirtualHistPainter::HistPainter(this);

   return fPainter;
}

//______________________________________________________________________________
Int_t TH1::GetQuantiles(Int_t nprobSum, Double_t *q, const Double_t *probSum)
{
   //  Compute Quantiles for this histogram
   //     Quantile x_q of a probability distribution Function F is defined as
   //
   //        F(x_q) = q with 0 <= q <= 1.
   //
   //     For instance the median x_0.5 of a distribution is defined as that value
   //     of the random variable for which the distribution function equals 0.5:
   //
   //        F(x_0.5) = Probability(x < x_0.5) = 0.5
   //
   //  code from Eddy Offermann, Renaissance
   //
   // input parameters
   //   - this 1-d histogram (TH1F,D,etc). Could also be a TProfile
   //   - nprobSum maximum size of array q and size of array probSum (if given)
   //   - probSum array of positions where quantiles will be computed.
   //     if probSum is null, probSum will be computed internally and will
   //     have a size = number of bins + 1 in h. it will correspond to the
   //      quantiles calculated at the lowest edge of the histogram (quantile=0) and
   //     all the upper edges of the bins.
   //     if probSum is not null, it is assumed to contain at least nprobSum values.
   //  output
   //   - return value nq (<=nprobSum) with the number of quantiles computed
   //   - array q filled with nq quantiles
   //
   //  Note that the Integral of the histogram is automatically recomputed
   //  if the number of entries is different of the number of entries when
   //  the integral was computed last time. In case you do not use the Fill
   //  functions to fill your histogram, but SetBinContent, you must call
   //  TH1::ComputeIntegral before calling this function.
   //
   //  Getting quantiles q from two histograms and storing results in a TGraph,
   //   a so-called QQ-plot
   //
   //     TGraph *gr = new TGraph(nprob);
   //     h1->GetQuantiles(nprob,gr->GetX());
   //     h2->GetQuantiles(nprob,gr->GetY());
   //     gr->Draw("alp");
   //
   // Example:
   //     void quantiles() {
   //        // demo for quantiles
   //        const Int_t nq = 20;
   //        TH1F *h = new TH1F("h","demo quantiles",100,-3,3);
   //        h->FillRandom("gaus",5000);
   //
   //        Double_t xq[nq];  // position where to compute the quantiles in [0,1]
   //        Double_t yq[nq];  // array to contain the quantiles
   //        for (Int_t i=0;i<nq;i++) xq[i] = Float_t(i+1)/nq;
   //        h->GetQuantiles(nq,yq,xq);
   //
   //        //show the original histogram in the top pad
   //        TCanvas *c1 = new TCanvas("c1","demo quantiles",10,10,700,900);
   //        c1->Divide(1,2);
   //        c1->cd(1);
   //        h->Draw();
   //
   //        // show the quantiles in the bottom pad
   //        c1->cd(2);
   //        gPad->SetGrid();
   //        TGraph *gr = new TGraph(nq,xq,yq);
   //        gr->SetMarkerStyle(21);
   //        gr->Draw("alp");
   //     }

   if (GetDimension() > 1) {
      Error("GetQuantiles","Only available for 1-d histograms");
      return 0;
   }

   const Int_t nbins = GetXaxis()->GetNbins();
   if (!fIntegral) ComputeIntegral();
   if (fIntegral[nbins+1] != fEntries) ComputeIntegral();

   Int_t i, ibin;
   Double_t *prob = (Double_t*)probSum;
   Int_t nq = nprobSum;
   if (probSum == 0) {
      nq = nbins+1;
      prob = new Double_t[nq];
      prob[0] = 0;
      for (i=1;i<nq;i++) {
         prob[i] = fIntegral[i]/fIntegral[nbins];
      }
   }

   for (i = 0; i < nq; i++) {
      ibin = TMath::BinarySearch(nbins,fIntegral,prob[i]);
      while (ibin < nbins-1 && fIntegral[ibin+1] == prob[i]) {
         if (fIntegral[ibin+2] == prob[i]) ibin++;
         else break;
      }
      q[i] = GetBinLowEdge(ibin+1);
      const Double_t dint = fIntegral[ibin+1]-fIntegral[ibin];
      if (dint > 0) q[i] += GetBinWidth(ibin+1)*(prob[i]-fIntegral[ibin])/dint;
   }

   if (!probSum) delete [] prob;
   return nq;
}

//______________________________________________________________________________
Int_t TH1::FitOptionsMake(Option_t *choptin, Foption_t &fitOption)
{
   //   -*-*-*-*-*-*-*Decode string choptin and fill fitOption structure*-*-*-*-*-*
   //                 ================================================

   if (choptin == 0) return 1;
   if (strlen(choptin) == 0) return 1;
   TString opt = choptin;
   opt.ToUpper();
   if (opt.Contains("Q"))  fitOption.Quiet   = 1;
   if (opt.Contains("V")) {fitOption.Verbose = 1; fitOption.Quiet = 0;}
   if (opt.Contains("X"))  fitOption.Chi2    = 1;
   if (opt.Contains("W"))  fitOption.W1      = 1;
   if (opt.Contains("WW")) fitOption.W1      = 2; //all bins have weight=1, even empty bins
   // likelihood fit options
   if (opt.Contains("L")) {
      fitOption.Like    = 1;
      //if (opt.Contains("LL")) fitOption.Like    = 2;
      if (opt.Contains("W")){ fitOption.Like    = 2;  fitOption.W1=0;}//  (weighted likelihood)
      if (opt.Contains("MULTI")) {
         if (fitOption.Like == 2) fitOption.Like = 6; // weighted multinomial
         else fitOption.Like    = 4; // multinomial likelihood fit instead of Poisson
         opt.ReplaceAll("MULTI","");
      }
   }
   if (opt.Contains("E"))  fitOption.Errors  = 1;
   if (opt.Contains("M"))  fitOption.More    = 1;
   if (opt.Contains("R"))  fitOption.Range   = 1;
   if (opt.Contains("G"))  fitOption.Gradient= 1;
   if (opt.Contains("N"))  fitOption.Nostore = 1;
   if (opt.Contains("0"))  fitOption.Nograph = 1;
   if (opt.Contains("+"))  fitOption.Plus    = 1;
   if (opt.Contains("I"))  fitOption.Integral= 1;
   if (opt.Contains("B"))  fitOption.Bound   = 1;
   if (opt.Contains("U")) {fitOption.User    = 1; fitOption.Like = 0;}
   if (opt.Contains("F"))  fitOption.Minuit = 1;
   if (opt.Contains("C"))  fitOption.Nochisq = 1;
   if (opt.Contains("S"))  fitOption.StoreResult = 1;

   return 1;
}

//______________________________________________________________________________
void H1InitGaus()
{
   //   -*-*-*-*Compute Initial values of parameters for a gaussian*-*-*-*-*-*-*
   //           ===================================================

   Double_t allcha, sumx, sumx2, x, val, rms, mean;
   Int_t bin;
   const Double_t sqrtpi = 2.506628;

   //   - Compute mean value and RMS of the histogram in the given range
   TVirtualFitter *hFitter = TVirtualFitter::GetFitter();
   TH1 *curHist = (TH1*)hFitter->GetObjectFit();
   Int_t hxfirst = hFitter->GetXfirst();
   Int_t hxlast  = hFitter->GetXlast();
   Double_t valmax  = curHist->GetBinContent(hxfirst);
   Double_t binwidx = curHist->GetBinWidth(hxfirst);
   allcha = sumx = sumx2 = 0;
   for (bin=hxfirst;bin<=hxlast;bin++) {
      x       = curHist->GetBinCenter(bin);
      val     = TMath::Abs(curHist->GetBinContent(bin));
      if (val > valmax) valmax = val;
      sumx   += val*x;
      sumx2  += val*x*x;
      allcha += val;
   }
   if (allcha == 0) return;
   mean = sumx/allcha;
   rms  = sumx2/allcha - mean*mean;
   if (rms > 0) rms  = TMath::Sqrt(rms);
   else         rms  = 0;
   if (rms == 0) rms = binwidx*(hxlast-hxfirst+1)/4;
   //if the distribution is really gaussian, the best approximation
   //is binwidx*allcha/(sqrtpi*rms)
   //However, in case of non-gaussian tails, this underestimates
   //the normalisation constant. In this case the maximum value
   //is a better approximation.
   //We take the average of both quantities
   Double_t constant = 0.5*(valmax+binwidx*allcha/(sqrtpi*rms));

   //In case the mean value is outside the histo limits and
   //the RMS is bigger than the range, we take
   //  mean = center of bins
   //  rms  = half range
   Double_t xmin = curHist->GetXaxis()->GetXmin();
   Double_t xmax = curHist->GetXaxis()->GetXmax();
   if ((mean < xmin || mean > xmax) && rms > (xmax-xmin)) {
      mean = 0.5*(xmax+xmin);
      rms  = 0.5*(xmax-xmin);
   }
   TF1 *f1 = (TF1*)hFitter->GetUserFunc();
   f1->SetParameter(0,constant);
   f1->SetParameter(1,mean);
   f1->SetParameter(2,rms);
   f1->SetParLimits(2,0,10*rms);
}

//______________________________________________________________________________
void H1InitExpo()
{
   //   -*-*-*-*Compute Initial values of parameters for an exponential*-*-*-*-*
   //           =======================================================

   Double_t constant, slope;
   Int_t ifail;
   TVirtualFitter *hFitter = TVirtualFitter::GetFitter();
   Int_t hxfirst = hFitter->GetXfirst();
   Int_t hxlast  = hFitter->GetXlast();
   Int_t nchanx  = hxlast - hxfirst + 1;

   H1LeastSquareLinearFit(-nchanx, constant, slope, ifail);

   TF1 *f1 = (TF1*)hFitter->GetUserFunc();
   f1->SetParameter(0,constant);
   f1->SetParameter(1,slope);

}

//______________________________________________________________________________
void H1InitPolynom()
{
   //   -*-*-*-*Compute Initial values of parameters for a polynom*-*-*-*-*-*-*
   //           ===================================================

   Double_t fitpar[25];

   TVirtualFitter *hFitter = TVirtualFitter::GetFitter();
   TF1 *f1 = (TF1*)hFitter->GetUserFunc();
   Int_t hxfirst = hFitter->GetXfirst();
   Int_t hxlast  = hFitter->GetXlast();
   Int_t nchanx  = hxlast - hxfirst + 1;
   Int_t npar    = f1->GetNpar();

   if (nchanx <=1 || npar == 1) {
      TH1 *curHist = (TH1*)hFitter->GetObjectFit();
      fitpar[0] = curHist->GetSumOfWeights()/Double_t(nchanx);
   } else {
      H1LeastSquareFit( nchanx, npar, fitpar);
   }
   for (Int_t i=0;i<npar;i++) f1->SetParameter(i, fitpar[i]);
}

//______________________________________________________________________________
void H1LeastSquareFit(Int_t n, Int_t m, Double_t *a)
{
   //   -*-*-*-*-*-*Least squares lpolynomial fitting without weights*-*-*-*-*-*-*
   //               =================================================
   //
   //     n   number of points to fit
   //     m   number of parameters
   //     a   array of parameters
   //
   //      based on CERNLIB routine LSQ: Translated to C++ by Rene Brun
   //      (E.Keil.  revised by B.Schorr, 23.10.1981.)
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   const Double_t zero = 0.;
   const Double_t one = 1.;
   const Int_t idim = 20;

   Double_t  b[400]        /* was [20][20] */;
   Int_t i, k, l, ifail;
   Double_t power;
   Double_t da[20], xk, yk;

   if (m <= 2) {
      H1LeastSquareLinearFit(n, a[0], a[1], ifail);
      return;
   }
   if (m > idim || m > n) return;
   b[0]  = Double_t(n);
   da[0] = zero;
   for (l = 2; l <= m; ++l) {
      b[l-1]           = zero;
      b[m + l*20 - 21] = zero;
      da[l-1]          = zero;
   }
   TVirtualFitter *hFitter = TVirtualFitter::GetFitter();
   TH1 *curHist  = (TH1*)hFitter->GetObjectFit();
   Int_t hxfirst = hFitter->GetXfirst();
   Int_t hxlast  = hFitter->GetXlast();
   for (k = hxfirst; k <= hxlast; ++k) {
      xk     = curHist->GetBinCenter(k);
      yk     = curHist->GetBinContent(k);
      power  = one;
      da[0] += yk;
      for (l = 2; l <= m; ++l) {
         power   *= xk;
         b[l-1]  += power;
         da[l-1] += power*yk;
      }
      for (l = 2; l <= m; ++l) {
         power            *= xk;
         b[m + l*20 - 21] += power;
      }
   }
   for (i = 3; i <= m; ++i) {
      for (k = i; k <= m; ++k) {
         b[k - 1 + (i-1)*20 - 21] = b[k + (i-2)*20 - 21];
      }
   }
   H1LeastSquareSeqnd(m, b, idim, ifail, 1, da);

   for (i=0; i<m; ++i) a[i] = da[i];

}

//______________________________________________________________________________
void H1LeastSquareLinearFit(Int_t ndata, Double_t &a0, Double_t &a1, Int_t &ifail)
{
   //   -*-*-*-*-*-*-*-*Least square linear fit without weights*-*-*-*-*-*-*-*-*
   //                   =======================================
   //
   //      extracted from CERNLIB LLSQ: Translated to C++ by Rene Brun
   //      (added to LSQ by B. Schorr, 15.02.1982.)
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   Double_t xbar, ybar, x2bar;
   Int_t i, n;
   Double_t xybar;
   Double_t fn, xk, yk;
   Double_t det;

   n     = TMath::Abs(ndata);
   ifail = -2;
   xbar  = ybar  = x2bar = xybar = 0;
   TVirtualFitter *hFitter = TVirtualFitter::GetFitter();
   TH1 *curHist  = (TH1*)hFitter->GetObjectFit();
   Int_t hxfirst = hFitter->GetXfirst();
   Int_t hxlast  = hFitter->GetXlast();
   for (i = hxfirst; i <= hxlast; ++i) {
      xk = curHist->GetBinCenter(i);
      yk = curHist->GetBinContent(i);
      if (ndata < 0) {
         if (yk <= 0) yk = 1e-9;
         yk = TMath::Log(yk);
      }
      xbar  += xk;
      ybar  += yk;
      x2bar += xk*xk;
      xybar += xk*yk;
   }
   fn    = Double_t(n);
   det   = fn*x2bar - xbar*xbar;
   ifail = -1;
   if (det <= 0) {
      a0 = ybar/fn;
      a1 = 0;
      return;
   }
   ifail = 0;
   a0 = (x2bar*ybar - xbar*xybar) / det;
   a1 = (fn*xybar - xbar*ybar) / det;

}

//______________________________________________________________________________
void H1LeastSquareSeqnd(Int_t n, Double_t *a, Int_t idim, Int_t &ifail, Int_t k, Double_t *b)
{
   //   -*-*-*-*-*-*Extracted from CERN Program library routine DSEQN*-*-*-*-*-*
   //               =================================================
   //
   //           : Translated to C++ by Rene Brun
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Int_t a_dim1, a_offset, b_dim1, b_offset;
   Int_t nmjp1, i, j, l;
   Int_t im1, jp1, nm1, nmi;
   Double_t s1, s21, s22;
   const Double_t one = 1.;

   /* Parameter adjustments */
   b_dim1 = idim;
   b_offset = b_dim1 + 1;
   b -= b_offset;
   a_dim1 = idim;
   a_offset = a_dim1 + 1;
   a -= a_offset;

   if (idim < n) return;

   ifail = 0;
   for (j = 1; j <= n; ++j) {
      if (a[j + j*a_dim1] <= 0) { ifail = -1; return; }
      a[j + j*a_dim1] = one / a[j + j*a_dim1];
      if (j == n) continue;
      jp1 = j + 1;
      for (l = jp1; l <= n; ++l) {
         a[j + l*a_dim1] = a[j + j*a_dim1] * a[l + j*a_dim1];
         s1 = -a[l + (j+1)*a_dim1];
         for (i = 1; i <= j; ++i) { s1 = a[l + i*a_dim1] * a[i + (j+1)*a_dim1] + s1; }
         a[l + (j+1)*a_dim1] = -s1;
      }
   }
   if (k <= 0) return;

   for (l = 1; l <= k; ++l) {
      b[l*b_dim1 + 1] = a[a_dim1 + 1]*b[l*b_dim1 + 1];
   }
   if (n == 1) return;
   for (l = 1; l <= k; ++l) {
      for (i = 2; i <= n; ++i) {
         im1 = i - 1;
         s21 = -b[i + l*b_dim1];
         for (j = 1; j <= im1; ++j) {
            s21 = a[i + j*a_dim1]*b[j + l*b_dim1] + s21;
         }
         b[i + l*b_dim1] = -a[i + i*a_dim1]*s21;
      }
      nm1 = n - 1;
      for (i = 1; i <= nm1; ++i) {
         nmi = n - i;
         s22 = -b[nmi + l*b_dim1];
         for (j = 1; j <= i; ++j) {
            nmjp1 = n - j + 1;
            s22 = a[nmi + nmjp1*a_dim1]*b[nmjp1 + l*b_dim1] + s22;
         }
         b[nmi + l*b_dim1] = -s22;
      }
   }
}


//______________________________________________________________________________
Int_t TH1::GetBin(Int_t binx, Int_t biny, Int_t binz) const
{
   //   -*-*-*-*Return Global bin number corresponding to binx,y,z*-*-*-*-*-*-*
   //           ==================================================
   //
   //      2-D and 3-D histograms are represented with a one dimensional
   //      structure.
   //      This has the advantage that all existing functions, such as
   //        GetBinContent, GetBinError, GetBinFunction work for all dimensions.
   //
   //     In case of a TH1x, returns binx directly.
   //     see TH1::GetBinXYZ for the inverse transformation.
   //
   //      Convention for numbering bins
   //      =============================
   //      For all histogram types: nbins, xlow, xup
   //        bin = 0;       underflow bin
   //        bin = 1;       first bin with low-edge xlow INCLUDED
   //        bin = nbins;   last bin with upper-edge xup EXCLUDED
   //        bin = nbins+1; overflow bin
   //      In case of 2-D or 3-D histograms, a "global bin" number is defined.
   //      For example, assuming a 3-D histogram with binx,biny,binz, the function
   //        Int_t bin = h->GetBin(binx,biny,binz);
   //      returns a global/linearized bin number. This global bin is useful
   //      to access the bin information independently of the dimension.
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Int_t nx, ny, nz;
   if (GetDimension() < 2) {
      nx  = fXaxis.GetNbins()+2;
      if (binx < 0)   binx = 0;
      if (binx >= nx) binx = nx-1;
      return binx;
   }
   if (GetDimension() < 3) {
      nx  = fXaxis.GetNbins()+2;
      if (binx < 0)   binx = 0;
      if (binx >= nx) binx = nx-1;
      ny  = fYaxis.GetNbins()+2;
      if (biny < 0)   biny = 0;
      if (biny >= ny) biny = ny-1;
      return  binx + nx*biny;
   }
   if (GetDimension() < 4) {
      nx  = fXaxis.GetNbins()+2;
      if (binx < 0)   binx = 0;
      if (binx >= nx) binx = nx-1;
      ny  = fYaxis.GetNbins()+2;
      if (biny < 0)   biny = 0;
      if (biny >= ny) biny = ny-1;
      nz  = fZaxis.GetNbins()+2;
      if (binz < 0)   binz = 0;
      if (binz >= nz) binz = nz-1;
      return  binx + nx*(biny +ny*binz);
   }
   return -1;
}
//______________________________________________________________________________
void TH1::GetBinXYZ(Int_t binglobal, Int_t &binx, Int_t &biny, Int_t &binz) const
{
   // return binx, biny, binz corresponding to the global bin number globalbin
   // see TH1::GetBin function above

   Int_t nx  = fXaxis.GetNbins()+2;
   Int_t ny  = fYaxis.GetNbins()+2;

   if (GetDimension() < 2) {
      binx = binglobal%nx;
      biny = -1;
      binz = -1;
   }
   if (GetDimension() < 3) {
      binx = binglobal%nx;
      biny = ((binglobal-binx)/nx)%ny;
      binz = -1;
   }
   if (GetDimension() < 4) {
      binx = binglobal%nx;
      biny = ((binglobal-binx)/nx)%ny;
      binz = ((binglobal-binx)/nx -biny)/ny;
   }
}


//______________________________________________________________________________
Double_t TH1::GetRandom() const
{
   // return a random number distributed according the histogram bin contents.
   // This function checks if the bins integral exists. If not, the integral
   // is evaluated, normalized to one.
   // The integral is automatically recomputed if the number of entries
   // is not the same then when the integral was computed.
   // NB Only valid for 1-d histograms. Use GetRandom2 or 3 otherwise.
   // If the histogram has a bin with negative content a NaN is returned 

   if (fDimension > 1) {
      Error("GetRandom","Function only valid for 1-d histograms");
      return 0;
   }
   Int_t nbinsx = GetNbinsX();
   Double_t integral = 0;
   // compute integral checking that all bins have positive content (see ROOT-5894)
   if (fIntegral) {
      if (fIntegral[nbinsx+1] != fEntries) integral = ((TH1*)this)->ComputeIntegral(true);
      else  integral = fIntegral[nbinsx];
   } else {
      integral = ((TH1*)this)->ComputeIntegral(true);
   }
   if (integral == 0) return 0;
   // return a NaN in case some bins have negative content
   if (integral == TMath::QuietNaN() ) return TMath::QuietNaN(); 

   Double_t r1 = gRandom->Rndm();
   Int_t ibin = TMath::BinarySearch(nbinsx,fIntegral,r1);
   Double_t x = GetBinLowEdge(ibin+1);
   if (r1 > fIntegral[ibin]) x +=
      GetBinWidth(ibin+1)*(r1-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);
   return x;
}

//______________________________________________________________________________
Double_t TH1::GetBinContent(Int_t) const
{
   //   -*-*-*-*-*Return content of bin number bin
   //             ================================
   // Implemented in TH1C,S,F,D
   //
   //      Convention for numbering bins
   //      =============================
   //      For all histogram types: nbins, xlow, xup
   //        bin = 0;       underflow bin
   //        bin = 1;       first bin with low-edge xlow INCLUDED
   //        bin = nbins;   last bin with upper-edge xup EXCLUDED
   //        bin = nbins+1; overflow bin
   //      In case of 2-D or 3-D histograms, a "global bin" number is defined.
   //      For example, assuming a 3-D histogram with binx,biny,binz, the function
   //        Int_t bin = h->GetBin(binx,biny,binz);
   //      returns a global/linearized bin number. This global bin is useful
   //      to access the bin information independently of the dimension.

   AbstractMethod("GetBinContent");
   return 0;
}

//______________________________________________________________________________
Double_t TH1::GetBinContent(Int_t binx, Int_t biny) const
{
   //   -*-*-*-*-*Return content of bin number binx, biny
   //             =======================================
   // NB: Function to be called for 2-D histograms only
   // see convention for numbering bins in TH1::GetBin

   Int_t bin = GetBin(binx,biny);
   return GetBinContent(bin);
}

//______________________________________________________________________________
Double_t TH1::GetBinContent(Int_t binx, Int_t biny, Int_t binz) const
{
   //   -*-*-*-*-*Return content of bin number binx,biny,binz
   //             ===========================================
   // NB: Function to be called for 3-D histograms only
   // see convention for numbering bins in TH1::GetBin

   Int_t bin = GetBin(binx,biny,binz);
   return GetBinContent(bin);
}

//______________________________________________________________________________
Double_t TH1::GetBinWithContent(Double_t c, Int_t &binx, Int_t firstx, Int_t lastx,Double_t maxdiff) const
{
   // compute first binx in the range [firstx,lastx] for which
   // diff = abs(bin_content-c) <= maxdiff
   // In case several bins in the specified range with diff=0 are found
   // the first bin found is returned in binx.
   // In case several bins in the specified range satisfy diff <=maxdiff
   // the bin with the smallest difference is returned in binx.
   // In all cases the function returns the smallest difference.
   //
   // NOTE1: if firstx <= 0, firstx is set to bin 1
   //        if (lastx < firstx then firstx is set to the number of bins
   //        ie if firstx=0 and lastx=0 (default) the search is on all bins.
   // NOTE2: if maxdiff=0 (default), the first bin with content=c is returned.

   if (fDimension > 1) {
      binx = 0;
      Error("GetBinWithContent","function is only valid for 1-D histograms");
      return 0;
   }
   if (firstx <= 0) firstx = 1;
   if (lastx < firstx) lastx = fXaxis.GetNbins();
   Int_t binminx = 0;
   Double_t diff, curmax = 1.e240;
   for (Int_t i=firstx;i<=lastx;i++) {
      diff = TMath::Abs(GetBinContent(i)-c);
      if (diff <= 0) {binx = i; return diff;}
      if (diff < curmax && diff <= maxdiff) {curmax = diff, binminx=i;}
   }
   binx = binminx;
   return curmax;
}

//______________________________________________________________________________
TAxis *TH1::GetXaxis() const
{
   // return a pointer to the X axis object

   return &((TH1*)this)->fXaxis;
}


//______________________________________________________________________________
TAxis *TH1::GetYaxis() const
{
   // return a pointer to the Y axis object

   return &((TH1*)this)->fYaxis;
}
//______________________________________________________________________________
TAxis *TH1::GetZaxis() const
{
   // return a pointer to the Z axis object

   return &((TH1*)this)->fZaxis;
}

//______________________________________________________________________________
Double_t TH1::Interpolate(Double_t x)
{
   // Given a point x, approximates the value via linear interpolation
   // based on the two nearest bin centers
   // Andy Mastbaum 10/21/08

   Int_t xbin = FindBin(x);
   Double_t x0,x1,y0,y1;

   if(x<=GetBinCenter(1)) {
      return GetBinContent(1);
   } else if(x>=GetBinCenter(GetNbinsX())) {
      return GetBinContent(GetNbinsX());
   } else {
      if(x<=GetBinCenter(xbin)) {
         y0 = GetBinContent(xbin-1);
         x0 = GetBinCenter(xbin-1);
         y1 = GetBinContent(xbin);
         x1 = GetBinCenter(xbin);
      } else {
         y0 = GetBinContent(xbin);
         x0 = GetBinCenter(xbin);
         y1 = GetBinContent(xbin+1);
         x1 = GetBinCenter(xbin+1);
      }
      return y0 + (x-x0)*((y1-y0)/(x1-x0));
   }
}

//______________________________________________________________________________
Double_t TH1::Interpolate(Double_t, Double_t)
{

   //Not yet implemented
   Error("Interpolate","This function must be called with 1 argument for a TH1");
   return 0;
}

//______________________________________________________________________________
Double_t TH1::Interpolate(Double_t, Double_t, Double_t)
{

   //Not yet implemented
   Error("Interpolate","This function must be called with 1 argument for a TH1");
   return 0;
}

//______________________________________________________________________________
Bool_t TH1::IsBinOverflow(Int_t bin) const
{

   // Return true if the bin is overflow.
   Int_t binx, biny, binz;
   GetBinXYZ(bin, binx, biny, binz);

   if ( fDimension == 1 )
      return binx >= GetNbinsX() + 1;
   else if ( fDimension == 2 )
      return (binx >= GetNbinsX() + 1) ||
             (biny >= GetNbinsY() + 1);
   else if ( fDimension == 3 )
      return (binx >= GetNbinsX() + 1) ||
             (biny >= GetNbinsY() + 1) ||
             (binz >= GetNbinsZ() + 1);
   else
      return 0;
}


//______________________________________________________________________________
Bool_t TH1::IsBinUnderflow(Int_t bin) const
{

   // Return true if the bin is overflow.
   Int_t binx, biny, binz;
   GetBinXYZ(bin, binx, biny, binz);

   if ( fDimension == 1 )
      return (binx <= 0);
   else if ( fDimension == 2 )
      return (binx <= 0 || biny <= 0);
   else if ( fDimension == 3 )
      return (binx <= 0 || biny <= 0 || binz <= 0);
   else
      return 0;
}

//___________________________________________________________________________
void TH1::LabelsDeflate(Option_t *ax)
{
   // Reduce the number of bins for the axis passed in the option to the number of bins having a label.
   // The method will remove only the extra bins existing after the last "labeled" bin.
   // Note that if there are "un-labeled" bins present between "labeled" bins they will not be removed

   Int_t iaxis = AxisChoice(ax);
   TAxis *axis = 0;
   if (iaxis == 1) axis = GetXaxis();
   if (iaxis == 2) axis = GetYaxis();
   if (iaxis == 3) axis = GetZaxis();
   if (!axis) {
      Error("LabelsDeflate","Invalid axis option %s",ax);
      return;
   }
   if (!axis->GetLabels()) return;

   // find bin with last labels
   // bin number is object ID in list of labels
   // therefore max bin number is number of bins of the deflated histograms
   TIter next(axis->GetLabels());
   TObject *obj;
   Int_t nbins = 0;
   while ((obj = next())) {
      Int_t ibin = obj->GetUniqueID();
      if (ibin > nbins) nbins = ibin;
   }
   if (nbins < 1) nbins = 1;
   TH1 *hold = (TH1*)IsA()->New();
   R__ASSERT(hold);
   hold->SetDirectory(0);
   Copy(*hold);

   Bool_t timedisp = axis->GetTimeDisplay();
   Double_t xmin = axis->GetXmin();
   Double_t xmax = axis->GetBinUpEdge(nbins);
   if (xmax <= xmin) xmax = xmin +nbins;
   axis->SetRange(0,0);
   axis->Set(nbins,xmin,xmax);
   SetBinsLength(-1);  // reset the number of cells
   Int_t errors = fSumw2.fN;
   if (errors) fSumw2.Set(fNcells);
   axis->SetTimeDisplay(timedisp);
   // reset histogram content
   Reset("ICE");

   //now loop on all bins and refill
   // NOTE that if the bins without labels have content
   // it will be put in the underflow/overflow.
   // For this reason we use AddBinContent method
   Double_t oldEntries = fEntries;
   Int_t bin,binx,biny,binz;
   for (bin=0; bin < hold->fNcells; ++bin) {
      hold->GetBinXYZ(bin,binx,biny,binz);
      Int_t ibin = GetBin(binx,biny,binz);
      Double_t cu = hold->GetBinContent(bin);
      AddBinContent(ibin,cu);
      if (errors) {
         fSumw2.fArray[ibin] += hold->fSumw2.fArray[bin];
      }
   }
   fEntries = oldEntries;
   delete hold;
}

//______________________________________________________________________________
void TH1::LabelsInflate(Option_t *ax)
{
   // Double the number of bins for axis.
   // Refill histogram
   // This function is called by TAxis::FindBin(const char *label)

   Int_t iaxis = AxisChoice(ax);
   TAxis *axis = 0;
   if (iaxis == 1) axis = GetXaxis();
   if (iaxis == 2) axis = GetYaxis();
   if (iaxis == 3) axis = GetZaxis();
   if (!axis) return;

   TH1 *hold = (TH1*)IsA()->New();;
   hold->SetDirectory(0);
   Copy(*hold);

   Bool_t timedisp = axis->GetTimeDisplay();
   Int_t  nbxold = fXaxis.GetNbins();
   Int_t  nbyold = fYaxis.GetNbins();
   Int_t  nbzold = fZaxis.GetNbins();
   Int_t nbins   = axis->GetNbins();
   Double_t xmin = axis->GetXmin();
   Double_t xmax = axis->GetXmax();
   xmax = xmin + 2*(xmax-xmin);
   axis->SetRange(0,0);
   // double the bins and recompute ncells
   axis->Set(2*nbins,xmin,xmax);
   SetBinsLength(-1);
   Int_t errors = fSumw2.fN;
   if (errors) fSumw2.Set(fNcells);
   axis->SetTimeDisplay(timedisp);

   Reset("ICE");  // reset content and error

   //now loop on all bins and refill
   Double_t oldEntries = fEntries;
   Int_t bin,ibin,binx,biny,binz;
   for (ibin =0; ibin < fNcells; ibin++) {
      GetBinXYZ(ibin,binx,biny,binz);
      bin = hold->GetBin(binx,biny,binz);
      // NOTE that overflow in hold will be not considered
      if (binx > nbxold  || biny > nbyold || binz > nbzold) bin = -1;
      if (bin > 0)  {
         Double_t cu  = hold->GetBinContent(bin);
         AddBinContent(ibin,cu);
         if (errors) fSumw2.fArray[ibin] += hold->fSumw2.fArray[bin];
      }
   }
   fEntries = oldEntries;
   delete hold;
}

//______________________________________________________________________________
void TH1::LabelsOption(Option_t *option, Option_t *ax)
{
   //  Set option(s) to draw axis with labels
   //  option = "a" sort by alphabetic order
   //         = ">" sort by decreasing values
   //         = "<" sort by increasing values
   //         = "h" draw labels horizontal
   //         = "v" draw labels vertical
   //         = "u" draw labels up (end of label right adjusted)
   //         = "d" draw labels down (start of label left adjusted)

   Int_t iaxis = AxisChoice(ax);
   TAxis *axis = 0;
   if (iaxis == 1) axis = GetXaxis();
   if (iaxis == 2) axis = GetYaxis();
   if (iaxis == 3) axis = GetZaxis();
   if (!axis) return;
   THashList *labels = axis->GetLabels();
   if (!labels) {
      Warning("LabelsOption","Cannot sort. No labels");
      return;
   }
   TString opt = option;
   opt.ToLower();
   if (opt.Contains("h")) {
      axis->SetBit(TAxis::kLabelsHori);
      axis->ResetBit(TAxis::kLabelsVert);
      axis->ResetBit(TAxis::kLabelsDown);
      axis->ResetBit(TAxis::kLabelsUp);
   }
   if (opt.Contains("v")) {
      axis->SetBit(TAxis::kLabelsVert);
      axis->ResetBit(TAxis::kLabelsHori);
      axis->ResetBit(TAxis::kLabelsDown);
      axis->ResetBit(TAxis::kLabelsUp);
   }
   if (opt.Contains("u")) {
      axis->SetBit(TAxis::kLabelsUp);
      axis->ResetBit(TAxis::kLabelsVert);
      axis->ResetBit(TAxis::kLabelsDown);
      axis->ResetBit(TAxis::kLabelsHori);
   }
   if (opt.Contains("d")) {
      axis->SetBit(TAxis::kLabelsDown);
      axis->ResetBit(TAxis::kLabelsVert);
      axis->ResetBit(TAxis::kLabelsHori);
      axis->ResetBit(TAxis::kLabelsUp);
   }
   Int_t sort = -1;
   if (opt.Contains("a")) sort = 0;
   if (opt.Contains(">")) sort = 1;
   if (opt.Contains("<")) sort = 2;
   if (sort < 0) return;
   if (sort > 0 && GetDimension() > 2) {
      Error("LabelsOption","Sorting by value not implemented for 3-D histograms");
      return;
   }

   Double_t entries = fEntries;
   Int_t n = TMath::Min(axis->GetNbins(), labels->GetSize());
   Int_t *a = new Int_t[n+2];

   Int_t i,j,k;
   Double_t *cont   = 0;
   Double_t *errors = 0;
   THashList *labold = new THashList(labels->GetSize(),1);
   TIter nextold(labels);
   TObject *obj;
   while ((obj=nextold())) {
      labold->Add(obj);
   }
   labels->Clear();
   if (sort > 0) {
      //---sort by values of bins
      if (GetDimension() == 1) {
         cont = new Double_t[n];
         if (fSumw2.fN) errors = new Double_t[n];
         for (i=1;i<=n;i++) {
            cont[i-1] = GetBinContent(i);
            if (errors) errors[i-1] = GetBinError(i);
         }
         if (sort ==1) TMath::Sort(n,cont,a,kTRUE);  //sort by decreasing values
         else          TMath::Sort(n,cont,a,kFALSE); //sort by increasing values
         for (i=1;i<=n;i++) {
            SetBinContent(i,cont[a[i-1]]);
            if (errors) SetBinError(i,errors[a[i-1]]);
         }
         for (i=1;i<=n;i++) {
            obj = labold->At(a[i-1]);
            labels->Add(obj);
            obj->SetUniqueID(i);
         }
      } else if (GetDimension()== 2) {
         Double_t *pcont = new Double_t[n+2];
         for (i=0;i<=n;i++) pcont[i] = 0;
         Int_t nx = fXaxis.GetNbins();
         Int_t ny = fYaxis.GetNbins();
         cont = new Double_t[(nx+2)*(ny+2)];
         if (fSumw2.fN) errors = new Double_t[(nx+2)*(ny+2)];
         for (i=1;i<=nx;i++) {
            for (j=1;j<=ny;j++) {
               cont[i+nx*j] = GetBinContent(i,j);
               if (errors) errors[i+nx*j] = GetBinError(i,j);
               if (axis == GetXaxis()) k = i;
               else                    k = j;
               pcont[k-1] += cont[i+nx*j];
            }
         }
         if (sort ==1) TMath::Sort(n,pcont,a,kTRUE);  //sort by decreasing values
         else          TMath::Sort(n,pcont,a,kFALSE); //sort by increasing values
         for (i=0;i<n;i++) {
            obj = labold->At(a[i]);
            labels->Add(obj);
            obj->SetUniqueID(i+1);
         }
         delete [] pcont;
         if (axis == GetXaxis()) {
            for (i=1;i<=n;i++) {
               for (j=1;j<=ny;j++) {
                  SetBinContent(i,j,cont[a[i-1]+1+nx*j]);
                  if (errors) SetBinError(i,j,errors[a[i-1]+1+nx*j]);
               }
            }
         }
         else {
            // using y axis
            for (i=1;i<=nx;i++) {
               for (j=1;j<=n;j++) {
                  SetBinContent(i,j,cont[i+nx*(a[j-1]+1)]);
                  if (errors) SetBinError(i,j,errors[i+nx*(a[j-1]+1)]);
               }
            }
         }
      } else {
         //to be implemented for 3d
      }
   } else {
      //---alphabetic sort
      const UInt_t kUsed = 1<<18;
      TObject *objk=0;
      a[0] = 0;
      a[n+1] = n+1;
      for (i=1;i<=n;i++) {
         const char *label = "zzzzzzzzzzzz";
         for (j=1;j<=n;j++) {
            obj = labold->At(j-1);
            if (!obj) continue;
            if (obj->TestBit(kUsed)) continue;
            //use strcasecmp for case non-sensitive sort (may be an option)
            if (strcmp(label,obj->GetName()) < 0) continue;
            objk = obj;
            a[i] = j;
            label = obj->GetName();
         }
         if (objk) {
            objk->SetUniqueID(i);
            labels->Add(objk);
            objk->SetBit(kUsed);
         }
      }
      for (i=1;i<=n;i++) {
         obj = labels->At(i-1);
         if (!obj) continue;
         obj->ResetBit(kUsed);
      }

      if (GetDimension() == 1) {
         cont = new Double_t[n+2];
         if (fSumw2.fN) errors = new Double_t[n+2];
         for (i=1;i<=n;i++) {
            cont[i] = GetBinContent(a[i]);
            if (errors) errors[i] = GetBinError(a[i]);
         }
         for (i=1;i<=n;i++) {
            SetBinContent(i,cont[i]);
            if (errors) SetBinError(i,errors[i]);
         }
      } else if (GetDimension()== 2) {
         Int_t nx = fXaxis.GetNbins()+2;
         Int_t ny = fYaxis.GetNbins()+2;
         cont = new Double_t[nx*ny];
         if (fSumw2.fN) errors = new Double_t[nx*ny];
         for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++) {
               cont[i+nx*j] = GetBinContent(i,j);
               if (errors) errors[i+nx*j] = GetBinError(i,j);
            }
         }
         if (axis == GetXaxis()) {
            for (i=1;i<=n;i++) {
               for (j=0;j<ny;j++) {
                  SetBinContent(i,j,cont[a[i]+nx*j]);
                  if (errors) SetBinError(i,j,errors[a[i]+nx*j]);
               }
            }
         } else {
            for (i=0;i<nx;i++) {
               for (j=1;j<=n;j++) {
                  SetBinContent(i,j,cont[i+nx*a[j]]);
                  if (errors) SetBinError(i,j,errors[i+nx*a[j]]);
               }
            }
         }
      } else {
         Int_t nx = fXaxis.GetNbins()+2;
         Int_t ny = fYaxis.GetNbins()+2;
         Int_t nz = fZaxis.GetNbins()+2;
         cont = new Double_t[nx*ny*nz];
         if (fSumw2.fN) errors = new Double_t[nx*ny*nz];
         for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++) {
               for (k=0;k<nz;k++) {
                  cont[i+nx*(j+ny*k)] = GetBinContent(i,j,k);
                  if (errors) errors[i+nx*(j+ny*k)] = GetBinError(i,j,k);
               }
            }
         }
         if (axis == GetXaxis()) {
            // labels on x axis
            for (i=1;i<=n;i++) {
               for (j=0;j<ny;j++) {
                  for (k=0;k<nz;k++) {
                     SetBinContent(i,j,k,cont[a[i]+nx*(j+ny*k)]);
                     if (errors) SetBinError(i,j,k,errors[a[i]+nx*(j+ny*k)]);
                  }
               }
            }
         }
         else if (axis == GetYaxis()) {
            // labels on y axis
            for (i=0;i<nx;i++) {
               for (j=1;j<=n;j++) {
                  for (k=0;k<nz;k++) {
                     SetBinContent(i,j,k,cont[i+nx*(a[j]+ny*k)]);
                     if (errors) SetBinError(i,j,k,errors[i+nx*(a[j]+ny*k)]);
                  }
               }
            }
         }
         else {
            // labels on z axis
            for (i=0;i<nx;i++) {
               for (j=0;j<ny;j++) {
                  for (k=1;k<=n;k++) {
                     SetBinContent(i,j,k,cont[i+nx*(j+ny*a[k])]);
                     if (errors) SetBinError(i,j,k,errors[i+nx*(j+ny*a[k])]);
                  }
               }
            }
         }
      }
   }
   fEntries = entries;
   delete labold;
   if (a)      delete [] a;
   if (cont)   delete [] cont;
   if (errors) delete [] errors;
}

//______________________________________________________________________________
static inline Bool_t AlmostEqual(Double_t a, Double_t b, Double_t epsilon = 0.00000001)
{
   return TMath::Abs(a - b) < epsilon;
}

//______________________________________________________________________________
static inline Bool_t AlmostInteger(Double_t a, Double_t epsilon = 0.00000001)
{
   return AlmostEqual(a - TMath::Floor(a), 0, epsilon) ||
      AlmostEqual(a - TMath::Floor(a), 1, epsilon);
}

//______________________________________________________________________________
Bool_t TH1::SameLimitsAndNBins(const TAxis& axis1, const TAxis& axis2)
{
   // Same limits and bins.

   if ((axis1.GetNbins() == axis2.GetNbins())
      && (axis1.GetXmin() == axis2.GetXmin())
      && (axis1.GetXmax() == axis2.GetXmax()))
      return kTRUE;
   else
      return kFALSE;
}

static inline bool IsEquidistantBinning(const TAxis& axis)
{
   // check if axis bin are equals
   if (!axis.GetXbins()->fN) return true;  //  
   // not able to check if there is only one axis entry
   bool isEquidistant = true;
   const Double_t firstBinWidth = axis.GetBinWidth(1);
   for (int i = 1; i < axis.GetNbins(); ++i) {
      const Double_t binWidth = axis.GetBinWidth(i);
      const bool match = TMath::AreEqualRel(firstBinWidth, binWidth, TMath::Limits<Double_t>::Epsilon());
      isEquidistant &= match;
      if (!match)
         break;
   }
   return isEquidistant;
}

//______________________________________________________________________________
Bool_t TH1::RecomputeAxisLimits(TAxis& destAxis, const TAxis& anAxis)
{
   // Finds new limits for the axis for the Merge function.
   // returns false if the limits are incompatible
   if (SameLimitsAndNBins(destAxis, anAxis))
      return kTRUE;

   if (!IsEquidistantBinning(destAxis) || !IsEquidistantBinning(anAxis))
      return kFALSE;       // not equidistant user binning not supported

   Double_t width1 = destAxis.GetBinWidth(0);
   Double_t width2 = anAxis.GetBinWidth(0);
   if (width1 == 0 || width2 == 0)
      return kFALSE;       // no binning not supported

   Double_t xmin = TMath::Min(destAxis.GetXmin(), anAxis.GetXmin());
   Double_t xmax = TMath::Max(destAxis.GetXmax(), anAxis.GetXmax());
   Double_t width = TMath::Max(width1, width2);

   // check the bin size
   if (!AlmostInteger(width/width1) || !AlmostInteger(width/width2))
      return kFALSE;

   // std::cout << "Find new limit using given axis " << anAxis.GetXmin() << " , " <<  anAxis.GetXmax() << " bin width " << width2 << std::endl;
   // std::cout << "           and destination axis " << destAxis.GetXmin() << " , " <<  destAxis.GetXmax() << " bin width " << width1 << std::endl;


   // check the limits
   Double_t delta;
   delta = (destAxis.GetXmin() - xmin)/width1;
   if (!AlmostInteger(delta))
      xmin -= (TMath::Ceil(delta) - delta)*width1;

   delta = (anAxis.GetXmin() - xmin)/width2;
   if (!AlmostInteger(delta))
      xmin -= (TMath::Ceil(delta) - delta)*width2;


   delta = (destAxis.GetXmin() - xmin)/width1;
   if (!AlmostInteger(delta))
      return kFALSE;


   delta = (xmax - destAxis.GetXmax())/width1;
   if (!AlmostInteger(delta))
      xmax += (TMath::Ceil(delta) - delta)*width1;


   delta = (xmax - anAxis.GetXmax())/width2;
   if (!AlmostInteger(delta))
      xmax += (TMath::Ceil(delta) - delta)*width2;


   delta = (xmax - destAxis.GetXmax())/width1;
   if (!AlmostInteger(delta))
      return kFALSE;
#ifdef DEBUG
   if (!AlmostInteger((xmax - xmin) / width)) {   // unnecessary check
      printf("TH1::RecomputeAxisLimits - Impossible\n");
      return kFALSE;
   }
#endif


   destAxis.Set(TMath::Nint((xmax - xmin)/width), xmin, xmax);

   //std::cout << "New re-computed axis : [ " << xmin << " , " << xmax << " ] width = " << width << " nbins " << destAxis.GetNbins() << std::endl;
   
   return kTRUE;
}

//______________________________________________________________________________
Long64_t TH1::Merge(TCollection *li)
{
   // Add all histograms in the collection to this histogram.
   // This function computes the min/max for the x axis,
   // compute a new number of bins, if necessary,
   // add bin contents, errors and statistics.
   // If all histograms have bin labels, bins with identical labels
   // will be merged, no matter what their order is.
   // If overflows are present and limits are different the function will fail.
   // The function returns the total number of entries in the result histogram
   // if the merge is successful, -1 otherwise.
   //
   // IMPORTANT remark. The axis x may have different number
   // of bins and different limits, BUT the largest bin width must be
   // a multiple of the smallest bin width and the upper limit must also
   // be a multiple of the bin width.
   // Example:
   // void atest() {
   //    TH1F *h1 = new TH1F("h1","h1",110,-110,0);
   //    TH1F *h2 = new TH1F("h2","h2",220,0,110);
   //    TH1F *h3 = new TH1F("h3","h3",330,-55,55);
   //    TRandom r;
   //    for (Int_t i=0;i<10000;i++) {
   //       h1->Fill(r.Gaus(-55,10));
   //       h2->Fill(r.Gaus(55,10));
   //       h3->Fill(r.Gaus(0,10));
   //    }
   //
   //    TList *list = new TList;
   //    list->Add(h1);
   //    list->Add(h2);
   //    list->Add(h3);
   //    TH1F *h = (TH1F*)h1->Clone("h");
   //    h->Reset();
   //    h->Merge(list);
   //    h->Draw();
   // }

   if (!li) return 0;
   if (li->IsEmpty()) return (Long64_t) GetEntries();

   // is this really needed ?
   TList inlist;
   inlist.AddAll(li);


   TAxis newXAxis;

   Bool_t initialLimitsFound = kFALSE;
   Bool_t allHaveLabels = kTRUE;  // assume all histo have labels and check later
   Bool_t allHaveLimits = kTRUE;
   Bool_t allSameLimits = kTRUE;
   Bool_t foundLabelHist = kFALSE;
   //Bool_t firstHistWithLimits = kTRUE;


   TIter next(&inlist);
   // start looping with this histogram
   TH1 * h = this;

   do  {
      // do not skip anymore empty histograms 
      // since are used to set the limits 
      Bool_t hasLimits = h->GetXaxis()->GetXmin() < h->GetXaxis()->GetXmax();
      allHaveLimits = allHaveLimits && hasLimits;

      if (hasLimits) {
         h->BufferEmpty();

         // this is done in case the first histograms are empty and
         // the histogram have different limits
#ifdef LATER
         if (firstHistWithLimits ) {
            // set axis limits in the case the first histogram did not have limits 
            if (h != this && !SameLimitsAndNBins( fXaxis, *h->GetXaxis()) ) {
              if (h->GetXaxis()->GetXbins()->GetSize() != 0) fXaxis.Set(h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXbins()->GetArray());
              else                                           fXaxis.Set(h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
            }
            firstHistWithLimits = kFALSE;
         }
#endif

         // this is executed the first time an histogram with limits is found
         // to set some initial values on the new axis
         if (!initialLimitsFound) {
            initialLimitsFound = kTRUE;
            if (h->GetXaxis()->GetXbins()->GetSize() != 0) newXAxis.Set(h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXbins()->GetArray());
            else                                           newXAxis.Set(h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
         }
         else {
            // check first if histograms have same bins
            if (!SameLimitsAndNBins(newXAxis, *(h->GetXaxis())) ) {
               allSameLimits = kFALSE;
               // recompute the limits in this case the optimal limits
               // The condition to works is that the histogram have same bin with
               // and one common bin edge
               if (!RecomputeAxisLimits(newXAxis, *(h->GetXaxis()))) {
                  Error("Merge", "Cannot merge histograms - limits are inconsistent:\n "
                        "first: (%d, %f, %f), second: (%d, %f, %f)",
                        newXAxis.GetNbins(), newXAxis.GetXmin(), newXAxis.GetXmax(),
                        h->GetXaxis()->GetNbins(), h->GetXaxis()->GetXmin(),
                        h->GetXaxis()->GetXmax());
                  return -1;
               }
            }
         }
      }
      if (allHaveLabels) {
         THashList* hlabels=h->GetXaxis()->GetLabels();
         Bool_t haveOneLabel = (hlabels != 0);
         // do here to print message only one time
         if (foundLabelHist && allHaveLabels && !haveOneLabel) {
            Warning("Merge","Not all histograms have labels. I will ignore labels,"
            " falling back to bin numbering mode.");
         }

         allHaveLabels &= (haveOneLabel);
         // for the error message
         if (haveOneLabel) foundLabelHist = kTRUE;
         // I could add a check if histogram contains bins without a label
         // and with non-zero bin content
         // Do we want to support this ???
         // only in case the kCanRebin bit is not set
         if (allHaveLabels && !h->TestBit(TH1::kCanRebin) ) {
            // count number of bins with non-null content
            Int_t non_zero_bins = 0;
            Int_t nbins = h->GetXaxis()->GetNbins();
            if (nbins > hlabels->GetEntries() ) {
               for (Int_t i = 1; i <= nbins; i++) {
                  if (h->GetBinContent(i) != 0 || (fSumw2.fN && h->GetBinError(i) != 0) ) {
                     non_zero_bins++;
                  }
               }
               if (non_zero_bins > hlabels->GetEntries() ) {
                  Warning("Merge","Histogram %s contains non-empty bins without labels - falling back to bin numbering mode",h->GetName() );
                  allHaveLabels = kFALSE;
               }
            }
         }
      }
   }    while ( ( h = dynamic_cast<TH1*> ( next() ) ) != NULL );

   if (!h && (*next) ) {
      Error("Merge","Attempt to merge object of class: %s to a %s",
            (*next)->ClassName(),this->ClassName());
      return -1;
   }


   next.Reset();
   // In the case of histogram with different limits
   // newXAxis will now have the new found limits
   // but one needs first to clone this histogram to perform the merge
   // The clone is not needed when all histograms have the same limits
   TH1 * hclone = 0;
   if (!allSameLimits) {
      // We don't want to add the clone to gDirectory,
      // so remove our kMustCleanup bit temporarily
      Bool_t mustCleanup = TestBit(kMustCleanup);
      if (mustCleanup) ResetBit(kMustCleanup);
      hclone = (TH1*)IsA()->New();
      hclone->SetDirectory(0);
      Copy(*hclone);
      if (mustCleanup) SetBit(kMustCleanup);
      BufferEmpty(1);         // To remove buffer.
      Reset();                // BufferEmpty sets limits so we can't use it later.
      SetEntries(0);
      inlist.AddFirst(hclone);
   }

   // set the binning and cell content on the histogram to merge when the histograms do not have the same binning 
   // and when one of the histogram does not have limits
   if (initialLimitsFound && (!allSameLimits || !allHaveLimits )) {
     if (newXAxis.GetXbins()->GetSize() != 0) SetBins(newXAxis.GetNbins(), newXAxis.GetXbins()->GetArray());
     else                                     SetBins(newXAxis.GetNbins(), newXAxis.GetXmin(), newXAxis.GetXmax());
   }

   // std::cout << "Merging on histogram " << GetName() << std::endl;
   // std::cout << "Merging flags : allHaveLimits - allHaveLabels - initialLimitsFound - allSameLimits " << std::endl;
   // std::cout << "                 " << allHaveLimits << "\t\t" << allHaveLabels << "\t\t" <<  initialLimitsFound << "\t\t" <<  allSameLimits << std::endl;


   if (!allHaveLimits && !allHaveLabels) {
      // fill this histogram with all the data from buffers of histograms without limits
      while (TH1* hist = (TH1*)next()) {
         // support also case where some histogram have limits and some have the buffer
         if ( (hist->GetXaxis()->GetXmin() >= hist->GetXaxis()->GetXmax() ) && hist->fBuffer  ) {
            // no limits
            Int_t nbentries = (Int_t)hist->fBuffer[0];
            for (Int_t i = 0; i < nbentries; i++)
               Fill(hist->fBuffer[2*i + 2], hist->fBuffer[2*i + 1]);
            // Entries from buffers have to be filled one by one
            // because FillN doesn't resize histograms.
         }
      }

      // all histograms have been processed
      if (!initialLimitsFound ) {
         // here the case where all histograms don't have limits
         // In principle I should not have copied in hclone since
         // when initialLimitsFound = false then allSameLimits should be  true
         if (hclone) {
            inlist.Remove(hclone);
            delete hclone;
         }
         return (Long64_t) GetEntries();
      }

      // In case some of the histograms do not have limits 
      // I need to remove the buffer 
      if (fBuffer) BufferEmpty(1); 

      next.Reset();
   }

   //merge bin contents and errors
   // in case when histogram have limits

   Double_t stats[kNstat], totstats[kNstat];
   for (Int_t i=0;i<kNstat;i++) {totstats[i] = stats[i] = 0;}
   GetStats(totstats);
   Double_t nentries = GetEntries();
   Bool_t canRebin=TestBit(kCanRebin);
   // reset, otherwise setting the under/overflow will rebin and make a mess
   if (!allHaveLabels) ResetBit(kCanRebin);
   while (TH1* hist=(TH1*)next()) {
      // process only if the histogram has limits; otherwise it was processed before      
      // in the case of an existing buffer (see if statement just before)

      //std::cout << "merging histogram " << GetName() << " with " << hist->GetName() << std::endl;

      // skip empty histograms 
      Double_t histEntries = hist->GetEntries();
      if (hist->fTsumw == 0 && histEntries == 0) continue;


      // merge for labels or histogram with limits 
      if (allHaveLabels || (hist->GetXaxis()->GetXmin() < hist->GetXaxis()->GetXmax()) ) {
         // import statistics
         hist->GetStats(stats);
         for (Int_t i=0;i<kNstat;i++)
            totstats[i] += stats[i];
         nentries += histEntries;


         Int_t nx = hist->GetXaxis()->GetNbins();
         // loop on bins of the histogram and do the merge
         for (Int_t binx = 0; binx <= nx + 1; binx++) {
            Double_t cu = hist->GetBinContent(binx);
            Double_t error1 = 0;
            Int_t ix = -1;
            if (fSumw2.fN) error1= hist->GetBinError(binx);
            // do only for bins with non null bin content or non-null errors (if Sumw2)
            if (TMath::Abs(cu) > 0 || (fSumw2.fN && error1 > 0 ) ) {
               // case  of overflow bins
               // they do not make sense also in the case of labels
               if (!allHaveLabels) {
                  // case of bins without labels
                  if (!allSameLimits)  {
                     if ( binx==0 || binx== nx+1) {
                        Error("Merge", "Cannot merge histograms - the histograms have"
                              " different limits and undeflows/overflows are present."
                              " The initial histogram is now broken!");
                        return -1;
                     }
                     // NOTE: in the case of one of the histogram  as labels - it is treated as
                     // an error and it has been flagged before
                     // since calling FindBin(x) for histo with labels does not make sense
                     // and the result is unpredictable
                     ix = fXaxis.FindBin(hist->GetXaxis()->GetBinCenter(binx));
                  }
                  else {
                     // histogram have same limits - no need to call FindBin
                     ix = binx;
                  }
               } else {
                  // here only in the case of bins with labels
                  const char* label=hist->GetXaxis()->GetBinLabel(binx);
                  // do we need to support case when there are bins with labels and bins without them ??
                  // NO -then return an error
                  if (label == 0 ) {
                     Fatal("Merge","Histogram %s with labels has NULL label pointer for bin %d",
                           hist->GetName(),binx );
                     return -1;
                  }
                  if (label[0] == 0 ) { // case label is "" , i.e. is not set
                     // exclude underflow which could contain the non-existing labels
                     // thsi we could merge in all underflow
                     if ( binx > 0 && binx <= nx) {
                        Error("Merge","Cannot merge ! Label histogram %s contains a bin %d which has not a label and has non-zero content ",hist->GetName(),binx );
                        return -1;
                     }
                     else
                        // case of underflow/overflow
                        ix = binx;
                  }
                  else {
                     // if bin does not exists FindBin will add it automatically
                     // by calling LabelsInflate() if the bit is set
                     // otherwise it will return zero and bin will be merged in underflow/overflow
                     // Do we want to keep this case ??
                     ix = fXaxis.FindBin(label);
                  }
                  // ix cannot be -1 . Can be 0 in case label is not found and bit is not set
                  if (ix <0) {
                     Fatal("Merge","Error return from TAxis::FindBin for label %s",label);
                     return -1;
                  }
               }
               if (ix >= 0) {
                  // MERGE here the bin contents
                  //std::cout << "merging bin " << binx << " into " << ix << " with bin content " << cu << " bin center x = " << GetBinCenter(ix) << std::endl;
                  if (ix > fNcells )
                     Fatal("Merge","Fatal error merging histogram %s - bin number is %d and array size is %d",GetName(), ix,fNcells); 

                  AddBinContent(ix,cu);
                  if (fSumw2.fN)  fSumw2.fArray[ix] += error1*error1;
               }
            }
         }
      }
   }
   if (canRebin) SetBit(kCanRebin);


   //copy merged stats
   PutStats(totstats);
   SetEntries(nentries);
   if (hclone) {
      inlist.Remove(hclone);
      delete hclone;
   }
   return (Long64_t)nentries;
}

//______________________________________________________________________________
Bool_t TH1::Multiply(TF1 *f1, Double_t c1)
{
   // Performs the operation: this = this*c1*f1
   // if errors are defined (see TH1::Sumw2), errors are also recalculated.
   //
   // Only bins inside the function range are recomputed.
   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
   // you should call Sumw2 before making this operation.
   // This is particularly important if you fit the histogram after TH1::Multiply
   //
   // The function return kFALSE if the Multiply operation failed

   if (!f1) {
      Error("Add","Attempt to multiply by a non-existing function");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();
   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

   // reset min-maximum
   SetMinimum();
   SetMaximum();

   //    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
   //    would resize the axis limits!
   ResetBit(kCanRebin);

   //   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t cu,w;
   Double_t xx[3];
   Double_t *params = 0;
   f1->InitArgs(xx,params);
   for (binz=0;binz<=nbinsz+1;binz++) {
      xx[2] = fZaxis.GetBinCenter(binz);
      for (biny=0;biny<=nbinsy+1;biny++) {
         xx[1] = fYaxis.GetBinCenter(biny);
         for (binx=0;binx<=nbinsx+1;binx++) {
            xx[0] = fXaxis.GetBinCenter(binx);
            if (!f1->IsInside(xx)) continue;
            TF1::RejectPoint(kFALSE);
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            Double_t error1 = GetBinError(bin);
            cu  = c1*f1->EvalPar(xx);
            if (TF1::RejectedPoint()) continue;
            w = GetBinContent(bin)*cu;
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               fSumw2.fArray[bin] = cu*cu*error1*error1;
            }
         }
      }
   }
   ResetStats();
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TH1::Multiply(const TH1 *h1)
{
   //   -*-*-*-*-*-*-*-*-*Multiply this histogram by h1*-*-*-*-*-*-*-*-*-*-*-*-*
   //                     =============================
   //
   //   this = this*h1
   //
   //   If errors of this are available (TH1::Sumw2), errors are recalculated.
   //   Note that if h1 has Sumw2 set, Sumw2 is automatically called for this
   //   if not already set.
   //
   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
   // you should call Sumw2 before making this operation.
   // This is particularly important if you fit the histogram after TH1::Multiply
   //
   // The function return kFALSE if the Multiply operation failed

   if (!h1) {
      Error("Multiply","Attempt to multiply by a non-existing histogram");
      return kFALSE;
   }

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   try {
      CheckConsistency(this,h1);
   } catch(DifferentNumberOfBins&) {
      Error("Multiply","Attempt to multiply histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different bin limits");
   } catch(DifferentLabels&) {
      Warning("Multiply","Attempt to multiply histograms with different labels");
   }


   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

   //    Create Sumw2 if h1 has Sumw2 set
   if (fSumw2.fN == 0 && h1->GetSumw2N() != 0) Sumw2();

   //   - Reset min-  maximum
   SetMinimum();
   SetMaximum();

   //    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
   //    would resize the axis limits!
   ResetBit(kCanRebin);

   //   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t c0,c1,w;
   for (binz=0;binz<=nbinsz+1;binz++) {
      for (biny=0;biny<=nbinsy+1;biny++) {
         for (binx=0;binx<=nbinsx+1;binx++) {
            bin = GetBin(binx,biny,binz);
            c0  = GetBinContent(bin);
            c1  = h1->GetBinContent(bin);
            w   = c0*c1;
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               Double_t e0 = GetBinError(bin);
               Double_t e1 = h1->GetBinError(bin);
               fSumw2.fArray[bin] = (e0*e0*c1*c1 + e1*e1*c0*c0);
            }
         }
      }
   }
   ResetStats();
   return kTRUE;
}


//______________________________________________________________________________
Bool_t TH1::Multiply(const TH1 *h1, const TH1 *h2, Double_t c1, Double_t c2, Option_t *option)
{
   //   -*-*-*Replace contents of this histogram by multiplication of h1 by h2*-*
   //         ================================================================
   //
   //   this = (c1*h1)*(c2*h2)
   //
   //   If errors of this are available (TH1::Sumw2), errors are recalculated.
   //   Note that if h1 or h2 have Sumw2 set, Sumw2 is automatically called for this
   //   if not already set.
   //
   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
   // you should call Sumw2 before making this operation.
   // This is particularly important if you fit the histogram after TH1::Multiply
   //
   // The function return kFALSE if the Multiply operation failed

   TString opt = option;
   opt.ToLower();
   //   Bool_t binomial = kFALSE;
   //   if (opt.Contains("b")) binomial = kTRUE;
   if (!h1 || !h2) {
      Error("Multiply","Attempt to multiply by a non-existing histogram");
      return kFALSE;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t nbinsx = GetNbinsX();
   Int_t nbinsy = GetNbinsY();
   Int_t nbinsz = GetNbinsZ();

   try {
      CheckConsistency(h1,h2);
      CheckConsistency(this,h1);
   } catch(DifferentNumberOfBins&) {
      Error("Multiply","Attempt to multiply histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different bin limits");
   } catch(DifferentLabels&) {
      Warning("Multiply","Attempt to multiply histograms with different labels");
   }

   if (fDimension < 2) nbinsy = -1;
   if (fDimension < 3) nbinsz = -1;

   //    Create Sumw2 if h1 or h2 have Sumw2 set
   if (fSumw2.fN == 0 && (h1->GetSumw2N() != 0 || h2->GetSumw2N() != 0)) Sumw2();

   //   - Reset min - maximum
   SetMinimum();
   SetMaximum();

   //    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
   //    would resize the axis limits!
   ResetBit(kCanRebin);

   //   - Loop on bins (including underflows/overflows)
   Int_t bin, binx, biny, binz;
   Double_t b1,b2,w,d1,d2;
   d1 = c1*c1;
   d2 = c2*c2;
   for (binz=0;binz<=nbinsz+1;binz++) {
      for (biny=0;biny<=nbinsy+1;biny++) {
         for (binx=0;binx<=nbinsx+1;binx++) {
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
            b1  = h1->GetBinContent(bin);
            b2  = h2->GetBinContent(bin);
            w   = (c1*b1)*(c2*b2);
            SetBinContent(bin,w);
            if (fSumw2.fN) {
               Double_t e1 = h1->GetBinError(bin);
               Double_t e2 = h2->GetBinError(bin);
               fSumw2.fArray[bin] = d1*d2*(e1*e1*b2*b2 + e2*e2*b1*b1);
            }
         }
      }
   }
   ResetStats();
   return kTRUE;
}

//______________________________________________________________________________
void TH1::Paint(Option_t *option)
{
   //   -*-*-*-*-*-*-*Control routine to paint any kind of histograms*-*-*-*-*-*-*
   //                 ===============================================
   //
   //  This function is automatically called by TCanvas::Update.
   //  (see TH1::Draw for the list of options)

   GetPainter(option);

   if (fPainter) {
      if (strlen(option) > 0) fPainter->Paint(option);
      else                    fPainter->Paint(fOption.Data());
   }
}

//______________________________________________________________________________
TH1 *TH1::Rebin(Int_t ngroup, const char*newname, const Double_t *xbins)
{
   //   Rebin this histogram
   //
   //  -case 1  xbins=0
   //   If newname is blank (default), the current histogram is modified and
   //   a pointer to it is returned.
   //
   //   If newname is not blank, the current histogram is not modified, and a
   //   new histogram is returned which is a Clone of the current histogram
   //   with its name set to newname.
   //
   //   The parameter ngroup indicates how many bins of this have to be merged
   //   into one bin of the result.
   //
   //   If the original histogram has errors stored (via Sumw2), the resulting
   //   histograms has new errors correctly calculated.
   //
   //   examples: if h1 is an existing TH1F histogram with 100 bins
   //     h1->Rebin();  //merges two bins in one in h1: previous contents of h1 are lost
   //     h1->Rebin(5); //merges five bins in one in h1
   //     TH1F *hnew = h1->Rebin(5,"hnew"); // creates a new histogram hnew
   //                                       // merging 5 bins of h1 in one bin
   //
   //   NOTE:  If ngroup is not an exact divider of the number of bins,
   //          the top limit of the rebinned histogram is reduced
   //          to the upper edge of the last bin that can make a complete
   //          group. The remaining bins are added to the overflow bin.
   //          Statistics will be recomputed from the new bin contents.
   //
   //  -case 2  xbins!=0
   //   A new histogram is created (you should specify newname).
   //   The parameter ngroup is the number of variable size bins in the created histogram.
   //   The array xbins must contain ngroup+1 elements that represent the low-edges
   //   of the bins.
   //   If the original histogram has errors stored (via Sumw2), the resulting
   //   histograms has new errors correctly calculated.
   //
   //   NOTE:  The bin edges specified in xbins should correspond to bin edges
   //          in the original histogram. If a bin edge in the new histogram is
   //          in the middle of a bin in the original histogram, all entries in
   //          the split bin in the original histogram will be transfered to the
   //          lower of the two possible bins in the new histogram. This is
   //          probably not what you want.
   //
   //   examples: if h1 is an existing TH1F histogram with 100 bins
   //     Double_t xbins[25] = {...} array of low-edges (xbins[25] is the upper edge of last bin
   //     h1->Rebin(24,"hnew",xbins);  //creates a new variable bin size histogram hnew

   Int_t nbins    = fXaxis.GetNbins();
   Double_t xmin  = fXaxis.GetXmin();
   Double_t xmax  = fXaxis.GetXmax();
   if ((ngroup <= 0) || (ngroup > nbins)) {
      Error("Rebin", "Illegal value of ngroup=%d",ngroup);
      return 0;
   }

   if (fDimension > 1 || InheritsFrom(TProfile::Class())) {
      Error("Rebin", "Operation valid on 1-D histograms only");
      return 0;
   }
   if (!newname && xbins) {
      Error("Rebin","if xbins is specified, newname must be given");
      return 0;
   }

   Int_t newbins = nbins/ngroup;
   if (!xbins) {
      Int_t nbg = nbins/ngroup;
      if (nbg*ngroup != nbins) {
         Warning("Rebin", "ngroup=%d is not an exact divider of nbins=%d.",ngroup,nbins);
      }
   }
   else {
   // in the case that xbins is given (rebinning in variable bins), ngroup is
   // the new number of bins and number of grouped bins is not constant.
   // when looping for setting the contents for the new histogram we
   // need to loop on all bins of original histogram.  Then set ngroup=nbins
      newbins = ngroup;
      ngroup = nbins;
   }

   // Save old bin contents into a new array
   Double_t entries = fEntries;
   Double_t *oldBins = new Double_t[nbins+2];
   Int_t bin, i;
   for (bin=0;bin<nbins+2;bin++) oldBins[bin] = GetBinContent(bin);
   Double_t *oldErrors = 0;
   if (fSumw2.fN != 0) {
      oldErrors = new Double_t[nbins+2];
      for (bin=0;bin<nbins+2;bin++) oldErrors[bin] = GetBinError(bin);
   }
   // rebin will not include underflow/overflow if new axis range is larger than old axis range 
   if (xbins) {  
      if (xbins[0] < fXaxis.GetXmin() && oldBins[0] != 0 )
         Warning("Rebin","underflow entries will not be used when rebinning");
      if (xbins[newbins] > fXaxis.GetXmax() && oldBins[nbins+1] != 0 )
         Warning("Rebin","overflow entries will not be used when rebinning");
   }


   // create a clone of the old histogram if newname is specified
   TH1 *hnew = this;
   if ((newname && strlen(newname) > 0) || xbins) {
      hnew = (TH1*)Clone(newname);
   }

   //reset kCanRebin bit to avoid a rebinning in SetBinContent
   Int_t bitRebin = hnew->TestBit(kCanRebin);
   hnew->SetBit(kCanRebin,0);

   // save original statistics
   Double_t stat[kNstat];
   GetStats(stat);
   bool resetStat = false;
   // change axis specs and rebuild bin contents array::RebinAx
   if(!xbins && (newbins*ngroup != nbins)) {
      xmax = fXaxis.GetBinUpEdge(newbins*ngroup);
      resetStat = true; //stats must be reset because top bins will be moved to overflow bin
   }
   // save the TAttAxis members (reset by SetBins)
   Int_t    nDivisions  = fXaxis.GetNdivisions();
   Color_t  axisColor   = fXaxis.GetAxisColor();
   Color_t  labelColor  = fXaxis.GetLabelColor();
   Style_t  labelFont   = fXaxis.GetLabelFont();
   Float_t  labelOffset = fXaxis.GetLabelOffset();
   Float_t  labelSize   = fXaxis.GetLabelSize();
   Float_t  tickLength  = fXaxis.GetTickLength();
   Float_t  titleOffset = fXaxis.GetTitleOffset();
   Float_t  titleSize   = fXaxis.GetTitleSize();
   Color_t  titleColor  = fXaxis.GetTitleColor();
   Style_t  titleFont   = fXaxis.GetTitleFont();

   if(!xbins && (fXaxis.GetXbins()->GetSize() > 0)){ // variable bin sizes
      Double_t *bins = new Double_t[newbins+1];
      for(i = 0; i <= newbins; ++i) bins[i] = fXaxis.GetBinLowEdge(1+i*ngroup);
      hnew->SetBins(newbins,bins); //this also changes errors array (if any)
      delete [] bins;
   } else if (xbins) {
      hnew->SetBins(newbins,xbins);
   } else {
      hnew->SetBins(newbins,xmin,xmax);
   }

   // Restore axis attributes
   fXaxis.SetNdivisions(nDivisions);
   fXaxis.SetAxisColor(axisColor);
   fXaxis.SetLabelColor(labelColor);
   fXaxis.SetLabelFont(labelFont);
   fXaxis.SetLabelOffset(labelOffset);
   fXaxis.SetLabelSize(labelSize);
   fXaxis.SetTickLength(tickLength);
   fXaxis.SetTitleOffset(titleOffset);
   fXaxis.SetTitleSize(titleSize);
   fXaxis.SetTitleColor(titleColor);
   fXaxis.SetTitleFont(titleFont);

   // copy merged bin contents (ignore under/overflows)
   // Start merging only once the new lowest edge is reached
   Int_t startbin = 1;
   const Double_t newxmin = hnew->GetXaxis()->GetBinLowEdge(1);
   while( fXaxis.GetBinCenter(startbin) < newxmin && startbin <= nbins ) {
      startbin++;
   }
   Int_t oldbin = startbin;
   Double_t binContent, binError;
   for (bin = 1;bin<=newbins;bin++) {
      binContent = 0;
      binError   = 0;
      Int_t imax = ngroup;
      Double_t xbinmax = hnew->GetXaxis()->GetBinUpEdge(bin);
      for (i=0;i<ngroup;i++) {
         if( (oldbin+i > nbins) ||
             ( hnew != this && (fXaxis.GetBinCenter(oldbin+i) > xbinmax)) ) {
            imax = i;
            break;
         }
         binContent += oldBins[oldbin+i];
         if (oldErrors) binError += oldErrors[oldbin+i]*oldErrors[oldbin+i];
      }
      hnew->SetBinContent(bin,binContent);
      if (oldErrors) hnew->SetBinError(bin,TMath::Sqrt(binError));
      oldbin += imax;
   }

   // sum underflow and overflow contents until startbin
   binContent = 0;
   binError = 0;
   for (i = 0; i < startbin; ++i)  {
      binContent += oldBins[i];
      if (oldErrors) binError += oldErrors[i]*oldErrors[i];
   }
   hnew->SetBinContent(0,binContent);
   if (oldErrors) hnew->SetBinError(0,TMath::Sqrt(binError));
   // sum overflow
   binContent = 0;
   binError = 0;
   for (i = oldbin; i <= nbins+1; ++i)  {
      binContent += oldBins[i];
      if (oldErrors) binError += oldErrors[i]*oldErrors[i];
   }
   hnew->SetBinContent(newbins+1,binContent);
   if (oldErrors) hnew->SetBinError(newbins+1,TMath::Sqrt(binError));
   hnew->SetBit(kCanRebin,bitRebin);

   // restore statistics and entries modified by SetBinContent
   hnew->SetEntries(entries);
   if (!resetStat) hnew->PutStats(stat);
   delete [] oldBins;
   if (oldErrors) delete [] oldErrors;
   return hnew;
}

//______________________________________________________________________________
Bool_t TH1::FindNewAxisLimits(const TAxis* axis, const Double_t point, Double_t& newMin, Double_t &newMax)
{
   // finds new limits for the axis so that *point* is within the range and
   // the limits are compatible with the previous ones (see TH1::Merge).
   // new limits are put into *newMin* and *newMax* variables.
   // axis - axis whose limits are to be recomputed
   // point - point that should fit within the new axis limits
   // newMin - new minimum will be stored here
   // newMax - new maximum will be stored here.
   // false if failed (e.g. if the initial axis limits are wrong
   // or the new range is more than 2^64 times the old one).

   Double_t xmin = axis->GetXmin();
   Double_t xmax = axis->GetXmax();
   if (xmin >= xmax) return kFALSE;
   Double_t range = xmax-xmin;
   Double_t binsize = range / axis->GetNbins();

   //recompute new axis limits by doubling the current range
   Int_t ntimes = 0;
   while (point < xmin) {
      if (ntimes++ > 64)
         return kFALSE;
      xmin = xmin - range;
      range *= 2;
      binsize *= 2;
      // // make sure that the merging will be correct
      // if ( xmin / binsize - TMath::Floor(xmin / binsize) >= 0.5) {
      //    xmin += 0.5 * binsize;
      //    xmax += 0.5 * binsize;  // won't work with a histogram with only one bin, but I don't care
      // }
   }
   while (point >= xmax) {
      if (ntimes++ > 64)
         return kFALSE;
      xmax = xmax + range;
      range *= 2;
      binsize *= 2;
      // // make sure that the merging will be correct
      // if ( xmin / binsize - TMath::Floor(xmin / binsize) >= 0.5) {
      //    xmin -= 0.5 * binsize;
      //    xmax -= 0.5 * binsize;  // won't work with a histogram with only one bin, but I don't care
      // }
   }
   newMin = xmin;
   newMax = xmax;
   //   Info("FindNewAxisLimits", "OldAxis: (%lf, %lf), new: (%lf, %lf), point: %lf",
   //      axis->GetXmin(), axis->GetXmax(), xmin, xmax, point);

   return kTRUE;
}

//______________________________________________________________________________
void TH1::RebinAxis(Double_t x, TAxis *axis)
{
   // Histogram is resized along axis such that x is in the axis range.
   // The new axis limits are recomputed by doubling iteratively
   // the current axis range until the specified value x is within the limits.
   // The algorithm makes a copy of the histogram, then loops on all bins
   // of the old histogram to fill the rebinned histogram.
   // Takes into account errors (Sumw2) if any.
   // The algorithm works for 1-d, 2-D and 3-D histograms.
   // The bit kCanRebin must be set before invoking this function.
   //  Ex:  h->SetBit(TH1::kCanRebin);

   if (!TestBit(kCanRebin)) return;
   if (TMath::IsNaN(x)) {         // x may be a NaN
      ResetBit(kCanRebin);
      return;
   }

   if (axis->GetXmin() >= axis->GetXmax()) return;
   if (axis->GetNbins() <= 0) return;

   Double_t xmin, xmax;
   if (!FindNewAxisLimits(axis, x, xmin, xmax))
      return;

   //save a copy of this histogram
   TH1 *hold = (TH1*)IsA()->New(); 
   hold->SetDirectory(0);
   Copy(*hold);
   //set new axis limits
   axis->SetLimits(xmin,xmax);

   Int_t  nbinsx = fXaxis.GetNbins();
   Int_t  nbinsy = fYaxis.GetNbins();
   Int_t  nbinsz = fZaxis.GetNbins();

   //now loop on all bins and refill
   Double_t err,cu;
   Double_t bx,by,bz;
   Int_t errors = GetSumw2N();
   Int_t ix,iy,iz,ibin,binx,biny,binz,bin;
   Reset("ICE"); //reset only Integral, contents and Errors
   for (binz=1;binz<=nbinsz;binz++) {
      bz  = hold->GetZaxis()->GetBinCenter(binz);
      iz  = fZaxis.FindFixBin(bz);
      for (biny=1;biny<=nbinsy;biny++) {
         by  = hold->GetYaxis()->GetBinCenter(biny);
         iy  = fYaxis.FindFixBin(by);
         for (binx=1;binx<=nbinsx;binx++) {
            bx = hold->GetXaxis()->GetBinCenter(binx);
            ix  = fXaxis.FindFixBin(bx);
            bin = hold->GetBin(binx,biny,binz);
            ibin= GetBin(ix,iy,iz);
            cu  = hold->GetBinContent(bin);
            AddBinContent(ibin,cu);
            if (errors) {
               err = hold->GetBinError(bin);
               fSumw2.fArray[ibin] += err*err;
            }
         }
      }
   }
   delete hold;
}

//______________________________________________________________________________
void TH1::RecursiveRemove(TObject *obj)
{
   // Recursively remove object from the list of functions

   if (fFunctions) {
      if (!fFunctions->TestBit(kInvalidObject)) fFunctions->RecursiveRemove(obj);
   }
}

//______________________________________________________________________________
void TH1::Scale(Double_t c1, Option_t *option)
{
   //   -*-*-*Multiply this histogram by a constant c1*-*-*-*-*-*-*-*-*
   //         ========================================
   //
   //   this = c1*this
   //
   // Note that both contents and errors(if any) are scaled.
   // This function uses the services of TH1::Add
   //
   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
   // you should call Sumw2 before making this operation.
   // This is particularly important if you fit the histogram after TH1::Scale
   //
   // One can scale an histogram such that the bins integral is equal to
   // the normalization parameter via TH1::Scale(Double_t norm), where norm
   // is the desired normalization divided by the integral of the histogram.
   //
   // If option contains "width" the bin contents and errors are divided
   // by the bin width.

   TString opt = option;
   opt.ToLower();
   Double_t ent = fEntries;
   if (opt.Contains("width")) Add(this,this,c1,-1);
   else                       Add(this,this,c1,0);
   fEntries = ent;

   //if contours set, must also scale contours
   Int_t ncontours = GetContour();
   if (ncontours == 0) return;
   Double_t *levels = fContour.GetArray();
   for (Int_t i=0;i<ncontours;i++) {
      levels[i] *= c1;
   }
}


//______________________________________________________________________________
void TH1::SetDefaultBufferSize(Int_t buffersize)
{
   // static function to set the default buffer size for automatic histograms.
   // When an histogram is created with one of its axis lower limit greater
   // or equal to its upper limit, the function SetBuffer is automatically
   // called with the default buffer size.

   if (buffersize < 0) buffersize = 0;
   fgBufferSize = buffersize;
}


//______________________________________________________________________________
void TH1::SetDefaultSumw2(Bool_t sumw2)
{
   // static function.
   // When this static function is called with sumw2=kTRUE, all new
   // histograms will automatically activate the storage
   // of the sum of squares of errors, ie TH1::Sumw2 is automatically called.

   fgDefaultSumw2 = sumw2;
}

//______________________________________________________________________________
void TH1::SetTitle(const char *title)
{
   // Change (i.e. set) the title
   //
   //   if title is in the form "stringt;stringx;stringy;stringz"
   //   the histogram title is set to stringt, the x axis title to stringx,
   //   the y axis title to stringy, and the z axis title to stringz.
   //   To insert the character ";" in one of the titles, one should use "#;"
   //   or "#semicolon".

   fTitle = title;
   fTitle.ReplaceAll("#;",2,"#semicolon",10);

   // Decode fTitle. It may contain X, Y and Z titles
   TString str1 = fTitle, str2;
   Int_t isc = str1.Index(";");
   Int_t lns = str1.Length();

   if (isc >=0 ) {
      fTitle = str1(0,isc);
      str1   = str1(isc+1, lns);
      isc    = str1.Index(";");
      if (isc >=0 ) {
         str2 = str1(0,isc);
         str2.ReplaceAll("#semicolon",10,";",1);
         fXaxis.SetTitle(str2.Data());
         lns  = str1.Length();
         str1 = str1(isc+1, lns);
         isc  = str1.Index(";");
         if (isc >=0 ) {
            str2 = str1(0,isc);
            str2.ReplaceAll("#semicolon",10,";",1);
            fYaxis.SetTitle(str2.Data());
            lns  = str1.Length();
            str1 = str1(isc+1, lns);
            str1.ReplaceAll("#semicolon",10,";",1);
            fZaxis.SetTitle(str1.Data());
         } else {
            str1.ReplaceAll("#semicolon",10,";",1);
            fYaxis.SetTitle(str1.Data());
         }
      } else {
         str1.ReplaceAll("#semicolon",10,";",1);
         fXaxis.SetTitle(str1.Data());
      }
   }

   fTitle.ReplaceAll("#semicolon",10,";",1);

   if (gPad && TestBit(kMustCleanup)) gPad->Modified();
}

// -------------------------------------------------------------------------
void  TH1::SmoothArray(Int_t nn, Double_t *xx, Int_t ntimes)
{
   // smooth array xx, translation of Hbook routine hsmoof.F
   // based on algorithm 353QH twice presented by J. Friedman
   // in Proc.of the 1974 CERN School of Computing, Norway, 11-24 August, 1974.

   if (nn < 3 ) {
      ::Error("SmoothArray","Need at least 3 points for smoothing: n = %d",nn);
      return;
   }

   Int_t ii;
   Double_t hh[6] = {0,0,0,0,0,0};

   std::vector<double> yy(nn); 
   std::vector<double> zz(nn); 
   std::vector<double> rr(nn); 

   for (Int_t pass=0;pass<ntimes;pass++) {
      // first copy original data into temp array
      std::copy(xx, xx+nn, zz.begin() ); 



      for (int noent = 0; noent < 2; ++noent) { // run algorithm two times 
      
         //  do 353 i.e. running median 3, 5, and 3 in a single loop
         for  (int kk = 0; kk < 3; kk++)  {
            std::copy(zz.begin(), zz.end(), yy.begin());
            int medianType = (kk != 1)  ?  3 : 5; 
            int ifirst      = (kk != 1 ) ?  1 : 2;
            int ilast       = (kk != 1 ) ? nn-1 : nn -2; 
            //nn2 = nn - ik - 1;
            // do all elements beside the first and last point for median 3
            //  and first two and last 2 for median 5
            for  ( ii = ifirst; ii < ilast; ii++)  {
               assert(ii - ifirst >= 0);
               for  (int jj = 0; jj < medianType; jj++)   {
                  hh[jj] = yy[ii - ifirst + jj ];
               }
               zz[ii] = TMath::Median(medianType, hh);
            }

            if  (kk == 0)  {   // first median 3
               // first point
               hh[0] = zz[1];
               hh[1] = zz[0];
               hh[2] = 3*zz[1] - 2*zz[2];
               zz[0] = TMath::Median(3, hh);
               // last point
               hh[0] = zz[nn - 2];
               hh[1] = zz[nn - 1];
               hh[2] = 3*zz[nn - 2] - 2*zz[nn - 3];
               zz[nn - 1] = TMath::Median(3, hh);
            }


            if  (kk == 1)  {   //  median 5
               for  (ii = 0; ii < 3; ii++) {
                  hh[ii] = yy[ii];
               }
               zz[1] = TMath::Median(3, hh);
               // last two points
               for  (ii = 0; ii < 3; ii++) {
                  hh[ii] = yy[nn - 3 + ii];
               }
               zz[nn - 2] = TMath::Median(3, hh);
            }

         }

         std::copy ( zz.begin(), zz.end(), yy.begin() );

         // quadratic interpolation for flat segments
         for (ii = 2; ii < (nn - 2); ii++) {
            if  (zz[ii - 1] != zz[ii]) continue;
            if  (zz[ii] != zz[ii + 1]) continue;
            hh[0] = zz[ii - 2] - zz[ii];
            hh[1] = zz[ii + 2] - zz[ii];
            if  (hh[0] * hh[1] <= 0) continue;
            int jk = 1;
            if  ( TMath::Abs(hh[1]) > TMath::Abs(hh[0]) ) jk = -1;
            yy[ii] = -0.5*zz[ii - 2*jk] + zz[ii]/0.75 + zz[ii + 2*jk] /6.;
            yy[ii + jk] = 0.5*(zz[ii + 2*jk] - zz[ii - 2*jk]) + zz[ii];
         }

         // running means
         //std::copy(zz.begin(), zz.end(), yy.begin()); 
         for  (ii = 1; ii < nn - 1; ii++) {
            zz[ii] = 0.25*yy[ii - 1] + 0.5*yy[ii] + 0.25*yy[ii + 1];
         }
         zz[0] = yy[0];
         zz[nn - 1] = yy[nn - 1];

         if (noent == 0) { 

            // save computed values 
            std::copy(zz.begin(), zz.end(), rr.begin()); 

            // COMPUTE  residuals
            for  (ii = 0; ii < nn; ii++)  {
               zz[ii] = xx[ii] - zz[ii];
            }
         }

      }  // end loop on noent
      
 
      double xmin = TMath::MinElement(nn,xx);
      for  (ii = 0; ii < nn; ii++) {
         if (xmin < 0) xx[ii] = rr[ii] + zz[ii];
         // make smoothing defined positive - not better using 0 ?
         else  xx[ii] = TMath::Max((rr[ii] + zz[ii]),0.0 );
      }
   }
}


// ------------------------------------------------------------------------
void  TH1::Smooth(Int_t ntimes, Option_t *option)
{
   // Smooth bin contents of this histogram.
   // if option contains "R" smoothing is applied only to the bins
   // defined in the X axis range (default is to smooth all bins)
   // Bin contents are replaced by their smooth values.
   // Errors (if any) are not modified.
   // the smoothing procedure is repeated ntimes (default=1)

   if (fDimension != 1) {
      Error("Smooth","Smooth only supported for 1-d histograms");
      return;
   }
   Int_t nbins = fXaxis.GetNbins();
   if (nbins < 3) { 
      Error("Smooth","Smooth only supported for histograms with >= 3 bins. Nbins = %d",nbins);
      return;
   }

   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);

   Int_t firstbin = 1, lastbin = nbins;
   TString opt = option;
   opt.ToLower();
   if (opt.Contains("r")) {
      firstbin= fXaxis.GetFirst();
      lastbin  = fXaxis.GetLast();
   }
   nbins = lastbin - firstbin + 1;
   Double_t *xx = new Double_t[nbins];
   Double_t nent = fEntries;
   Int_t i;
   for (i=0;i<nbins;i++) {
      xx[i] = GetBinContent(i+firstbin);
   }

   TH1::SmoothArray(nbins,xx,ntimes);

   for (i=0;i<nbins;i++) {
      SetBinContent(i+firstbin,xx[i]);
   }
   fEntries = nent;
   delete [] xx;

   if (gPad) gPad->Modified();
}


// ------------------------------------------------------------------------
void  TH1::StatOverflows(Bool_t flag)
{
   //  if flag=kTRUE, underflows and overflows are used by the Fill functions
   //  in the computation of statistics (mean value, RMS).
   //  By default, underflows or overflows are not used.

   fgStatOverflows = flag;
}

//_______________________________________________________________________
void TH1::Streamer(TBuffer &b)
{
   //   -*-*-*-*-*-*-*Stream a class object*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //                 =====================
   if (b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = b.ReadVersion(&R__s, &R__c);
      if (fDirectory) fDirectory->Remove(this);
      fDirectory = 0;
      if (R__v > 2) {
         b.ReadClassBuffer(TH1::Class(), this, R__v, R__s, R__c);

         ResetBit(kMustCleanup);
         fXaxis.SetParent(this);
         fYaxis.SetParent(this);
         fZaxis.SetParent(this);
         TIter next(fFunctions);
         TObject *obj;
         while ((obj=next())) {
            if (obj->InheritsFrom(TF1::Class())) ((TF1*)obj)->SetParent(this);
         }
         return;
      }
      //process old versions before automatic schema evolution
      TNamed::Streamer(b);
      TAttLine::Streamer(b);
      TAttFill::Streamer(b);
      TAttMarker::Streamer(b);
      b >> fNcells;
      fXaxis.Streamer(b);
      fYaxis.Streamer(b);
      fZaxis.Streamer(b);
      fXaxis.SetParent(this);
      fYaxis.SetParent(this);
      fZaxis.SetParent(this);
      b >> fBarOffset;
      b >> fBarWidth;
      b >> fEntries;
      b >> fTsumw;
      b >> fTsumw2;
      b >> fTsumwx;
      b >> fTsumwx2;
      if (R__v < 2) {
         Float_t maximum, minimum, norm;
         Float_t *contour=0;
         b >> maximum; fMaximum = maximum;
         b >> minimum; fMinimum = minimum;
         b >> norm;    fNormFactor = norm;
         Int_t n = b.ReadArray(contour);
         fContour.Set(n);
         for (Int_t i=0;i<n;i++) fContour.fArray[i] = contour[i];
         delete [] contour;
      } else {
         b >> fMaximum;
         b >> fMinimum;
         b >> fNormFactor;
         fContour.Streamer(b);
      }
      fSumw2.Streamer(b);
      fOption.Streamer(b);
      fFunctions->Delete();
      fFunctions->Streamer(b);
      b.CheckByteCount(R__s, R__c, TH1::IsA());

   } else {
      b.WriteClassBuffer(TH1::Class(),this);
   }
}

//______________________________________________________________________________
void TH1::Print(Option_t *option) const
{
   //   -*-*-*-*-*Print some global quantities for this histogram*-*-*-*-*-*-*-*
   //             ===============================================
   //
   //  If option "base" is given, number of bins and ranges are also printed
   //  If option "range" is given, bin contents and errors are also printed
   //                     for all bins in the current range (default 1-->nbins)
   //  If option "all" is given, bin contents and errors are also printed
   //                     for all bins including under and overflows.

   printf( "TH1.Print Name  = %s, Entries= %d, Total sum= %g\n",GetName(),Int_t(GetEntries()),GetSumOfWeights());
   TString opt = option;
   opt.ToLower();
   Int_t all;
   if      (opt.Contains("all"))   all = 0;
   else if (opt.Contains("range")) all = 1;
   else if (opt.Contains("base"))  all = 2;
   else                            return;

   Int_t bin, binx, biny, binz;
   Int_t firstx=0,lastx=0,firsty=0,lasty=0,firstz=0,lastz=0;
   if (all == 0) {
      lastx  = fXaxis.GetNbins()+1;
      if (fDimension > 1) lasty = fYaxis.GetNbins()+1;
      if (fDimension > 2) lastz = fZaxis.GetNbins()+1;
   } else {
      firstx = fXaxis.GetFirst(); lastx  = fXaxis.GetLast();
      if (fDimension > 1) {firsty = fYaxis.GetFirst(); lasty = fYaxis.GetLast();}
      if (fDimension > 2) {firstz = fZaxis.GetFirst(); lastz = fZaxis.GetLast();}
   }

   if (all== 2) {
      printf("          Title = %s\n", GetTitle());
      printf("          NbinsX= %d, xmin= %g, xmax=%g", fXaxis.GetNbins(), fXaxis.GetXmin(), fXaxis.GetXmax());
      if( fDimension > 1) printf(", NbinsY= %d, ymin= %g, ymax=%g", fYaxis.GetNbins(), fYaxis.GetXmin(), fYaxis.GetXmax());
      if( fDimension > 2) printf(", NbinsZ= %d, zmin= %g, zmax=%g", fZaxis.GetNbins(), fZaxis.GetXmin(), fZaxis.GetXmax());
      printf("\n");
      return;
   }

   Double_t w,e;
   Double_t x,y,z;
   if (fDimension == 1) {
      for (binx=firstx;binx<=lastx;binx++) {
         x = fXaxis.GetBinCenter(binx);
         w = GetBinContent(binx);
         e = GetBinError(binx);
         if(fSumw2.fN) printf(" fSumw[%d]=%g, x=%g, error=%g\n",binx,w,x,e);
         else          printf(" fSumw[%d]=%g, x=%g\n",binx,w,x);
      }
   }
   if (fDimension == 2) {
      for (biny=firsty;biny<=lasty;biny++) {
         y = fYaxis.GetBinCenter(biny);
         for (binx=firstx;binx<=lastx;binx++) {
            bin = GetBin(binx,biny);
            x = fXaxis.GetBinCenter(binx);
            w = GetBinContent(bin);
            e = GetBinError(bin);
            if(fSumw2.fN) printf(" fSumw[%d][%d]=%g, x=%g, y=%g, error=%g\n",binx,biny,w,x,y,e);
            else          printf(" fSumw[%d][%d]=%g, x=%g, y=%g\n",binx,biny,w,x,y);
         }
      }
   }
   if (fDimension == 3) {
      for (binz=firstz;binz<=lastz;binz++) {
         z = fZaxis.GetBinCenter(binz);
         for (biny=firsty;biny<=lasty;biny++) {
            y = fYaxis.GetBinCenter(biny);
            for (binx=firstx;binx<=lastx;binx++) {
               bin = GetBin(binx,biny,binz);
               x = fXaxis.GetBinCenter(binx);
               w = GetBinContent(bin);
               e = GetBinError(bin);
               if(fSumw2.fN) printf(" fSumw[%d][%d][%d]=%g, x=%g, y=%g, z=%g, error=%g\n",binx,biny,binz,w,x,y,z,e);
               else          printf(" fSumw[%d][%d][%d]=%g, x=%g, y=%g, z=%g\n",binx,biny,binz,w,x,y,z);
            }
         }
      }
   }
}

//______________________________________________________________________________
void TH1::Rebuild(Option_t *)
{
   // Using the current bin info, recompute the arrays for contents and errors

   SetBinsLength();
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::Reset(Option_t *option)
{
   //   -*-*-*-*-*-*Reset this histogram: contents, errors, etc*-*-*-*-*-*-*-*
   //               ===========================================
   //
   // if option "ICE" is specified, resets only Integral, Contents and Errors.
   // if option "ICES" is specified, resets only Integral, Contents , Errors and Statistics
   //                  This option is used
   // if option "M"   is specified, resets also Minimum and Maximum

   // The option "ICE" is used when rebinning the histogram (in RebinAxis, LabelInflate, etc..)
   // The option "ICES is used in combination with the buffer (see BufferEmpty and BufferFill)

   TString opt = option;
   opt.ToUpper();
   fSumw2.Reset();
   if (fIntegral) {delete [] fIntegral; fIntegral = 0;}

   if (opt.Contains("M")) {
      SetMinimum();
      SetMaximum();
   }

   if (opt.Contains("ICE") && !opt.Contains("S")) return;

   // Setting fBuffer[0] = 0 is like resetting the buffer but not deleting it
   // But what is the sense of calling BufferEmpty() ? For making the axes ?
   // BufferEmpty will update contents that later will be
   // reset in calling TH1D::Reset. For this we need to reset the stats afterwards
   // It may be needed for computing the axis limits....
   if (fBuffer) {BufferEmpty(); fBuffer[0] = 0;}

   // need to reset also the statistics
   // (needs to be done after calling BufferEmpty() )
   fTsumw       = 0;
   fTsumw2      = 0;
   fTsumwx      = 0;
   fTsumwx2     = 0;
   fEntries     = 0;

   if (opt == "ICES") return;


   TObject *stats = fFunctions->FindObject("stats");
   fFunctions->Remove(stats);
   //special logic to support the case where the same object is
   //added multiple times in fFunctions.
   //This case happens when the same object is added with different
   //drawing modes
   TObject *obj;
   while ((obj  = fFunctions->First())) {
      while(fFunctions->Remove(obj)) { }
      delete obj;
   }
   if(stats) fFunctions->Add(stats);
   fContour.Set(0);
}

//______________________________________________________________________________
void TH1::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
   // Save primitive as a C++ statement(s) on output stream out

   Bool_t nonEqiX = kFALSE;
   Bool_t nonEqiY = kFALSE;
   Bool_t nonEqiZ = kFALSE;
   Int_t i;
   static Int_t nxaxis = 0;
   static Int_t nyaxis = 0;
   static Int_t nzaxis = 0;
   TString sxaxis="xAxis",syaxis="yAxis",szaxis="zAxis";

   // Check if the histogram has equidistant X bins or not.  If not, we
   // create an array holding the bins.
   if (GetXaxis()->GetXbins()->fN && GetXaxis()->GetXbins()->fArray) {
      nonEqiX = kTRUE;
      nxaxis++;
      sxaxis += nxaxis;
      out << "   Double_t "<<sxaxis<<"[" << GetXaxis()->GetXbins()->fN
         << "] = {";
      for (i = 0; i < GetXaxis()->GetXbins()->fN; i++) {
         if (i != 0) out << ", ";
         out << GetXaxis()->GetXbins()->fArray[i];
      }
      out << "}; " << endl;
   }
   // If the histogram is 2 or 3 dimensional, check if the histogram
   // has equidistant Y bins or not.  If not, we create an array
   // holding the bins.
   if (fDimension > 1 && GetYaxis()->GetXbins()->fN &&
      GetYaxis()->GetXbins()->fArray) {
         nonEqiY = kTRUE;
         nyaxis++;
         syaxis += nyaxis;
         out << "   Double_t "<<syaxis<<"[" << GetYaxis()->GetXbins()->fN
            << "] = {";
         for (i = 0; i < GetYaxis()->GetXbins()->fN; i++) {
            if (i != 0) out << ", ";
            out << GetYaxis()->GetXbins()->fArray[i];
         }
         out << "}; " << endl;
   }
   // IF the histogram is 3 dimensional, check if the histogram
   // has equidistant Z bins or not.  If not, we create an array
   // holding the bins.
   if (fDimension > 2 && GetZaxis()->GetXbins()->fN &&
      GetZaxis()->GetXbins()->fArray) {
         nonEqiZ = kTRUE;
         nzaxis++;
         szaxis += nzaxis;
         out << "   Double_t "<<szaxis<<"[" << GetZaxis()->GetXbins()->fN
            << "] = {";
         for (i = 0; i < GetZaxis()->GetXbins()->fN; i++) {
            if (i != 0) out << ", ";
            out << GetZaxis()->GetXbins()->fArray[i];
         }
         out << "}; " << endl;
   }

   char quote = '"';
   out <<"   "<<endl;
   out <<"   "<< ClassName() <<" *";

   // Histogram pointer has by default the histogram name.
   // However, in case the histogram has no directory, it is safer to add a incremental suffix.
   // If the histogram belongs to a graph or a stack the suffix is not added because
   // the graph and stack objects are not aware of this new name. Same thing if
   // the histogram is drawn with the option COLZ because the TPaletteAxis drawn
   // when this option is selected, does not know this new name either.
   TString opt = option;
   opt.ToLower();
   static Int_t hcounter = 0;
   TString histName = GetName();
   if (!fDirectory && !histName.Contains("Graph")
                   && !histName.Contains("_stack_")
                   && !opt.Contains("colz")) {
      hcounter++;
      histName += "__";
      histName += hcounter;
   }
   const char *hname = histName.Data();
   if (!strlen(hname)) hname = "unnamed";

   TString t(GetTitle());
   t.ReplaceAll("\\","\\\\");
   t.ReplaceAll("\"","\\\"");
   out << hname << " = new " << ClassName() << "(" << quote
      << hname << quote << "," << quote<< t.Data() << quote
      << "," << GetXaxis()->GetNbins();
   if (nonEqiX)
      out << ", "<<sxaxis;
   else
      out << "," << GetXaxis()->GetXmin()
      << "," << GetXaxis()->GetXmax();
   if (fDimension > 1) {
      out << "," << GetYaxis()->GetNbins();
      if (nonEqiY)
         out << ", "<<syaxis;
      else
         out << "," << GetYaxis()->GetXmin()
         << "," << GetYaxis()->GetXmax();
   }
   if (fDimension > 2) {
      out << "," << GetZaxis()->GetNbins();
      if (nonEqiZ)
         out << ", "<<szaxis;
      else
         out << "," << GetZaxis()->GetXmin()
         << "," << GetZaxis()->GetXmax();
   }
   out << ");" << endl;

   // save bin contents
   Int_t bin;
   for (bin=0;bin<fNcells;bin++) {
      Double_t bc = GetBinContent(bin);
      if (bc) {
         out<<"   "<<hname<<"->SetBinContent("<<bin<<","<<bc<<");"<<endl;
      }
   }

   // save bin errors
   if (fSumw2.fN) {
      for (bin=0;bin<fNcells;bin++) {
         Double_t be = GetBinError(bin);
         if (be) {
            out<<"   "<<hname<<"->SetBinError("<<bin<<","<<be<<");"<<endl;
         }
      }
   }

   TH1::SavePrimitiveHelp(out, hname, option);
}

//______________________________________________________________________________
void TH1::SavePrimitiveHelp(ostream &out, const char *hname, Option_t *option /*= ""*/)
{
   // helper function for the SavePrimitive functions from TH1
   // or classes derived from TH1, eg TProfile, TProfile2D.

   char quote = '"';
   if (TMath::Abs(GetBarOffset()) > 1e-5) {
      out<<"   "<<hname<<"->SetBarOffset("<<GetBarOffset()<<");"<<endl;
   }
   if (TMath::Abs(GetBarWidth()-1) > 1e-5) {
      out<<"   "<<hname<<"->SetBarWidth("<<GetBarWidth()<<");"<<endl;
   }
   if (fMinimum != -1111) {
      out<<"   "<<hname<<"->SetMinimum("<<fMinimum<<");"<<endl;
   }
   if (fMaximum != -1111) {
      out<<"   "<<hname<<"->SetMaximum("<<fMaximum<<");"<<endl;
   }
   if (fNormFactor != 0) {
      out<<"   "<<hname<<"->SetNormFactor("<<fNormFactor<<");"<<endl;
   }
   if (fEntries != 0) {
      out<<"   "<<hname<<"->SetEntries("<<fEntries<<");"<<endl;
   }
   if (fDirectory == 0) {
      out<<"   "<<hname<<"->SetDirectory(0);"<<endl;
   }
   if (TestBit(kNoStats)) {
      out<<"   "<<hname<<"->SetStats(0);"<<endl;
   }
   if (fOption.Length() != 0) {
      out<<"   "<<hname<<"->SetOption("<<quote<<fOption.Data()<<quote<<");"<<endl;
   }

   // save contour levels
   Int_t ncontours = GetContour();
   if (ncontours > 0) {
      out<<"   "<<hname<<"->SetContour("<<ncontours<<");"<<endl;
      Double_t zlevel;
      for (Int_t bin=0;bin<ncontours;bin++) {
         if (gPad->GetLogz()) {
            zlevel = TMath::Power(10,GetContourLevel(bin));
         } else {
            zlevel = GetContourLevel(bin);
         }
         out<<"   "<<hname<<"->SetContourLevel("<<bin<<","<<zlevel<<");"<<endl;
      }
   }

   // save list of functions
   TObjOptLink *lnk = (TObjOptLink*)fFunctions->FirstLink();
   TObject *obj;
   while (lnk) {
      obj = lnk->GetObject();
      obj->SavePrimitive(out,"nodraw");
      if (obj->InheritsFrom(TF1::Class())) {
         out<<"   "<<hname<<"->GetListOfFunctions()->Add("<<obj->GetName()<<");"<<endl;
      } else if (obj->InheritsFrom("TPaveStats")) {
         out<<"   "<<hname<<"->GetListOfFunctions()->Add(ptstats);"<<endl;
         out<<"   ptstats->SetParent("<<hname<<");"<<endl;
      } else {
         out<<"   "<<hname<<"->GetListOfFunctions()->Add("<<obj->GetName()<<","<<quote<<lnk->GetOption()<<quote<<");"<<endl;
      }
      lnk = (TObjOptLink*)lnk->Next();
   }

   // save attributes
   SaveFillAttributes(out,hname,0,1001);
   SaveLineAttributes(out,hname,1,1,1);
   SaveMarkerAttributes(out,hname,1,1,1);
   fXaxis.SaveAttributes(out,hname,"->GetXaxis()");
   fYaxis.SaveAttributes(out,hname,"->GetYaxis()");
   fZaxis.SaveAttributes(out,hname,"->GetZaxis()");
   TString opt = option;
   opt.ToLower();
   if (!opt.Contains("nodraw")) {
      out<<"   "<<hname<<"->Draw("
         <<quote<<option<<quote<<");"<<endl;
   }
}

//______________________________________________________________________________
void TH1::UseCurrentStyle()
{
   //   Copy current attributes from/to current style

   if (!gStyle) return;
   if (gStyle->IsReading()) {
      fXaxis.ResetAttAxis("X");
      fYaxis.ResetAttAxis("Y");
      fZaxis.ResetAttAxis("Z");
      SetBarOffset(gStyle->GetBarOffset());
      SetBarWidth(gStyle->GetBarWidth());
      SetFillColor(gStyle->GetHistFillColor());
      SetFillStyle(gStyle->GetHistFillStyle());
      SetLineColor(gStyle->GetHistLineColor());
      SetLineStyle(gStyle->GetHistLineStyle());
      SetLineWidth(gStyle->GetHistLineWidth());
      SetMarkerColor(gStyle->GetMarkerColor());
      SetMarkerStyle(gStyle->GetMarkerStyle());
      SetMarkerSize(gStyle->GetMarkerSize());
      Int_t dostat = gStyle->GetOptStat();
      if (gStyle->GetOptFit() && !dostat) dostat = 1000000001;
      SetStats(dostat);
   } else {
      gStyle->SetBarOffset(fBarOffset);
      gStyle->SetBarWidth(fBarWidth);
      gStyle->SetHistFillColor(GetFillColor());
      gStyle->SetHistFillStyle(GetFillStyle());
      gStyle->SetHistLineColor(GetLineColor());
      gStyle->SetHistLineStyle(GetLineStyle());
      gStyle->SetHistLineWidth(GetLineWidth());
      gStyle->SetMarkerColor(GetMarkerColor());
      gStyle->SetMarkerStyle(GetMarkerStyle());
      gStyle->SetMarkerSize(GetMarkerSize());
      gStyle->SetOptStat(TestBit(kNoStats));
   }
   TIter next(GetListOfFunctions());
   TObject *obj;

   while ((obj = next())) {
      obj->UseCurrentStyle();
   }
}

//______________________________________________________________________________
Double_t TH1::GetMean(Int_t axis) const
{
   //  For axis = 1,2 or 3 returns the mean value of the histogram along
   //  X,Y or Z axis.
   //  For axis = 11, 12, 13 returns the standard error of the mean value
   //  of the histogram along X, Y or Z axis
   //
   //  Note that the mean value/RMS is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.



   //   -*-*-*-*-*-*Return mean value of this histogram along the X axis*-*-*-*-*
   //               ====================================================
   //  Note that the mean value/RMS is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.

   if (axis<1 || (axis>3 && axis<11) || axis>13) return 0;
   Double_t stats[kNstat];
   for (Int_t i=4;i<kNstat;i++) stats[i] = 0;
   GetStats(stats);
   if (stats[0] == 0) return 0;
   if (axis<4){
      Int_t ax[3] = {2,4,7};
      return stats[ax[axis-1]]/stats[0];
   } else {
      // mean error = RMS / sqrt( Neff )
      Double_t rms = GetRMS(axis-10);
      Double_t neff = GetEffectiveEntries();
      return ( neff > 0 ? rms/TMath::Sqrt(neff) : 0. );
   }
}

//______________________________________________________________________________
Double_t TH1::GetMeanError(Int_t axis) const
{
   //   -*-*-*-*-*-*Return standard error of mean of this histogram along the X axis*-*-*-*-*
   //               ====================================================
   //  Note that the mean value/RMS is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.
   //  Also note, that although the definition of standard error doesn't include the
   //  assumption of normality, many uses of this feature implicitly assume it.

   return GetMean(axis+10);
}

//______________________________________________________________________________
Double_t TH1::GetRMS(Int_t axis) const
{
   //  For axis = 1,2 or 3 returns the Sigma value of the histogram along
   //  X, Y or Z axis
   //  For axis = 11, 12 or 13 returns the error of RMS estimation along
   //  X, Y or Z axis for Normal distribution
   //
   //     Note that the mean value/sigma is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.
   //  Note that this function returns the Standard Deviation (Sigma)
   //  of the distribution (not RMS).
   //  The Sigma estimate is computed as Sqrt((1/N)*(Sum(x_i-x_mean)^2))
   //  The name "RMS" was introduced many years ago (Hbook/PAW times).
   //  We kept the name for continuity.

   if (axis<1 || (axis>3 && axis<11) || axis>13) return 0;

   Double_t x, rms2, stats[kNstat];
   for (Int_t i=4;i<kNstat;i++) stats[i] = 0;
   GetStats(stats);
   if (stats[0] == 0) return 0;
   Int_t ax[3] = {2,4,7};
   Int_t axm = ax[axis%10 - 1];
   x    = stats[axm]/stats[0];
   rms2 = TMath::Abs(stats[axm+1]/stats[0] -x*x);
   if (axis<10)
      return TMath::Sqrt(rms2);
   else {
      // The right formula for RMS error depends on 4th momentum (see Kendall-Stuart Vol 1 pag 243)
      // formula valid for only gaussian distribution ( 4-th momentum =  3 * sigma^4 )
      Double_t neff = GetEffectiveEntries();
      return ( neff > 0 ? TMath::Sqrt(rms2/(2*neff) ) : 0. );
   }
}

//______________________________________________________________________________
Double_t TH1::GetRMSError(Int_t axis) const
{
   //  Return error of RMS estimation for Normal distribution
   //
   //  Note that the mean value/RMS is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.
   //  Value returned is standard deviation of sample standard deviation.
   //  Note that it is an approximated value which is valid only in the case that the
   //  original data distribution is Normal. The correct one would require
   //  the 4-th momentum value, which cannot be accurately estimated from an histogram since
   //  the x-information for all entries is not kept.

   return GetRMS(axis+10);
}

//______________________________________________________________________________
Double_t TH1::GetSkewness(Int_t axis) const
{
   //For axis = 1, 2 or 3 returns skewness of the histogram along x, y or z axis.
   //For axis = 11, 12 or 13 returns the approximate standard error of skewness
   //of the histogram along x, y or z axis
   //Note, that since third and fourth moment are not calculated
   //at the fill time, skewness and its standard error are computed bin by bin


   if (axis > 0 && axis <= 3){

      Double_t mean = GetMean(axis);
      Double_t rms = GetRMS(axis);
      Double_t rms3 = rms*rms*rms;

      Int_t firstBinX = fXaxis.GetFirst();
      Int_t lastBinX  = fXaxis.GetLast();
      Int_t firstBinY = fYaxis.GetFirst();
      Int_t lastBinY  = fYaxis.GetLast();
      Int_t firstBinZ = fZaxis.GetFirst();
      Int_t lastBinZ  = fZaxis.GetLast();
      // include underflow/overflow if TH1::StatOverflows(kTRUE) in case no range is set on the axis
      if (fgStatOverflows) {
        if ( !fXaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinX == 1) firstBinX = 0;
            if (lastBinX ==  fXaxis.GetNbins() ) lastBinX += 1;
         }
         if ( !fYaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinY == 1) firstBinY = 0;
            if (lastBinY ==  fYaxis.GetNbins() ) lastBinY += 1;
         }
         if ( !fZaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinZ == 1) firstBinZ = 0;
            if (lastBinZ ==  fZaxis.GetNbins() ) lastBinZ += 1;
         }
      }

      Double_t x = 0;
      Double_t sum=0;
      Double_t np=0;
      for (Int_t  binx = firstBinX; binx <= lastBinX; binx++) {
         for (Int_t biny = firstBinY; biny <= lastBinY; biny++) {
            for (Int_t binz = firstBinZ; binz <= lastBinZ; binz++) {
               if (axis==1 ) x = fXaxis.GetBinCenter(binx);
               else if (axis==2 ) x = fYaxis.GetBinCenter(biny);
               else if (axis==3 ) x = fZaxis.GetBinCenter(binz);
               Double_t w = GetBinContent(binx,biny,binz);
               np+=w;
               sum+=w*(x-mean)*(x-mean)*(x-mean);
            }
         }
      }
      sum/=np*rms3;
      return sum;
   }
   else if (axis > 10 && axis <= 13) {
      //compute standard error of skewness
      // assume parent normal distribution use formula from  Kendall-Stuart, Vol 1 pag 243, second edition
      Double_t neff = GetEffectiveEntries();
      return ( neff > 0 ? TMath::Sqrt(6./neff ) : 0. );
   }
   else {
      Error("GetSkewness", "illegal value of parameter");
      return 0;
   }
}

//______________________________________________________________________________
Double_t TH1::GetKurtosis(Int_t axis) const
{
   //For axis =1, 2 or 3 returns kurtosis of the histogram along x, y or z axis.
   //Kurtosis(gaussian(0, 1)) = 0.
   //For axis =11, 12 or 13 returns the approximate standard error of kurtosis
   //of the histogram along x, y or z axis
   //Note, that since third and fourth moment are not calculated
   //at the fill time, kurtosis and its standard error are computed bin by bin

   if (axis > 0 && axis <= 3){

      Double_t mean = GetMean(axis);
      Double_t rms = GetRMS(axis);
      Double_t rms4 = rms*rms*rms*rms;

      Int_t firstBinX = fXaxis.GetFirst();
      Int_t lastBinX  = fXaxis.GetLast();
      Int_t firstBinY = fYaxis.GetFirst();
      Int_t lastBinY  = fYaxis.GetLast();
      Int_t firstBinZ = fZaxis.GetFirst();
      Int_t lastBinZ  = fZaxis.GetLast();
      // include underflow/overflow if TH1::StatOverflows(kTRUE) in case no range is set on the axis
      if (fgStatOverflows) {
        if ( !fXaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinX == 1) firstBinX = 0;
            if (lastBinX ==  fXaxis.GetNbins() ) lastBinX += 1;
         }
         if ( !fYaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinY == 1) firstBinY = 0;
            if (lastBinY ==  fYaxis.GetNbins() ) lastBinY += 1;
         }
         if ( !fZaxis.TestBit(TAxis::kAxisRange) ) {
            if (firstBinZ == 1) firstBinZ = 0;
            if (lastBinZ ==  fZaxis.GetNbins() ) lastBinZ += 1;
         }
      }

      Double_t x = 0;
      Double_t sum=0;
      Double_t np=0;
      for (Int_t binx = firstBinX; binx <= lastBinX; binx++) {
         for (Int_t biny = firstBinY; biny <= lastBinY; biny++) {
            for (Int_t binz = firstBinZ; binz <= lastBinZ; binz++) {
               if (axis==1 ) x = fXaxis.GetBinCenter(binx);
               else if (axis==2 ) x = fYaxis.GetBinCenter(biny);
               else if (axis==3 ) x = fZaxis.GetBinCenter(binz);
               Double_t w = GetBinContent(binx,biny,binz);
               np+=w;
               sum+=w*(x-mean)*(x-mean)*(x-mean)*(x-mean);
            }
         }
      }
      sum/=(np*rms4);
      return sum-3;

   } else if (axis > 10 && axis <= 13) {
      //compute standard error of skewness
      // assume parent normal distribution use formula from  Kendall-Stuart, Vol 1 pag 243, second edition
      Double_t neff = GetEffectiveEntries();
      return ( neff > 0 ? TMath::Sqrt(24./neff ) : 0. );
   }
   else {
      Error("GetKurtosis", "illegal value of parameter");
      return 0;
   }
}


//______________________________________________________________________________
void TH1::GetStats(Double_t *stats) const
{
   // fill the array stats from the contents of this histogram
   // The array stats must be correctly dimensioned in the calling program.
   // stats[0] = sumw
   // stats[1] = sumw2
   // stats[2] = sumwx
   // stats[3] = sumwx2
   //
   // If no axis-subrange is specified (via TAxis::SetRange), the array stats
   // is simply a copy of the statistics quantities computed at filling time.
   // If a sub-range is specified, the function recomputes these quantities
   // from the bin contents in the current axis range.
   //
   //  Note that the mean value/RMS is computed using the bins in the currently
   //  defined range (see TAxis::SetRange). By default the range includes
   //  all bins from 1 to nbins included, excluding underflows and overflows.
   //  To force the underflows and overflows in the computation, one must
   //  call the static function TH1::StatOverflows(kTRUE) before filling
   //  the histogram.

   if (fBuffer) ((TH1*)this)->BufferEmpty();

   // Loop on bins (possibly including underflows/overflows)
   Int_t bin, binx;
   Double_t w,err;
   Double_t x;
   // case of labels with rebin of axis set
   // statistics in x does not make any sense - set to zero
   if ((const_cast<TAxis&>(fXaxis)).GetLabels() && TestBit(TH1::kCanRebin) ) {
      stats[0] = fTsumw;
      stats[1] = fTsumw2;
      stats[2] = 0;
      stats[3] = 0;
   }
   else if ((fTsumw == 0 && fEntries > 0) || fXaxis.TestBit(TAxis::kAxisRange)) {
      for (bin=0;bin<4;bin++) stats[bin] = 0;

      Int_t firstBinX = fXaxis.GetFirst();
      Int_t lastBinX  = fXaxis.GetLast();
      // include underflow/overflow if TH1::StatOverflows(kTRUE) in case no range is set on the axis
      if (fgStatOverflows && !fXaxis.TestBit(TAxis::kAxisRange)) {
         if (firstBinX == 1) firstBinX = 0;
         if (lastBinX ==  fXaxis.GetNbins() ) lastBinX += 1;
      }
      for (binx = firstBinX; binx <= lastBinX; binx++) {
         x   = fXaxis.GetBinCenter(binx);
         //w   = TMath::Abs(GetBinContent(binx));
         // not sure what to do here if w < 0
         w   = GetBinContent(binx);
         err = TMath::Abs(GetBinError(binx));
         stats[0] += w;
         stats[1] += err*err;
         stats[2] += w*x;
         stats[3] += w*x*x;
      }
      // if (stats[0] < 0) {
      //    // in case total is negative do something ??
      //    stats[0] = 0;
      // }
   } else {
      stats[0] = fTsumw;
      stats[1] = fTsumw2;
      stats[2] = fTsumwx;
      stats[3] = fTsumwx2;
   }
}

//______________________________________________________________________________
void TH1::PutStats(Double_t *stats)
{
   // Replace current statistics with the values in array stats

   fTsumw   = stats[0];
   fTsumw2  = stats[1];
   fTsumwx  = stats[2];
   fTsumwx2 = stats[3];
}

//______________________________________________________________________________
void TH1::ResetStats()
{
   // Reset the statistics including the number of entries
   // and replace with values calculates from bin content
   // The number of entries is set to the total bin content or (in case of weighted histogram)
   // to number of effective entries
   Double_t stats[kNstat] = {0};
   fTsumw = 0;
   fEntries = 1; // to force re-calculation of the statistics in TH1::GetStats
   GetStats(stats);
   PutStats(stats);
   fEntries = TMath::Abs(fTsumw);
   // use effective entries for weighted histograms:  (sum_w) ^2 / sum_w2
   if (fSumw2.fN > 0 && fTsumw > 0 && stats[1] > 0 ) fEntries = stats[0]*stats[0]/ stats[1];
}

//______________________________________________________________________________
Double_t TH1::GetSumOfWeights() const
{
   //   -*-*-*-*-*-*Return the sum of weights excluding under/overflows*-*-*-*-*
   //               ===================================================

   Int_t bin,binx,biny,binz;
   Double_t sum =0;
   for(binz=1; binz<=fZaxis.GetNbins(); binz++) {
      for(biny=1; biny<=fYaxis.GetNbins(); biny++) {
         for(binx=1; binx<=fXaxis.GetNbins(); binx++) {
            bin = GetBin(binx,biny,binz);
            sum += GetBinContent(bin);
         }
      }
   }
   return sum;
}


//______________________________________________________________________________
Double_t TH1::Integral(Option_t *option) const
{
   //Return integral of bin contents. Only bins in the bins range are considered.
   // By default the integral is computed as the sum of bin contents in the range.
   // if option "width" is specified, the integral is the sum of
   // the bin contents multiplied by the bin width in x.

   return Integral(fXaxis.GetFirst(),fXaxis.GetLast(),option);
}

//______________________________________________________________________________
Double_t TH1::Integral(Int_t binx1, Int_t binx2, Option_t *option) const
{
   //Return integral of bin contents in range [binx1,binx2]
   // By default the integral is computed as the sum of bin contents in the range.
   // if option "width" is specified, the integral is the sum of
   // the bin contents multiplied by the bin width in x.
   double err = 0;
   return DoIntegral(binx1,binx2,0,-1,0,-1,err,option);
}
//______________________________________________________________________________
Double_t TH1::IntegralAndError(Int_t binx1, Int_t binx2, Double_t & error, Option_t *option) const
{
   //Return integral of bin contents in range [binx1,binx2] and its error
   // By default the integral is computed as the sum of bin contents in the range.
   // if option "width" is specified, the integral is the sum of
   // the bin contents multiplied by the bin width in x.
   // the error is computed using error propagation from the bin errors assumming that
   // all the bins are uncorrelated
   return DoIntegral(binx1,binx2,0,-1,0,-1,error,option,kTRUE);
}


//______________________________________________________________________________
Double_t TH1::DoIntegral(Int_t binx1, Int_t binx2, Int_t biny1, Int_t biny2, Int_t binz1, Int_t binz2, Double_t & error ,
                          Option_t *option, Bool_t doError) const
{
   // internal function compute integral and optionally the error  between the limits
   // specified by the bin number values working for all histograms (1D, 2D and 3D)

   Int_t nbinsx = GetNbinsX();
   if (binx1 < 0) binx1 = 0;
   if (binx2 > nbinsx+1 || binx2 < binx1) binx2 = nbinsx+1;
   if (GetDimension() > 1) {
      Int_t nbinsy = GetNbinsY();
      if (biny1 < 0) biny1 = 0;
      if (biny2 > nbinsy+1 || biny2 < biny1) biny2 = nbinsy+1;
   } else {
      biny1 = 0; biny2 = 0;
   }
   if (GetDimension() > 2) {
      Int_t nbinsz = GetNbinsZ();
      if (binz1 < 0) binz1 = 0;
      if (binz2 > nbinsz+1 || binz2 < binz1) binz2 = nbinsz+1;
   } else {
      binz1 = 0; binz2 = 0;
   }

   //   - Loop on bins in specified range
   TString opt = option;
   opt.ToLower();
   Bool_t width   = kFALSE;
   if (opt.Contains("width")) width = kTRUE;


   Double_t dx = 1.;
   Double_t dy = 1.;
   Double_t dz = 1.;
   Double_t integral = 0;
   Double_t igerr2 = 0;
   for (Int_t binx = binx1; binx <= binx2; ++binx) {
      if (width) dx = fXaxis.GetBinWidth(binx);
      for (Int_t biny = biny1; biny <= biny2; ++biny) {
         if (width) dy = fYaxis.GetBinWidth(biny);
         for (Int_t binz = binz1; binz <= binz2; ++binz) {
            if (width) dz = fZaxis.GetBinWidth(binz);
            Int_t bin  = GetBin(binx, biny, binz);
            if (width) integral += GetBinContent(bin)*dx*dy*dz;
            else       integral += GetBinContent(bin);
            if (doError) {
               if (width)  igerr2 += GetBinError(bin)*GetBinError(bin)*dx*dx*dy*dy*dz*dz;
               else        igerr2 += GetBinError(bin)*GetBinError(bin);
            }
         }
      }
   }

   if (doError) error = TMath::Sqrt(igerr2);
   return integral;
}


//______________________________________________________________________________
Double_t TH1::AndersonDarlingTest(const TH1 *h2, Option_t *option) const
{
   //  Statistical test of compatibility in shape between
   //  this histogram and h2, using the Anderson-Darling 2 sample test.
   //  The AD 2 sample test formula are derived from the paper 
   //  F.W Scholz, M.A. Stephens "k-Sample Anderson-Darling Test". 
   //  The test is implemented in root in the ROOT::Math::GoFTest class
   //  It is the same formula ( (6) in the paper), and also shown in this preprint
   //  http://arxiv.org/pdf/0804.0380v1.pdf
   //  Binned data are considered as un-binned data 
   //   with identical observation happening in the bin center. 
   //
   //     option is a character string to specify options
   //         "D" Put out a line of "Debug" printout
   //         "T" Return the normalized A-D test statistic
   // 
   //  Note1: Underflow and overflow are not considered in the test
   //  Note2:  The test works only for un-weighted histogram (i.e. representing counts)
   //  Note3:  The histograms are not required to have the same X axis
   //  Note4:  The test works only for 1-dimensional histograms

   Double_t advalue = 0; 
   Double_t pvalue = AndersonDarlingTest(h2, advalue); 

   TString opt = option;
   opt.ToUpper();
   if (opt.Contains("D") ) {
      printf(" AndersonDarlingTest Prob     = %g, AD TestStatistic  = %g\n",pvalue,advalue);
   } 
   if (opt.Contains("T") ) return advalue; 

   return pvalue;    
}

//______________________________________________________________________________
Double_t TH1::AndersonDarlingTest(const TH1 *h2, Double_t & advalue) const
{
   // Same funciton as above but returning also the test statistic value

   if (GetDimension() != 1 || h2->GetDimension() != 1) {
      Error("AndersonDarlingTest","Histograms must be 1-D");
      return -1; 
   }

   // use the BinData class 
   ROOT::Fit::BinData data1; 
   ROOT::Fit::BinData data2; 
   ROOT::Fit::FillData(data1, this, 0);
   ROOT::Fit::FillData(data2, h2, 0);

   double pvalue; 
   ROOT::Math::GoFTest::AndersonDarling2SamplesTest(data1,data2, pvalue,advalue);

   return pvalue; 
}

//______________________________________________________________________________
Double_t TH1::KolmogorovTest(const TH1 *h2, Option_t *option) const
{
   //  Statistical test of compatibility in shape between
   //  this histogram and h2, using Kolmogorov test.
   //  Note that the KolmogorovTest (KS) test should in theory be used only for unbinned data
   //  and not for binned data as in the case of the histogram (see NOTE 3 below). 
   //  So, before using this method blindly, read the NOTE 3. 
   //
   //
   //     Default: Ignore under- and overflow bins in comparison
   //
   //     option is a character string to specify options
   //         "U" include Underflows in test  (also for 2-dim)
   //         "O" include Overflows     (also valid for 2-dim)
   //         "N" include comparison of normalizations
   //         "D" Put out a line of "Debug" printout
   //         "M" Return the Maximum Kolmogorov distance instead of prob
   //         "X" Run the pseudo experiments post-processor with the following procedure:
   //             make pseudoexperiments based on random values from the parent
   //             distribution and compare the KS distance of the pseudoexperiment
   //             to the parent distribution. Bin the KS distances in a histogram,
   //             and then take the integral of all the KS values above the value
   //             obtained from the original data to Monte Carlo distribution.
   //             The number of pseudo-experiments nEXPT is currently fixed at 1000.
   //             The function returns the integral.
   //             (thanks to Ben Kilminster to submit this procedure). Note that
   //             this option "X" is much slower.
   //
   //   The returned function value is the probability of test
   //       (much less than one means NOT compatible)
   //
   //  Code adapted by Rene Brun from original HBOOK routine HDIFF
   //
   //  NOTE1
   //  A good description of the Kolmogorov test can be seen at:
   //    http://www.itl.nist.gov/div898/handbook/eda/section3/eda35g.htm
   //
   //  NOTE2
   //  see also alternative function TH1::Chi2Test
   //  The Kolmogorov test is assumed to give better results than Chi2Test
   //  in case of histograms with low statistics.
   //
   //  NOTE3 (Jan Conrad, Fred James)
   //  "The returned value PROB is calculated such that it will be
   //  uniformly distributed between zero and one for compatible histograms,
   //  provided the data are not binned (or the number of bins is very large
   //  compared with the number of events). Users who have access to unbinned
   //  data and wish exact confidence levels should therefore not put their data
   //  into histograms, but should call directly TMath::KolmogorovTest. On
   //  the other hand, since TH1 is a convenient way of collecting data and
   //  saving space, this function has been provided. However, the values of
   //  PROB for binned data will be shifted slightly higher than expected,
   //  depending on the effects of the binning. For example, when comparing two
   //  uniform distributions of 500 events in 100 bins, the values of PROB,
   //  instead of being exactly uniformly distributed between zero and one, have
   //  a mean value of about 0.56. We can apply a useful
   //  rule: As long as the bin width is small compared with any significant
   //  physical effect (for example the experimental resolution) then the binning
   //  cannot have an important effect. Therefore, we believe that for all
   //  practical purposes, the probability value PROB is calculated correctly
   //  provided the user is aware that:
   //     1. The value of PROB should not be expected to have exactly the correct
   //  distribution for binned data.
   //     2. The user is responsible for seeing to it that the bin widths are
   //  small compared with any physical phenomena of interest.
   //     3. The effect of binning (if any) is always to make the value of PROB
   //  slightly too big. That is, setting an acceptance criterion of (PROB>0.05
   //  will assure that at most 5% of truly compatible histograms are rejected,
   //  and usually somewhat less."
   //
   //  Note also that for GoF test of unbinned data ROOT provides also the class
   //  ROOT::Math::GoFTest. The class has also method for doing one sample tests
   //  (i.e. comparing the data with a given distribution). 

   TString opt = option;
   opt.ToUpper();

   Double_t prob = 0;
   TH1 *h1 = (TH1*)this;
   if (h2 == 0) return 0;
   TAxis *axis1 = h1->GetXaxis();
   TAxis *axis2 = h2->GetXaxis();
   Int_t ncx1   = axis1->GetNbins();
   Int_t ncx2   = axis2->GetNbins();

   // Check consistency of dimensions
   if (h1->GetDimension() != 1 || h2->GetDimension() != 1) {
      Error("KolmogorovTest","Histograms must be 1-D\n");
      return 0;
   }

   // Check consistency in number of channels
   if (ncx1 != ncx2) {
      Error("KolmogorovTest","Number of channels is different, %d and %d\n",ncx1,ncx2);
      return 0;
   }

   // Check consistency in channel edges
   Double_t difprec = 1e-5;
   Double_t diff1 = TMath::Abs(axis1->GetXmin() - axis2->GetXmin());
   Double_t diff2 = TMath::Abs(axis1->GetXmax() - axis2->GetXmax());
   if (diff1 > difprec || diff2 > difprec) {
      Error("KolmogorovTest","histograms with different binning");
      return 0;
   }

   Bool_t afunc1 = kFALSE;
   Bool_t afunc2 = kFALSE;
   Double_t sum1 = 0, sum2 = 0;
   Double_t ew1, ew2, w1 = 0, w2 = 0;
   Int_t bin;
   Int_t ifirst = 1;
   Int_t ilast = ncx1;
   // integral of all bins (use underflow/overflow if option)
   if (opt.Contains("U")) ifirst = 0;
   if (opt.Contains("O")) ilast = ncx1 +1;
   for (bin = ifirst; bin <= ilast; bin++) {
      sum1 += h1->GetBinContent(bin);
      sum2 += h2->GetBinContent(bin);
      ew1   = h1->GetBinError(bin);
      ew2   = h2->GetBinError(bin);
      w1   += ew1*ew1;
      w2   += ew2*ew2;
   }
   if (sum1 == 0) {
      Error("KolmogorovTest","Histogram1 %s integral is zero\n",h1->GetName());
      return 0;
   }
   if (sum2 == 0) {
      Error("KolmogorovTest","Histogram2 %s integral is zero\n",h2->GetName());
      return 0;
   }

   // calculate the effective entries.
   // the case when errors are zero (w1 == 0 or w2 ==0) are equivalent to
   // compare to a function. In that case the rescaling is done only on sqrt(esum2) or sqrt(esum1)
   Double_t esum1 = 0, esum2 = 0;
   if (w1 > 0)
      esum1 = sum1 * sum1 / w1;
   else
      afunc1 = kTRUE;    // use later for calculating z

   if (w2 > 0)
      esum2 = sum2 * sum2 / w2;
   else
      afunc2 = kTRUE;    // use later for calculating z

   if (afunc2 && afunc1) {
      Error("KolmogorovTest","Errors are zero for both histograms\n");
      return 0;
   }


   Double_t s1 = 1/sum1;
   Double_t s2 = 1/sum2;

   // Find largest difference for Kolmogorov Test
   Double_t dfmax =0, rsum1 = 0, rsum2 = 0;

   for (bin=ifirst;bin<=ilast;bin++) {
      rsum1 += s1*h1->GetBinContent(bin);
      rsum2 += s2*h2->GetBinContent(bin);
      dfmax = TMath::Max(dfmax,TMath::Abs(rsum1-rsum2));
   }

   // Get Kolmogorov probability
   Double_t z, prb1=0, prb2=0, prb3=0;

   // case h1 is exact (has zero errors)
  if  (afunc1)
      z = dfmax*TMath::Sqrt(esum2);
  // case h2 has zero errors
  else if (afunc2)
      z = dfmax*TMath::Sqrt(esum1);
  else
     // for comparison between two data sets
     z = dfmax*TMath::Sqrt(esum1*esum2/(esum1+esum2));

   prob = TMath::KolmogorovProb(z);

   // option N to combine normalization makes sense if both afunc1 and afunc2 are false
   if (opt.Contains("N") && !(afunc1 || afunc2 ) ) {
      // Combine probabilities for shape and normalization,
      prb1 = prob;
      Double_t d12    = esum1-esum2;
      Double_t chi2   = d12*d12/(esum1+esum2);
      prb2 = TMath::Prob(chi2,1);
      // see Eadie et al., section 11.6.2
      if (prob > 0 && prb2 > 0) prob *= prb2*(1-TMath::Log(prob*prb2));
      else                      prob = 0;
   }
   // X option. Pseudo-experiments post-processor to determine KS probability
   const Int_t nEXPT = 1000;
   if (opt.Contains("X") && !(afunc1 || afunc2 ) ) {
      Double_t dSEXPT;
      TH1 *hExpt = (TH1*)(gDirectory ? gDirectory->CloneObject(this,kFALSE) : gROOT->CloneObject(this,kFALSE));
      // make nEXPT experiments (this should be a parameter)
      prb3 = 0;
      for (Int_t i=0; i < nEXPT; i++) {
         hExpt->Reset();
         hExpt->FillRandom(h1,(Int_t)esum2);
         dSEXPT = KolmogorovTest(hExpt,"M");
         if (dSEXPT>dfmax) prb3 += 1.0;
      }
      prb3 /= (Double_t)nEXPT;
      delete hExpt;
   }

   // debug printout
   if (opt.Contains("D")) {
      printf(" Kolmo Prob  h1 = %s, sum bin content =%g  effective entries =%g\n",h1->GetName(),sum1,esum1);
      printf(" Kolmo Prob  h2 = %s, sum bin content =%g  effective entries =%g\n",h2->GetName(),sum2,esum2);
      printf(" Kolmo Prob     = %g, Max Dist = %g\n",prob,dfmax);
      if (opt.Contains("N"))
         printf(" Kolmo Prob     = %f for shape alone, =%f for normalisation alone\n",prb1,prb2);
      if (opt.Contains("X"))
         printf(" Kolmo Prob     = %f with %d pseudo-experiments\n",prb3,nEXPT);
   }
   // This numerical error condition should never occur:
   if (TMath::Abs(rsum1-1) > 0.002) Warning("KolmogorovTest","Numerical problems with h1=%s\n",h1->GetName());
   if (TMath::Abs(rsum2-1) > 0.002) Warning("KolmogorovTest","Numerical problems with h2=%s\n",h2->GetName());

   if(opt.Contains("M"))      return dfmax;
   else if(opt.Contains("X")) return prb3;
   else                       return prob;
}

//______________________________________________________________________________
void TH1::SetContent(const Double_t *content)
{
   //   -*-*-*-*-*-*Replace bin contents by the contents of array content*-*-*-*
   //               =====================================================
   Int_t bin;
   Double_t bincontent;
   int nbins = fNcells; // save value because SetBinContent can change fNCells
   for (bin=0; bin<nbins; bin++) {
      bincontent = *(content + bin);
      SetBinContent(bin, bincontent);
   }
}

//______________________________________________________________________________
Int_t TH1::GetContour(Double_t *levels)
{
   //  Return contour values into array levels if pointer levels is non zero
   //
   //  The function returns the number of contour levels.
   //  see GetContourLevel to return one contour only
   //

   Int_t nlevels = fContour.fN;
   if (levels) {
      if (nlevels == 0) {
         nlevels = 20;
         SetContour(nlevels);
      } else {
         if (TestBit(kUserContour) == 0) SetContour(nlevels);
      }
      for (Int_t level=0; level<nlevels; level++) levels[level] = fContour.fArray[level];
   }
   return nlevels;
}

//______________________________________________________________________________
Double_t TH1::GetContourLevel(Int_t level) const
{
   // Return value of contour number level
   // see GetContour to return the array of all contour levels

   if (level <0 || level >= fContour.fN) return 0;
   Double_t zlevel = fContour.fArray[level];
   return zlevel;
}

//______________________________________________________________________________
Double_t TH1::GetContourLevelPad(Int_t level) const
{
   // Return the value of contour number "level" in Pad coordinates ie: if the Pad
   // is in log scale along Z it returns le log of the contour level value.
   // see GetContour to return the array of all contour levels

   if (level <0 || level >= fContour.fN) return 0;
   Double_t zlevel = fContour.fArray[level];

   // In case of user defined contours and Pad in log scale along Z,
   // fContour.fArray doesn't contain the log of the contour whereas it does
   // in case of equidistant contours.
   if (gPad && gPad->GetLogz() && TestBit(kUserContour)) {
      if (zlevel <= 0) return 0;
      zlevel = TMath::Log10(zlevel);
   }
   return zlevel;
}

//______________________________________________________________________________
void TH1::SetBuffer(Int_t buffersize, Option_t * /*option*/)
{
   // set the maximum number of entries to be kept in the buffer

   if (fBuffer) {
      BufferEmpty();
      delete [] fBuffer;
      fBuffer = 0;
   }
   if (buffersize <= 0) {
      fBufferSize = 0;
      return;
   }
   if (buffersize < 100) buffersize = 100;
   fBufferSize = 1 + buffersize*(fDimension+1);
   fBuffer = new Double_t[fBufferSize];
   memset(fBuffer,0,sizeof(Double_t)*fBufferSize);
}

//______________________________________________________________________________
void TH1::SetContour(Int_t  nlevels, const Double_t *levels)
{
   //  Set the number and values of contour levels.
   //
   //  By default the number of contour levels is set to 20. The contours values
   //  in the array "levels" should be specified in increasing order.
   //
   //  if argument levels = 0 or missing, equidistant contours are computed

   Int_t level;
   ResetBit(kUserContour);
   if (nlevels <=0 ) {
      fContour.Set(0);
      return;
   }
   fContour.Set(nlevels);

   //   -  Contour levels are specified
   if (levels) {
      SetBit(kUserContour);
      for (level=0; level<nlevels; level++) fContour.fArray[level] = levels[level];
   } else {
      //   - contour levels are computed automatically as equidistant contours
      Double_t zmin = GetMinimum();
      Double_t zmax = GetMaximum();
      if ((zmin == zmax) && (zmin != 0)) {
         zmax += 0.01*TMath::Abs(zmax);
         zmin -= 0.01*TMath::Abs(zmin);
      }
      Double_t dz   = (zmax-zmin)/Double_t(nlevels);
      if (gPad && gPad->GetLogz()) {
         if (zmax <= 0) return;
         if (zmin <= 0) zmin = 0.001*zmax;
         zmin = TMath::Log10(zmin);
         zmax = TMath::Log10(zmax);
         dz   = (zmax-zmin)/Double_t(nlevels);
      }
      for (level=0; level<nlevels; level++) {
         fContour.fArray[level] = zmin + dz*Double_t(level);
      }
   }
}

//______________________________________________________________________________
void TH1::SetContourLevel(Int_t level, Double_t value)
{
   // Set value for one contour level.

   if (level <0 || level >= fContour.fN) return;
   SetBit(kUserContour);
   fContour.fArray[level] = value;
}

//______________________________________________________________________________
Double_t TH1::GetMaximum(Double_t maxval) const
{
   //  Return maximum value smaller than maxval of bins in the range,
   //  unless the value has been overridden by TH1::SetMaximum,
   //  in which case it returns that value. (This happens, for example,
   //  when the histogram is drawn and the y or z axis limits are changed
   //
   //  To get the maximum value of bins in the histogram regardless of
   //  whether the value has been overridden, use
   //      h->GetBinContent(h->GetMaximumBin())

   if (fMaximum != -1111) return fMaximum;
   Int_t bin, binx, biny, binz;
   Int_t xfirst  = fXaxis.GetFirst();
   Int_t xlast   = fXaxis.GetLast();
   Int_t yfirst  = fYaxis.GetFirst();
   Int_t ylast   = fYaxis.GetLast();
   Int_t zfirst  = fZaxis.GetFirst();
   Int_t zlast   = fZaxis.GetLast();
   Double_t maximum = -FLT_MAX, value;
   for (binz=zfirst;binz<=zlast;binz++) {
      for (biny=yfirst;biny<=ylast;biny++) {
         for (binx=xfirst;binx<=xlast;binx++) {
            bin = GetBin(binx,biny,binz);
            value = GetBinContent(bin);
            if (value > maximum && value < maxval) maximum = value;
         }
      }
   }
   return maximum;
}

//______________________________________________________________________________
Int_t TH1::GetMaximumBin() const
{
   //   -*-*-*-*-*Return location of bin with maximum value in the range*-*
   //             ======================================================
   Int_t locmax, locmay, locmaz;
   return GetMaximumBin(locmax, locmay, locmaz);
}

//______________________________________________________________________________
Int_t TH1::GetMaximumBin(Int_t &locmax, Int_t &locmay, Int_t &locmaz) const
{
   //   -*-*-*-*-*Return location of bin with maximum value in the range*-*
   //             ======================================================
   Int_t bin, binx, biny, binz;
   Int_t locm;
   Int_t xfirst  = fXaxis.GetFirst();
   Int_t xlast   = fXaxis.GetLast();
   Int_t yfirst  = fYaxis.GetFirst();
   Int_t ylast   = fYaxis.GetLast();
   Int_t zfirst  = fZaxis.GetFirst();
   Int_t zlast   = fZaxis.GetLast();
   Double_t maximum = -FLT_MAX, value;
   locm = locmax = locmay = locmaz = 0;
   for (binz=zfirst;binz<=zlast;binz++) {
      for (biny=yfirst;biny<=ylast;biny++) {
         for (binx=xfirst;binx<=xlast;binx++) {
            bin = GetBin(binx,biny,binz);
            value = GetBinContent(bin);
            if (value > maximum) {
               maximum = value;
               locm    = bin;
               locmax  = binx;
               locmay  = biny;
               locmaz  = binz;
            }
         }
      }
   }
   return locm;
}

//______________________________________________________________________________
Double_t TH1::GetMinimum(Double_t minval) const
{
   //  Return minimum value larger than minval of bins in the range,
   //  unless the value has been overridden by TH1::SetMinimum,
   //  in which case it returns that value. (This happens, for example,
   //  when the histogram is drawn and the y or z axis limits are changed
   //
   //  To get the minimum value of bins in the histogram regardless of
   //  whether the value has been overridden, use
   //     h->GetBinContent(h->GetMinimumBin())

   if (fMinimum != -1111) return fMinimum;
   Int_t bin, binx, biny, binz;
   Int_t xfirst  = fXaxis.GetFirst();
   Int_t xlast   = fXaxis.GetLast();
   Int_t yfirst  = fYaxis.GetFirst();
   Int_t ylast   = fYaxis.GetLast();
   Int_t zfirst  = fZaxis.GetFirst();
   Int_t zlast   = fZaxis.GetLast();
   Double_t minimum=FLT_MAX, value;
   for (binz=zfirst;binz<=zlast;binz++) {
      for (biny=yfirst;biny<=ylast;biny++) {
         for (binx=xfirst;binx<=xlast;binx++) {
            bin = GetBin(binx,biny,binz);
            value = GetBinContent(bin);
            if (value < minimum && value > minval) minimum = value;
         }
      }
   }
   return minimum;
}

//______________________________________________________________________________
Int_t TH1::GetMinimumBin() const
{
   //   -*-*-*-*-*Return location of bin with minimum value in the range*-*
   //             ======================================================
   Int_t locmix, locmiy, locmiz;
   return GetMinimumBin(locmix, locmiy, locmiz);
}

//______________________________________________________________________________
Int_t TH1::GetMinimumBin(Int_t &locmix, Int_t &locmiy, Int_t &locmiz) const
{
   //   -*-*-*-*-*Return location of bin with minimum value in the range*-*
   //             ======================================================
   Int_t bin, binx, biny, binz;
   Int_t locm;
   Int_t xfirst  = fXaxis.GetFirst();
   Int_t xlast   = fXaxis.GetLast();
   Int_t yfirst  = fYaxis.GetFirst();
   Int_t ylast   = fYaxis.GetLast();
   Int_t zfirst  = fZaxis.GetFirst();
   Int_t zlast   = fZaxis.GetLast();
   Double_t minimum = FLT_MAX, value;
   locm = locmix = locmiy = locmiz = 0;
   for (binz=zfirst;binz<=zlast;binz++) {
      for (biny=yfirst;biny<=ylast;biny++) {
         for (binx=xfirst;binx<=xlast;binx++) {
            bin = GetBin(binx,biny,binz);
            value = GetBinContent(bin);
            if (value < minimum) {
               minimum = value;
               locm    = bin;
               locmix  = binx;
               locmiy  = biny;
               locmiz  = binz;
            }
         }
      }
   }
   return locm;
}

//______________________________________________________________________________
void TH1::SetBins(Int_t nx, Double_t xmin, Double_t xmax)
{
   //   -*-*-*-*-*-*-*Redefine  x axis parameters*-*-*-*-*-*-*-*-*-*-*-*
   //                 ===========================
   // The X axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange

   if (GetDimension() != 1) {
      Error("SetBins","Operation only valid for 1-d histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fXaxis.Set(nx,xmin,xmax);
   fYaxis.Set(1,0,1);
   fZaxis.Set(1,0,1);
   fNcells = nx+2;
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::SetBins(Int_t nx, const Double_t *xBins)
{
   //   -*-*-*-*-*-*-*Redefine  x axis parameters with variable bin sizes *-*-*-*-*-*-*-*-*-*
   //                 ===================================================
   // The X axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange
   // xBins is supposed to be of length nx+1
   if (GetDimension() != 1) {
      Error("SetBins","Operation only valid for 1-d histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fXaxis.Set(nx,xBins);
   fYaxis.Set(1,0,1);
   fZaxis.Set(1,0,1);
   fNcells = nx+2;
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::SetBins(Int_t nx, Double_t xmin, Double_t xmax, Int_t ny, Double_t ymin, Double_t ymax)
{
   //   -*-*-*-*-*-*-*Redefine  x and y axis parameters*-*-*-*-*-*-*-*-*-*-*-*
   //                 =================================
   // The X and Y axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange

   if (GetDimension() != 2) {
      Error("SetBins","Operation only valid for 2-D histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fYaxis.SetRange(0,0);
   fXaxis.Set(nx,xmin,xmax);
   fYaxis.Set(ny,ymin,ymax);
   fZaxis.Set(1,0,1);
   fNcells = (nx+2)*(ny+2);
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::SetBins(Int_t nx, const Double_t *xBins, Int_t ny, const Double_t *yBins)
{
   //   -*-*-*-*-*-*-*Redefine  x and y axis parameters with variable bin sizes *-*-*-*-*-*-*-*-*
   //                 =========================================================
   // The X and Y axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange
   // xBins is supposed to be of length nx+1, yBins is supposed to be of length ny+1

   if (GetDimension() != 2) {
      Error("SetBins","Operation only valid for 2-D histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fYaxis.SetRange(0,0);
   fXaxis.Set(nx,xBins);
   fYaxis.Set(ny,yBins);
   fZaxis.Set(1,0,1);
   fNcells = (nx+2)*(ny+2);
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}


//______________________________________________________________________________
void TH1::SetBins(Int_t nx, Double_t xmin, Double_t xmax, Int_t ny, Double_t ymin, Double_t ymax, Int_t nz, Double_t zmin, Double_t zmax)
{
   //   -*-*-*-*-*-*-*Redefine  x, y and z axis parameters*-*-*-*-*-*-*-*-*-*-*-*
   //                 ====================================
   // The X, Y and Z axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange

   if (GetDimension() != 3) {
      Error("SetBins","Operation only valid for 3-D histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fYaxis.SetRange(0,0);
   fZaxis.SetRange(0,0);
   fXaxis.Set(nx,xmin,xmax);
   fYaxis.Set(ny,ymin,ymax);
   fZaxis.Set(nz,zmin,zmax);
   fNcells = (nx+2)*(ny+2)*(nz+2);
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::SetBins(Int_t nx, const Double_t *xBins, Int_t ny, const Double_t *yBins, Int_t nz, const Double_t *zBins)
{
   //   -*-*-*-*-*-*-*Redefine  x, y and z axis parameters with variable bin sizes *-*-*-*-*-*-*-*-*
   //                 ============================================================
   // The X, Y and Z axis parameters are modified.
   // The bins content array is resized
   // if errors (Sumw2) the errors array is resized
   // The previous bin contents are lost
   // To change only the axis limits, see TAxis::SetRange
   // xBins is supposed to be of length nx+1, yBins is supposed to be of length ny+1,
   // zBins is supposed to be of length nz+1

   if (GetDimension() != 3) {
      Error("SetBins","Operation only valid for 3-D histograms");
      return;
   }
   fXaxis.SetRange(0,0);
   fYaxis.SetRange(0,0);
   fZaxis.SetRange(0,0);
   fXaxis.Set(nx,xBins);
   fYaxis.Set(ny,yBins);
   fZaxis.Set(nz,zBins);
   fNcells = (nx+2)*(ny+2)*(nz+2);
   SetBinsLength(fNcells);
   if (fSumw2.fN) {
      fSumw2.Set(fNcells);
   }
}

//______________________________________________________________________________
void TH1::SetMaximum(Double_t maximum)
{
   // Set the maximum value for the Y axis, in case of 1-D histograms,
   // or the Z axis in case of 2-D histograms
   //
   // By default the maximum value used in drawing is the maximum value of the histogram plus
   // a margin of 10 per cent. If this function has been called, the value of 'maximum' is
   // used, with no extra margin.
   //
   // TH1::GetMaximum returns the maximum value of the bins in the histogram, unless the
   // maximum has been set manually by this function or by altering the y/z axis limits.
   // Use TH1::GetMaximumBin to find the bin with the maximum value of an histogram
   //
   fMaximum = maximum;
}


//______________________________________________________________________________
void TH1::SetMinimum(Double_t minimum)
{
   // Set the minimum value for the Y axis, in case of 1-D histograms,
   // or the Z axis in case of 2-D histograms
   //
   // By default the minimum value used in drawing is the minimum value of the histogram plus
   // a margin of 10 per cent. If this function has been called, the value of 'minimum' is
   // used, with no extra margin.
   //
   // TH1::GetMinimum returns the minimum value of the bins in the histogram, unless the
   // minimum has been set manually by this function or by altering the y/z axis limits.
   // Use TH1::GetMinimumBin to find the bin with the minimum value of an histogram
   //
   fMinimum = minimum;
}

//______________________________________________________________________________
void TH1::SetDirectory(TDirectory *dir)
{
   // By default when an histogram is created, it is added to the list
   // of histogram objects in the current directory in memory.
   // Remove reference to this histogram from current directory and add
   // reference to new directory dir. dir can be 0 in which case the
   // histogram does not belong to any directory.

   if (fDirectory == dir) return;
   if (fDirectory) fDirectory->Remove(this);
   fDirectory = dir;
   if (fDirectory) fDirectory->Append(this);
}


//______________________________________________________________________________
void TH1::SetError(const Double_t *error)
{
   //   -*-*-*-*-*-*-*Replace bin errors by values in array error*-*-*-*-*-*-*-*-*
   //                 ===========================================
   Int_t bin;
   Double_t binerror;
   for (bin=0; bin<fNcells; bin++) {
      binerror = error[bin];
      SetBinError(bin, binerror);
   }
}

//______________________________________________________________________________
void TH1::SetName(const char *name)
{
   // Change the name of this histogram
   //

   //  Histograms are named objects in a THashList.
   //  We must update the hashlist if we change the name
   if (fDirectory) fDirectory->Remove(this);
   fName = name;
   if (fDirectory) fDirectory->Append(this);
}

//______________________________________________________________________________
void TH1::SetNameTitle(const char *name, const char *title)
{
   // Change the name and title of this histogram
   //

   //  Histograms are named objects in a THashList.
   //  We must update the hashlist if we change the name
   if (fDirectory) fDirectory->Remove(this);
   fName  = name;
   SetTitle(title);
   if (fDirectory) fDirectory->Append(this);
}

//______________________________________________________________________________
void TH1::SetStats(Bool_t stats)
{
   //   -*-*-*-*-*-*-*Set statistics option on/off
   //                 ============================
   //  By default, the statistics box is drawn.
   //  The paint options can be selected via gStyle->SetOptStats.
   //  This function sets/resets the kNoStats bin in the histogram object.
   //  It has priority over the Style option.

   ResetBit(kNoStats);
   if (!stats) {
      SetBit(kNoStats);
      //remove the "stats" object from the list of functions
      if (fFunctions) {
         TObject *obj = fFunctions->FindObject("stats");
         if (obj) {
            fFunctions->Remove(obj);
            delete obj;
         }
      }
   }
}

//______________________________________________________________________________
void TH1::Sumw2(Bool_t flag)
{
   // Create structure to store sum of squares of weights*-*-*-*-*-*-*-*
   //
   //     if histogram is already filled, the sum of squares of weights
   //     is filled with the existing bin contents
   //
   //     The error per bin will be computed as sqrt(sum of squares of weight)
   //     for each bin.
   //
   //  This function is automatically called when the histogram is created
   //  if the static function TH1::SetDefaultSumw2 has been called before.
   //  If flag = false the structure is deleted 
   
   if (!flag) { 
      // clear the array if existing - do nothing otherwise
      if (fSumw2.fN > 0 ) fSumw2.Set(0);
      return;
   }

   if (fSumw2.fN == fNcells) {
      if (!fgDefaultSumw2 )
         Warning("Sumw2","Sum of squares of weights structure already created");
      return;
   }

   fSumw2.Set(fNcells);

   if ( fEntries > 0 )
      for (Int_t bin=0; bin<fNcells; bin++) {
         fSumw2.fArray[bin] = TMath::Abs(GetBinContent(bin));
      }
}

//______________________________________________________________________________
TF1 *TH1::GetFunction(const char *name) const
{
   //   -*-*-*Return pointer to function with name*-*-*-*-*-*-*-*-*-*-*-*-*
   //         ===================================
   //
   // Functions such as TH1::Fit store the fitted function in the list of
   // functions of this histogram.

   return (TF1*)fFunctions->FindObject(name);
}

//______________________________________________________________________________
Double_t TH1::GetBinError(Int_t bin) const
{
   //   -*-*-*-*-*Return value of error associated to bin number bin*-*-*-*-*
   //             ==================================================
   //
   //    if the sum of squares of weights has been defined (via Sumw2),
   //    this function returns the sqrt(sum of w2).
   //    otherwise it returns the sqrt(contents) for this bin.
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (fBuffer) ((TH1*)this)->BufferEmpty();
   if (fSumw2.fN) {
      Double_t err2 = fSumw2.fArray[bin];
      return TMath::Sqrt(err2);
   }
   Double_t error2 = TMath::Abs(GetBinContent(bin));
   return TMath::Sqrt(error2);
}

//______________________________________________________________________________
Double_t TH1::GetBinErrorLow(Int_t bin) const
{
   //   -*-*-*-*-*Return lower error associated to bin number bin*-*-*-*-*
   //             ==================================================
   //
   //    The error will depend on the statistic option used will return
   //     the binContent - lower interval value
   //
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (fBinStatErrOpt == kNormal || fSumw2.fN) return GetBinError(bin);
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (fBuffer) ((TH1*)this)->BufferEmpty();

   Double_t alpha = 1.- 0.682689492;
   if (fBinStatErrOpt == kPoisson2) alpha = 0.05;

   Double_t c = GetBinContent(bin);
   Int_t n = int(c);
   if (n < 0) {
      Warning("GetBinErrorLow","Histogram has negative bin content-force usage to normal errors");
      ((TH1*)this)->fBinStatErrOpt = kNormal;
      return GetBinError(bin);
   }

   if (n == 0) return 0;
   return c - ROOT::Math::gamma_quantile( alpha/2, n, 1.);
}

//______________________________________________________________________________
Double_t TH1::GetBinErrorUp(Int_t bin) const
{
   //   -*-*-*-*-*Return upper error associated to bin number bin*-*-*-*-*
   //             ==================================================
   //
   //    The error will depend on the statistic option used will return
   //     the binContent - upper interval value
   //
   //
   //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   if (fBinStatErrOpt == kNormal || fSumw2.fN) return GetBinError(bin);
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (fBuffer) ((TH1*)this)->BufferEmpty();

   Double_t alpha = 1.- 0.682689492;
   if (fBinStatErrOpt == kPoisson2) alpha = 0.05;

   Double_t c = GetBinContent(bin);
   Int_t n = int(c);
   if (n < 0) {
      Warning("GetBinErrorUp","Histogram has negative bin content-force usage to normal errors");
      ((TH1*)this)->fBinStatErrOpt = kNormal;
      return GetBinError(bin);
   }

   // for N==0 return an upper limit at 0.68 or (1-alpha)/2 ?
   // decide to return always (1-alpha)/2 upper interval
   //if (n == 0) return ROOT::Math::gamma_quantile_c(alpha,n+1,1);
   return ROOT::Math::gamma_quantile_c( alpha/2, n+1, 1) - c;
}

//______________________________________________________________________________
Double_t TH1::GetBinError(Int_t binx, Int_t biny) const
{
   //   -*-*-*-*-*Return error of bin number binx, biny
   //             =====================================
   // NB: Function to be called for 2-D histograms only

   Int_t bin = GetBin(binx,biny);
   return GetBinError(bin);
}

//______________________________________________________________________________
Double_t TH1::GetBinError(Int_t binx, Int_t biny, Int_t binz) const
{
   //   -*-*-*-*-*Return error of bin number binx,biny,binz
   //             =========================================
   // NB: Function to be called for 3-D histograms only

   Int_t bin = GetBin(binx,biny,binz);
   return GetBinError(bin);
}

//______________________________________________________________________________
Double_t TH1::GetCellContent(Int_t binx, Int_t biny) const
{
   //   -*-*-*-*-*Return content of bin number binx, biny
   //             =====================================
   // NB: Function to be called for 2-D histograms only

   Int_t bin = GetBin(binx,biny);
   return GetBinContent(bin);
}

//______________________________________________________________________________
Double_t TH1::GetCellError(Int_t binx, Int_t biny) const
{
   //   -*-*-*-*-*Return error of bin number binx, biny
   //             =====================================
   // NB: Function to be called for 2-D histograms only

   Int_t bin = GetBin(binx,biny);
   return GetBinError(bin);
}

//L.M. These following getters are useless and should be probably deprecated
//______________________________________________________________________________
Double_t TH1::GetBinCenter(Int_t bin) const 
{
   // return bin center for 1D historam
   // Better to use h1.GetXaxis().GetBinCenter(bin)
   
   if (fDimension == 1) return  fXaxis.GetBinCenter(bin);
   Error("GetBinCenter","Invalid method for a %d-d histogram - return a NaN",fDimension);
   return TMath::QuietNaN();
}

//______________________________________________________________________________
Double_t TH1::GetBinLowEdge(Int_t bin) const 
{
   // return bin lower edge for 1D historam
   // Better to use h1.GetXaxis().GetBinLowEdge(bin)
   
   if (fDimension == 1) return  fXaxis.GetBinLowEdge(bin);
   Error("GetBinLowEdge","Invalid method for a %d-d histogram - return a NaN",fDimension);
   return TMath::QuietNaN();
}

//______________________________________________________________________________
Double_t TH1::GetBinWidth(Int_t bin) const 
{
   // return bin width for 1D historam
   // Better to use h1.GetXaxis().GetBinWidth(bin)
   
   if (fDimension == 1) return  fXaxis.GetBinWidth(bin);
   Error("GetBinWidth","Invalid method for a %d-d histogram - return a NaN",fDimension);
   return TMath::QuietNaN();
}

//______________________________________________________________________________
void TH1::GetCenter(Double_t *center) const 
{
   // Fill array with center of bins for 1D histogram
   // Better to use h1.GetXaxis().GetCenter(center)
   
   if (fDimension == 1) {
      fXaxis.GetCenter(center);
      return;
   }
   Error("GetCenter","Invalid method for a %d-d histogram ",fDimension);
}

//______________________________________________________________________________
void TH1::GetLowEdge(Double_t *edge) const 
{
   // Fill array with low edge of bins for 1D histogram
   // Better to use h1.GetXaxis().GetLowEdge(edge)
   
   if (fDimension == 1) {
      fXaxis.GetLowEdge(edge);
      return;
   }
   Error("GetLowEdge","Invalid method for a %d-d histogram ",fDimension);
}

//______________________________________________________________________________
void TH1::SetBinError(Int_t bin, Double_t error)
{
   // see convention for numbering bins in TH1::GetBin
   if (!fSumw2.fN) Sumw2();
   if (bin <0 || bin>= fSumw2.fN) return;
   fSumw2.fArray[bin] = error*error;
}

//______________________________________________________________________________
void TH1::SetBinContent(Int_t binx, Int_t biny, Double_t content)
{
   // see convention for numbering bins in TH1::GetBin
   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   Int_t bin = GetBin(binx,biny);
   SetBinContent(bin,content);
}

//______________________________________________________________________________
void TH1::SetBinContent(Int_t binx, Int_t biny, Int_t binz, Double_t content)
{
   // see convention for numbering bins in TH1::GetBin
   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   if (binz <0 || binz>fZaxis.GetNbins()+1) return;
   Int_t bin = GetBin(binx,biny,binz);
   SetBinContent(bin,content);
}

//______________________________________________________________________________
void TH1::SetCellContent(Int_t binx, Int_t biny, Double_t content)
{
   // Set cell content.

   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   Int_t bin = GetBin(binx,biny);
   SetBinContent(bin,content);
}

//______________________________________________________________________________
void TH1::SetBinError(Int_t binx, Int_t biny, Double_t error)
{
   // see convention for numbering bins in TH1::GetBin
   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   Int_t bin = GetBin(binx,biny);
   SetBinError(bin,error);
}

//______________________________________________________________________________
void TH1::SetBinError(Int_t binx, Int_t biny, Int_t binz, Double_t error)
{
   // see convention for numbering bins in TH1::GetBin
   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   if (binz <0 || binz>fZaxis.GetNbins()+1) return;
   Int_t bin = GetBin(binx,biny,binz);
   SetBinError(bin,error);
}

//______________________________________________________________________________
void TH1::SetCellError(Int_t binx, Int_t biny, Double_t error)
{
   // see convention for numbering bins in TH1::GetBin
   if (binx <0 || binx>fXaxis.GetNbins()+1) return;
   if (biny <0 || biny>fYaxis.GetNbins()+1) return;
   if (!fSumw2.fN) Sumw2();
   Int_t bin = biny*(fXaxis.GetNbins()+2) + binx;
   fSumw2.fArray[bin] = error*error;
}

//______________________________________________________________________________
void TH1::SetBinContent(Int_t, Double_t)
{
   // see convention for numbering bins in TH1::GetBin
   AbstractMethod("SetBinContent");
}

//______________________________________________________________________________
TH1 *TH1::ShowBackground(Int_t niter, Option_t *option)
{
//   This function calculates the background spectrum in this histogram.
//   The background is returned as a histogram.
//
//   Function parameters:
//   -niter, number of iterations (default value = 2)
//      Increasing niter make the result smoother and lower.
//   -option: may contain one of the following options
//      - to set the direction parameter
//        "BackDecreasingWindow". By default the direction is BackIncreasingWindow
//      - filterOrder-order of clipping filter,  (default "BackOrder2"
//                  -possible values= "BackOrder4"
//                                    "BackOrder6"
//                                    "BackOrder8"
//      - "nosmoothing"- if selected, the background is not smoothed
//           By default the background is smoothed.
//      - smoothWindow-width of smoothing window, (default is "BackSmoothing3")
//                  -possible values= "BackSmoothing5"
//                                    "BackSmoothing7"
//                                    "BackSmoothing9"
//                                    "BackSmoothing11"
//                                    "BackSmoothing13"
//                                    "BackSmoothing15"
//      - "nocompton"- if selected the estimation of Compton edge
//                  will be not be included   (by default the compton estimation is set)
//      - "same" : if this option is specified, the resulting background
//                 histogram is superimposed on the picture in the current pad.
//                 This option is given by default.
//
//  NOTE that the background is only evaluated in the current range of this histogram.
//  i.e., if this has a bin range (set via h->GetXaxis()->SetRange(binmin, binmax),
//  the returned histogram will be created with the same number of bins
//  as this input histogram, but only bins from binmin to binmax will be filled
//  with the estimated background.
//


   return (TH1*)gROOT->ProcessLineFast(Form("TSpectrum::StaticBackground((TH1*)0x%lx,%d,\"%s\")",
                                            (ULong_t)this, niter, option));
}

//______________________________________________________________________________
Int_t TH1::ShowPeaks(Double_t sigma, Option_t *option, Double_t threshold)
{
   //Interface to TSpectrum::Search.
   //The function finds peaks in this histogram where the width is > sigma
   //and the peak maximum greater than threshold*maximum bin content of this.
   //For more details see TSpectrum::Search.
   //Note the difference in the default value for option compared to TSpectrum::Search
   //option="" by default (instead of "goff").

   return (Int_t)gROOT->ProcessLineFast(Form("TSpectrum::StaticSearch((TH1*)0x%lx,%g,\"%s\",%g)",
                                             (ULong_t)this, sigma, option, threshold));
}


//______________________________________________________________________________
TH1* TH1::TransformHisto(TVirtualFFT *fft, TH1* h_output,  Option_t *option)
{
//For a given transform (first parameter), fills the histogram (second parameter)
//with the transform output data, specified in the third parameter
//If the 2nd parameter h_output is empty, a new histogram (TH1D or TH2D) is created
//and the user is responsible for deleting it.
// Available options:
//   "RE" - real part of the output
//   "IM" - imaginary part of the output
//   "MAG" - magnitude of the output
//   "PH"  - phase of the output

   if (!fft ||  !fft->GetN() ) {
      ::Error("TransformHisto","Invalid FFT transform class");
      return 0;
   }

   if (fft->GetNdim()>2){
      ::Error("TransformHisto","Only 1d and 2D transform are supported");
      return 0;
   }
   Int_t binx,biny;
   TString opt = option;
   opt.ToUpper();
   Int_t *n = fft->GetN();
   TH1 *hout=0;
   if (h_output) {
      hout = h_output;
   }
   else {
      TString name = TString::Format("out_%s", opt.Data());
      if (fft->GetNdim()==1)
         hout = new TH1D(name, name,n[0], 0, n[0]);
      else if (fft->GetNdim()==2)
         hout = new TH2D(name, name, n[0], 0, n[0], n[1], 0, n[1]);
   }
   R__ASSERT(hout != 0);
   TString type=fft->GetType();
   Int_t ind[2];
   if (opt.Contains("RE")){
      if (type.Contains("2C") || type.Contains("2HC")) {
         Double_t re, im;
         for (binx = 1; binx<=hout->GetNbinsX(); binx++) {
            for (biny=1; biny<=hout->GetNbinsY(); biny++) {
               ind[0] = binx-1; ind[1] = biny-1;
               fft->GetPointComplex(ind, re, im);
               hout->SetBinContent(binx, biny, re);
            }
         }
      } else {
         for (binx = 1; binx<=hout->GetNbinsX(); binx++) {
            for (biny=1; biny<=hout->GetNbinsY(); biny++) {
               ind[0] = binx-1; ind[1] = biny-1;
               hout->SetBinContent(binx, biny, fft->GetPointReal(ind));
            }
         }
      }
   }
   if (opt.Contains("IM")) {
      if (type.Contains("2C") || type.Contains("2HC")) {
         Double_t re, im;
         for (binx = 1; binx<=hout->GetNbinsX(); binx++) {
            for (biny=1; biny<=hout->GetNbinsY(); biny++) {
               ind[0] = binx-1; ind[1] = biny-1;
               fft->GetPointComplex(ind, re, im);
               hout->SetBinContent(binx, biny, im);
            }
         }
      } else {
         ::Error("TransformHisto","No complex numbers in the output");
         return 0;
      }
   }
   if (opt.Contains("MA")) {
      if (type.Contains("2C") || type.Contains("2HC")) {
         Double_t re, im;
         for (binx = 1; binx<=hout->GetNbinsX(); binx++) {
            for (biny=1; biny<=hout->GetNbinsY(); biny++) {
               ind[0] = binx-1; ind[1] = biny-1;
               fft->GetPointComplex(ind, re, im);
               hout->SetBinContent(binx, biny, TMath::Sqrt(re*re + im*im));
            }
         }
      } else {
         for (binx = 1; binx<=hout->GetNbinsX(); binx++) {
            for (biny=1; biny<=hout->GetNbinsY(); biny++) {
               ind[0] = binx-1; ind[1] = biny-1;
               hout->SetBinContent(binx, biny, TMath::Abs(fft->GetPointReal(ind)));
            }
         }
      }
   }
   if (opt.Contains("PH")) {
      if (type.Contains("2C") || type.Contains("2HC")){
         Double_t re, im, ph;
         for (binx = 1; binx<=hout->GetNbinsX(); binx++){
            for (biny=1; biny<=hout->GetNbinsY(); biny++){
               ind[0] = binx-1; ind[1] = biny-1;
               fft->GetPointComplex(ind, re, im);
               if (TMath::Abs(re) > 1e-13){
                  ph = TMath::ATan(im/re);
                  //find the correct quadrant
                  if (re<0 && im<0)
                     ph -= TMath::Pi();
                  if (re<0 && im>=0)
                     ph += TMath::Pi();
               } else {
                  if (TMath::Abs(im) < 1e-13)
                     ph = 0;
                  else if (im>0)
                     ph = TMath::Pi()*0.5;
                  else
                     ph = -TMath::Pi()*0.5;
               }
               hout->SetBinContent(binx, biny, ph);
            }
         }
      } else {
         printf("Pure real output, no phase");
         return 0;
      }
   }

   return hout;
}



//______________________________________________________________________________
//                     TH1C methods
// TH1C : histograms with one byte per channel.   Maximum bin content = 127
//______________________________________________________________________________

ClassImp(TH1C)

//______________________________________________________________________________
TH1C::TH1C(): TH1(), TArrayC()
{
   // Constructor.

   fDimension = 1;
   SetBinsLength(3);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1C::TH1C(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
: TH1(name,title,nbins,xlow,xup)
{
   //
   //    Create a 1-Dim histogram with fix bins of type char (one byte per channel)
   //    ==========================================================================
   //                    (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayC::Set(fNcells);

   if (xlow >= xup) SetBuffer(fgBufferSize);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1C::TH1C(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type char (one byte per channel)
   //    ==========================================================================
   //                    (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayC::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1C::TH1C(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type char (one byte per channel)
   //    ==========================================================================
   //                    (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayC::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1C::~TH1C()
{
   // Destructor.
}

//______________________________________________________________________________
TH1C::TH1C(const TH1C &h1c) : TH1(), TArrayC()
{
   // Copy constructor.

   ((TH1C&)h1c).Copy(*this);
}

//______________________________________________________________________________
void TH1C::AddBinContent(Int_t bin)
{
   //   -*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //                   ==========================

   if (fArray[bin] < 127) fArray[bin]++;
}

//______________________________________________________________________________
void TH1C::AddBinContent(Int_t bin, Double_t w)
{
   //   -*-*-*-*-*-*-*-*Increment bin content by w*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //                   ==========================

   Int_t newval = fArray[bin] + Int_t(w);
   if (newval > -128 && newval < 128) {fArray[bin] = Char_t(newval); return;}
   if (newval < -127) fArray[bin] = -127;
   if (newval >  127) fArray[bin] =  127;
}

//______________________________________________________________________________
void TH1C::Copy(TObject &newth1) const
{
   // Copy this to newth1

   TH1::Copy(newth1);
}

//______________________________________________________________________________
TH1 *TH1C::DrawCopy(Option_t *option) const
{
   // Draw copy.

   TString opt = option;
   opt.ToLower();
   if (gPad && !opt.Contains("same")) gPad->Clear();
   TH1C *newth1 = (TH1C*)Clone();
   newth1->SetDirectory(0);
   newth1->SetBit(kCanDelete);
   newth1->AppendPad(option);
   return newth1;
}

//______________________________________________________________________________
Double_t TH1C::GetBinContent(Int_t bin) const
{
   // see convention for numbering bins in TH1::GetBin

   if (fBuffer) ((TH1C*)this)->BufferEmpty();
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (!fArray) return 0;
   return Double_t (fArray[bin]);
}

//______________________________________________________________________________
void TH1C::Reset(Option_t *option)
{
   // Reset.

   TH1::Reset(option);
   TArrayC::Reset();
}

//______________________________________________________________________________
void TH1C::SetBinContent(Int_t bin, Double_t content)
{
   // Set bin content
   // see convention for numbering bins in TH1::GetBin
   // In case the bin number is greater than the number of bins and
   // the timedisplay option is set or the kCanRebin bit is set,
   // the number of bins is automatically doubled to accommodate the new bin

   fEntries++;
   fTsumw = 0;
   if (bin < 0) return;
   if (bin >= fNcells-1) {
      if (fXaxis.GetTimeDisplay()) {
         while (bin >=  fNcells-1)  LabelsInflate();
      } else {
         if (!TestBit(kCanRebin)) {
            if (bin == fNcells-1) fArray[bin] = Char_t (content);
            return;
         }
         while (bin >= fNcells-1)  LabelsInflate();
      }
   }
   fArray[bin] = Char_t (content);
}

//______________________________________________________________________________
void TH1C::SetBinsLength(Int_t n)
{
   // Set total number of bins including under/overflow
   // Reallocate bin contents array

   if (n < 0) n = fXaxis.GetNbins() + 2;
   fNcells = n;
   TArrayC::Set(n);
}

//______________________________________________________________________________
TH1C& TH1C::operator=(const TH1C &h1)
{
   // Operator =

   if (this != &h1)  ((TH1C&)h1).Copy(*this);
   return *this;
}


//______________________________________________________________________________
TH1C operator*(Double_t c1, const TH1C &h1)
{
   // Operator *

   TH1C hnew = h1;
   hnew.Scale(c1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1C operator+(const TH1C &h1, const TH1C &h2)
{
   // Operator +

   TH1C hnew = h1;
   hnew.Add(&h2,1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1C operator-(const TH1C &h1, const TH1C &h2)
{
   // Operator -

   TH1C hnew = h1;
   hnew.Add(&h2,-1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1C operator*(const TH1C &h1, const TH1C &h2)
{
   // Operator *

   TH1C hnew = h1;
   hnew.Multiply(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1C operator/(const TH1C &h1, const TH1C &h2)
{
   // Operator /

   TH1C hnew = h1;
   hnew.Divide(&h2);
   hnew.SetDirectory(0);
   return hnew;
}



//______________________________________________________________________________
//                     TH1S methods
// TH1S : histograms with one short per channel.  Maximum bin content = 32767
//______________________________________________________________________________

ClassImp(TH1S)

//______________________________________________________________________________
TH1S::TH1S(): TH1(), TArrayS()
{
   // Constructor.

   fDimension = 1;
   SetBinsLength(3);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1S::TH1S(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
: TH1(name,title,nbins,xlow,xup)
{
   //
   //    Create a 1-Dim histogram with fix bins of type short
   //    ====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayS::Set(fNcells);

   if (xlow >= xup) SetBuffer(fgBufferSize);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1S::TH1S(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type short
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayS::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1S::TH1S(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type short
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayS::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1S::~TH1S()
{
   // Destructor.
}

//______________________________________________________________________________
TH1S::TH1S(const TH1S &h1s) : TH1(), TArrayS()
{
   // Copy constructor.

   ((TH1S&)h1s).Copy(*this);
}

//______________________________________________________________________________
void TH1S::AddBinContent(Int_t bin)
{
   //   -*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //                   ==========================

   if (fArray[bin] < 32767) fArray[bin]++;
}

//______________________________________________________________________________
void TH1S::AddBinContent(Int_t bin, Double_t w)
{
   //                   Increment bin content by w
   //                   ==========================

   Int_t newval = fArray[bin] + Int_t(w);
   if (newval > -32768 && newval < 32768) {fArray[bin] = Short_t(newval); return;}
   if (newval < -32767) fArray[bin] = -32767;
   if (newval >  32767) fArray[bin] =  32767;
}

//______________________________________________________________________________
void TH1S::Copy(TObject &newth1) const
{
   // Copy this to newth1

   TH1::Copy(newth1);
}

//______________________________________________________________________________
TH1 *TH1S::DrawCopy(Option_t *option) const
{
   // Draw copy.

   TString opt = option;
   opt.ToLower();
   if (gPad && !opt.Contains("same")) gPad->Clear();
   TH1S *newth1 = (TH1S*)Clone();
   newth1->SetDirectory(0);
   newth1->SetBit(kCanDelete);
   newth1->AppendPad(option);
   return newth1;
}

//______________________________________________________________________________
Double_t TH1S::GetBinContent(Int_t bin) const
{
   // see convention for numbering bins in TH1::GetBin
   if (fBuffer) ((TH1S*)this)->BufferEmpty();
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (!fArray) return 0;
   return Double_t (fArray[bin]);
}

//______________________________________________________________________________
void TH1S::Reset(Option_t *option)
{
   // Reset.

   TH1::Reset(option);
   TArrayS::Reset();
}

//______________________________________________________________________________
void TH1S::SetBinContent(Int_t bin, Double_t content)
{
   // Set bin content
   // see convention for numbering bins in TH1::GetBin
   // In case the bin number is greater than the number of bins and
   // the timedisplay option is set or the kCanRebin bit is set,
   // the number of bins is automatically doubled to accommodate the new bin

   fEntries++;
   fTsumw = 0;
   if (bin < 0) return;
   if (bin >= fNcells-1) {
      if (fXaxis.GetTimeDisplay()) {
         while (bin >=  fNcells-1)  LabelsInflate();
      } else {
         if (!TestBit(kCanRebin)) {
            if (bin == fNcells-1) fArray[bin] = Short_t (content);
            return;
         }
         while (bin >= fNcells-1)  LabelsInflate();
      }
   }
   fArray[bin] = Short_t (content);
}

//______________________________________________________________________________
void TH1S::SetBinsLength(Int_t n)
{
   // Set total number of bins including under/overflow
   // Reallocate bin contents array

   if (n < 0) n = fXaxis.GetNbins() + 2;
   fNcells = n;
   TArrayS::Set(n);
}

//______________________________________________________________________________
TH1S& TH1S::operator=(const TH1S &h1)
{
   // Operator =

   if (this != &h1)  ((TH1S&)h1).Copy(*this);
   return *this;
}


//______________________________________________________________________________
TH1S operator*(Double_t c1, const TH1S &h1)
{
   // Operator *

   TH1S hnew = h1;
   hnew.Scale(c1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1S operator+(const TH1S &h1, const TH1S &h2)
{
   // Operator +

   TH1S hnew = h1;
   hnew.Add(&h2,1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1S operator-(const TH1S &h1, const TH1S &h2)
{
   // Operator -

   TH1S hnew = h1;
   hnew.Add(&h2,-1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1S operator*(const TH1S &h1, const TH1S &h2)
{
   // Operator *

   TH1S hnew = h1;
   hnew.Multiply(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1S operator/(const TH1S &h1, const TH1S &h2)
{
   // Operator /

   TH1S hnew = h1;
   hnew.Divide(&h2);
   hnew.SetDirectory(0);
   return hnew;
}


//______________________________________________________________________________
//                     TH1I methods
// TH1I : histograms with one int per channel.    Maximum bin content = 2147483647
//______________________________________________________________________________

ClassImp(TH1I)

//______________________________________________________________________________
TH1I::TH1I(): TH1(), TArrayI()
{
   // Constructor.

   fDimension = 1;
   SetBinsLength(3);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1I::TH1I(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
: TH1(name,title,nbins,xlow,xup)
{
   //
   //    Create a 1-Dim histogram with fix bins of type integer
   //    ====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayI::Set(fNcells);

   if (xlow >= xup) SetBuffer(fgBufferSize);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1I::TH1I(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type integer
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayI::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1I::TH1I(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type integer
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayI::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1I::~TH1I()
{
   // Destructor.
}

//______________________________________________________________________________
TH1I::TH1I(const TH1I &h1i) : TH1(), TArrayI()
{
   // Copy constructor.

   ((TH1I&)h1i).Copy(*this);
}

//______________________________________________________________________________
void TH1I::AddBinContent(Int_t bin)
{
   //   -*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //                   ==========================

   if (fArray[bin] < 2147483647) fArray[bin]++;
}

//______________________________________________________________________________
void TH1I::AddBinContent(Int_t bin, Double_t w)
{
   //                   Increment bin content by w
   //                   ==========================

   Int_t newval = fArray[bin] + Int_t(w);
   if (newval > -2147483647 && newval < 2147483647) {fArray[bin] = Int_t(newval); return;}
   if (newval < -2147483647) fArray[bin] = -2147483647;
   if (newval >  2147483647) fArray[bin] =  2147483647;
}

//______________________________________________________________________________
void TH1I::Copy(TObject &newth1) const
{
   // Copy this to newth1

   TH1::Copy(newth1);
}

//______________________________________________________________________________
TH1 *TH1I::DrawCopy(Option_t *option) const
{
   // Draw copy.

   TString opt = option;
   opt.ToLower();
   if (gPad && !opt.Contains("same")) gPad->Clear();
   TH1I *newth1 = (TH1I*)Clone();
   newth1->SetDirectory(0);
   newth1->SetBit(kCanDelete);
   newth1->AppendPad(option);
   return newth1;
}

//______________________________________________________________________________
Double_t TH1I::GetBinContent(Int_t bin) const
{
   // see convention for numbering bins in TH1::GetBin
   if (fBuffer) ((TH1I*)this)->BufferEmpty();
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (!fArray) return 0;
   return Double_t (fArray[bin]);
}

//______________________________________________________________________________
void TH1I::Reset(Option_t *option)
{
   // Reset.

   TH1::Reset(option);
   TArrayI::Reset();
}

//______________________________________________________________________________
void TH1I::SetBinContent(Int_t bin, Double_t content)
{
   // Set bin content
   // see convention for numbering bins in TH1::GetBin
   // In case the bin number is greater than the number of bins and
   // the timedisplay option is set or the kCanRebin bit is set,
   // the number of bins is automatically doubled to accommodate the new bin

   fEntries++;
   fTsumw = 0;
   if (bin < 0) return;
   if (bin >= fNcells-1) {
      if (fXaxis.GetTimeDisplay()) {
         while (bin >=  fNcells-1)  LabelsInflate();
      } else {
         if (!TestBit(kCanRebin)) {
            if (bin == fNcells-1) fArray[bin] = Int_t (content);
            return;
         }
         while (bin >= fNcells-1)  LabelsInflate();
      }
   }
   fArray[bin] = Int_t (content);
}

//______________________________________________________________________________
void TH1I::SetBinsLength(Int_t n)
{
   // Set total number of bins including under/overflow
   // Reallocate bin contents array

   if (n < 0) n = fXaxis.GetNbins() + 2;
   fNcells = n;
   TArrayI::Set(n);
}

//______________________________________________________________________________
TH1I& TH1I::operator=(const TH1I &h1)
{
   // Operator =

   if (this != &h1)  ((TH1I&)h1).Copy(*this);
   return *this;
}


//______________________________________________________________________________
TH1I operator*(Double_t c1, const TH1I &h1)
{
   // Operator *

   TH1I hnew = h1;
   hnew.Scale(c1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1I operator+(const TH1I &h1, const TH1I &h2)
{
   // Operator +

   TH1I hnew = h1;
   hnew.Add(&h2,1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1I operator-(const TH1I &h1, const TH1I &h2)
{
   // Operator -

   TH1I hnew = h1;
   hnew.Add(&h2,-1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1I operator*(const TH1I &h1, const TH1I &h2)
{
   // Operator *

   TH1I hnew = h1;
   hnew.Multiply(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1I operator/(const TH1I &h1, const TH1I &h2)
{
   // Operator /

   TH1I hnew = h1;
   hnew.Divide(&h2);
   hnew.SetDirectory(0);
   return hnew;
}


//______________________________________________________________________________
//                     TH1F methods
// TH1F : histograms with one float per channel.  Maximum precision 7 digits
//______________________________________________________________________________

ClassImp(TH1F)

//______________________________________________________________________________
TH1F::TH1F(): TH1(), TArrayF()
{
   // Constructor.

   fDimension = 1;
   SetBinsLength(3);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1F::TH1F(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
: TH1(name,title,nbins,xlow,xup)
{
   //
   //    Create a 1-Dim histogram with fix bins of type float
   //    ====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayF::Set(fNcells);

   if (xlow >= xup) SetBuffer(fgBufferSize);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1F::TH1F(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type float
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayF::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1F::TH1F(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type float
   //    =========================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayF::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1F::TH1F(const TVectorF &v)
: TH1("TVectorF","",v.GetNrows(),0,v.GetNrows())
{
   // Create a histogram from a TVectorF
   // by default the histogram name is "TVectorF" and title = ""

   TArrayF::Set(fNcells);
   fDimension = 1;
   Int_t ivlow  = v.GetLwb();
   for (Int_t i=0;i<fNcells-2;i++) {
      SetBinContent(i+1,v(i+ivlow));
   }
   TArrayF::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1F::TH1F(const TH1F &h) : TH1(), TArrayF()
{
   // Copy Constructor.

   ((TH1F&)h).Copy(*this);
}

//______________________________________________________________________________
TH1F::~TH1F()
{
   // Destructor.
}

//______________________________________________________________________________
void TH1F::Copy(TObject &newth1) const
{
   // Copy this to newth1.

   TH1::Copy(newth1);
}

//______________________________________________________________________________
TH1 *TH1F::DrawCopy(Option_t *option) const
{
   // Draw copy.

   TString opt = option;
   opt.ToLower();
   if (gPad && !opt.Contains("same")) gPad->Clear();
   TH1F *newth1 = (TH1F*)Clone();
   newth1->SetDirectory(0);
   newth1->SetBit(kCanDelete);
   newth1->AppendPad(option);
   return newth1;
}

//______________________________________________________________________________
Double_t TH1F::GetBinContent(Int_t bin) const
{
   // see convention for numbering bins in TH1::GetBin
   if (fBuffer) ((TH1F*)this)->BufferEmpty();
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (!fArray) return 0;
   return Double_t (fArray[bin]);
}

//______________________________________________________________________________
void TH1F::Reset(Option_t *option)
{
   // Reset.

   TH1::Reset(option);
   TArrayF::Reset();
}

//______________________________________________________________________________
void TH1F::SetBinContent(Int_t bin, Double_t content)
{
   // Set bin content
   // see convention for numbering bins in TH1::GetBin
   // In case the bin number is greater than the number of bins and
   // the timedisplay option is set or the kCanRebin bit is set,
   // the number of bins is automatically doubled to accommodate the new bin

   fEntries++;
   fTsumw = 0;
   if (bin < 0) return;
   if (bin >= fNcells-1) {
      if (fXaxis.GetTimeDisplay()) {
         while (bin >=  fNcells-1)  LabelsInflate();
      } else {
         if (!TestBit(kCanRebin)) {
            if (bin == fNcells-1) fArray[bin] = Float_t (content);
            return;
         }
         while (bin >= fNcells-1)  LabelsInflate();
      }
   }
   fArray[bin] = Float_t (content);
}

//______________________________________________________________________________
void TH1F::SetBinsLength(Int_t n)
{
   // Set total number of bins including under/overflow
   // Reallocate bin contents array

   if (n < 0) n = fXaxis.GetNbins() + 2;
   fNcells = n;
   TArrayF::Set(n);
}

//______________________________________________________________________________
TH1F& TH1F::operator=(const TH1F &h1)
{
   // Operator =

   if (this != &h1)  ((TH1F&)h1).Copy(*this);
   return *this;
}


//______________________________________________________________________________
TH1F operator*(Double_t c1, const TH1F &h1)
{
   // Operator *

   TH1F hnew = h1;
   hnew.Scale(c1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1F operator+(const TH1F &h1, const TH1F &h2)
{
   // Operator +

   TH1F hnew = h1;
   hnew.Add(&h2,1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1F operator-(const TH1F &h1, const TH1F &h2)
{
   // Operator -

   TH1F hnew = h1;
   hnew.Add(&h2,-1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1F operator*(const TH1F &h1, const TH1F &h2)
{
   // Operator *

   TH1F hnew = h1;
   hnew.Multiply(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1F operator/(const TH1F &h1, const TH1F &h2)
{
   // Operator /

   TH1F hnew = h1;
   hnew.Divide(&h2);
   hnew.SetDirectory(0);
   return hnew;
}



//______________________________________________________________________________
//                     TH1D methods
// TH1D : histograms with one double per channel. Maximum precision 14 digits
//______________________________________________________________________________

ClassImp(TH1D)

//______________________________________________________________________________
TH1D::TH1D(): TH1(), TArrayD()
{
   // Constructor.

   fDimension = 1;
   SetBinsLength(3);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1D::TH1D(const char *name,const char *title,Int_t nbins,Double_t xlow,Double_t xup)
: TH1(name,title,nbins,xlow,xup)
{
   //
   //    Create a 1-Dim histogram with fix bins of type double
   //    =====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayD::Set(fNcells);

   if (xlow >= xup) SetBuffer(fgBufferSize);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1D::TH1D(const char *name,const char *title,Int_t nbins,const Float_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type double
   //    =====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayD::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1D::TH1D(const char *name,const char *title,Int_t nbins,const Double_t *xbins)
: TH1(name,title,nbins,xbins)
{
   //
   //    Create a 1-Dim histogram with variable bins of type double
   //    =====================================================
   //           (see TH1::TH1 for explanation of parameters)
   //
   fDimension = 1;
   TArrayD::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1D::TH1D(const TVectorD &v)
: TH1("TVectorD","",v.GetNrows(),0,v.GetNrows())
{
   // Create a histogram from a TVectorD
   // by default the histogram name is "TVectorD" and title = ""

   TArrayD::Set(fNcells);
   fDimension = 1;
   Int_t ivlow  = v.GetLwb();
   for (Int_t i=0;i<fNcells-2;i++) {
      SetBinContent(i+1,v(i+ivlow));
   }
   TArrayD::Set(fNcells);
   if (fgDefaultSumw2) Sumw2();
}

//______________________________________________________________________________
TH1D::~TH1D()
{
   // Destructor.
}

//______________________________________________________________________________
TH1D::TH1D(const TH1D &h1d) : TH1(), TArrayD()
{
   // Constructor.

   ((TH1D&)h1d).Copy(*this);
}

//______________________________________________________________________________
void TH1D::Copy(TObject &newth1) const
{
   // Copy this to newth1

   TH1::Copy(newth1);
}

//______________________________________________________________________________
TH1 *TH1D::DrawCopy(Option_t *option) const
{
   // Draw copy.

   TString opt = option;
   opt.ToLower();
   if (gPad && !opt.Contains("same")) gPad->Clear();
   TH1D *newth1 = (TH1D*)Clone();
   newth1->SetDirectory(0);
   newth1->SetBit(kCanDelete);
   newth1->AppendPad(option);
   return newth1;
}

//______________________________________________________________________________
Double_t TH1D::GetBinContent(Int_t bin) const
{
   // see convention for numbering bins in TH1::GetBin
   if (fBuffer) ((TH1D*)this)->BufferEmpty();
   if (bin < 0) bin = 0;
   if (bin >= fNcells) bin = fNcells-1;
   if (!fArray) return 0;
   return Double_t (fArray[bin]);
}

//______________________________________________________________________________
void TH1D::Reset(Option_t *option)
{
   // Reset.

   TH1::Reset(option);
   TArrayD::Reset();
}

//______________________________________________________________________________
void TH1D::SetBinContent(Int_t bin, Double_t content)
{
   // Set bin content
   // see convention for numbering bins in TH1::GetBin
   // In case the bin number is greater than the number of bins and
   // the timedisplay option is set or the kCanRebin bit is set,
   // the number of bins is automatically doubled to accommodate the new bin

   fEntries++;
   fTsumw = 0;
   if (bin < 0) return;
   if (bin >= fNcells-1) {
      if (fXaxis.GetTimeDisplay()) {
         while (bin >=  fNcells-1)  LabelsInflate();
      } else {
         if (!TestBit(kCanRebin)) {
            if (bin == fNcells-1) fArray[bin] = content;
            return;
         }
         while (bin >= fNcells-1)  LabelsInflate();
      }
   }
   fArray[bin] = content;
}

//______________________________________________________________________________
void TH1D::SetBinsLength(Int_t n)
{
   // Set total number of bins including under/overflow
   // Reallocate bin contents array

   if (n < 0) n = fXaxis.GetNbins() + 2;
   fNcells = n;
   TArrayD::Set(n);
}

//______________________________________________________________________________
TH1D& TH1D::operator=(const TH1D &h1)
{
   // Operator =

   if (this != &h1)  ((TH1D&)h1).Copy(*this);
   return *this;
}

//______________________________________________________________________________
TH1D operator*(Double_t c1, const TH1D &h1)
{
   // Operator *

   TH1D hnew = h1;
   hnew.Scale(c1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1D operator+(const TH1D &h1, const TH1D &h2)
{
   // Operator +

   TH1D hnew = h1;
   hnew.Add(&h2,1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1D operator-(const TH1D &h1, const TH1D &h2)
{
   // Operator -

   TH1D hnew = h1;
   hnew.Add(&h2,-1);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1D operator*(const TH1D &h1, const TH1D &h2)
{
   // Operator *

   TH1D hnew = h1;
   hnew.Multiply(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1D operator/(const TH1D &h1, const TH1D &h2)
{
   // Operator /

   TH1D hnew = h1;
   hnew.Divide(&h2);
   hnew.SetDirectory(0);
   return hnew;
}

//______________________________________________________________________________
TH1 *R__H(Int_t hid)
{
   //return pointer to histogram with name
   //   hid if id >=0
   //   h_id if id <0

   TString hname;
   if(hid >= 0) hname.Form("h%d",hid);
   else         hname.Form("h_%d",hid);
   return (TH1*)gDirectory->Get(hname);
}

//______________________________________________________________________________
TH1 *R__H(const char * hname)
{
   //return pointer to histogram with name hname

   return (TH1*)gDirectory->Get(hname);
}
