#!/bin/bash

ROOTDIR=/cern/root_v5-34-00-patches
WHERE=.

# common
cp -pv $ROOTDIR/core/base/inc/LinkDef1.h $WHERE
cp -pv $ROOTDIR/core/base/inc/THighlighter.h $WHERE
cp -pv $ROOTDIR/core/base/src/THighlighter.cxx $WHERE

cp -pv $ROOTDIR/graf2d/gpad/inc/TCanvas.h $WHERE
cp -pv $ROOTDIR/graf2d/gpad/src/TCanvas.cxx $WHERE

# graph
cp -pv $ROOTDIR/hist/histpainter/inc/TGraphPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/TGraphPainter.cxx $WHERE

cp -pv $ROOTDIR/hist/hist/inc/TGraph.h $WHERE
cp -pv $ROOTDIR/hist/hist/src/TGraph.cxx $WHERE

# histo
cp -pv $ROOTDIR/hist/histpainter/inc/THistPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/THistPainter.cxx $WHERE

cp -pv $ROOTDIR/hist/hist/inc/TH1.h $WHERE
cp -pv $ROOTDIR/hist/hist/src/TH1.cxx $WHERE
