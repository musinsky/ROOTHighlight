#!/bin/bash

# last highlight sync with specific commit (or master)
# 09/04/2018
COMMIT=2d8e2cfb7372691ab5e0e823e4830d46c4dbddb8
#COMMIT=master

ROOTDIR=https://raw.githubusercontent.com/root-project/root/$COMMIT
PREFIXDIR=ROOT_$COMMIT

# sort as git
wget $ROOTDIR/graf2d/gpad/inc/TCanvas.h -P $PREFIXDIR
wget $ROOTDIR/graf2d/gpad/src/TCanvas.cxx -P $PREFIXDIR
wget $ROOTDIR/graf3d/gl/inc/TGLHistPainter.h -P $PREFIXDIR
wget $ROOTDIR/graf3d/gl/src/TGLHistPainter.cxx -P $PREFIXDIR
wget $ROOTDIR/hist/hist/inc/TGraph.h -P $PREFIXDIR
wget $ROOTDIR/hist/hist/inc/TH1.h -P $PREFIXDIR
wget $ROOTDIR/hist/hist/inc/TVirtualGraphPainter.h -P $PREFIXDIR
wget $ROOTDIR/hist/hist/inc/TVirtualHistPainter.h -P $PREFIXDIR
wget $ROOTDIR/hist/hist/src/TGraph.cxx -P $PREFIXDIR
wget $ROOTDIR/hist/hist/src/TH1.cxx -P $PREFIXDIR
wget $ROOTDIR/hist/histpainter/inc/TGraphPainter.h -P $PREFIXDIR
wget $ROOTDIR/hist/histpainter/inc/THistPainter.h -P $PREFIXDIR
wget $ROOTDIR/hist/histpainter/src/TGraphPainter.cxx -P $PREFIXDIR
wget $ROOTDIR/hist/histpainter/src/THistPainter.cxx -P $PREFIXDIR
