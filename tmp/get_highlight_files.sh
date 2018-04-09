#!/bin/bash

ROOTDIR=/cern/root_master
PREFIXDIR=ROOT_highlight

mkdir -p $PREFIXDIR
# sort as git
cp -pv $ROOTDIR/graf2d/gpad/inc/TCanvas.h $PREFIXDIR
cp -pv $ROOTDIR/graf2d/gpad/src/TCanvas.cxx $PREFIXDIR
cp -pv $ROOTDIR/graf3d/gl/inc/TGLHistPainter.h $PREFIXDIR
cp -pv $ROOTDIR/graf3d/gl/src/TGLHistPainter.cxx $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/inc/TGraph.h $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/inc/TH1.h $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/inc/TVirtualGraphPainter.h $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/inc/TVirtualHistPainter.h $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/src/TGraph.cxx $PREFIXDIR
cp -pv $ROOTDIR/hist/hist/src/TH1.cxx $PREFIXDIR
cp -pv $ROOTDIR/hist/histpainter/inc/TGraphPainter.h $PREFIXDIR
cp -pv $ROOTDIR/hist/histpainter/inc/THistPainter.h $PREFIXDIR
cp -pv $ROOTDIR/hist/histpainter/src/TGraphPainter.cxx $PREFIXDIR
cp -pv $ROOTDIR/hist/histpainter/src/THistPainter.cxx $PREFIXDIR
