#!/bin/bash

ROOTDIR=/cern/root_v5-34-00-patches
WHERE=.

# sort as git
cp -pv $ROOTDIR/graf2d/gpad/inc/TCanvas.h $WHERE
cp -pv $ROOTDIR/graf2d/gpad/src/TCanvas.cxx $WHERE
cp -pv $ROOTDIR/graf3d/gl/inc/TGLHistPainter.h $WHERE
cp -pv $ROOTDIR/hist/hist/inc/TGraph.h $WHERE
cp -pv $ROOTDIR/hist/hist/inc/TH1.h $WHERE
cp -pv $ROOTDIR/hist/hist/inc/TVirtualGraphPainter.h $WHERE
cp -pv $ROOTDIR/hist/hist/inc/TVirtualHistPainter.h $WHERE
cp -pv $ROOTDIR/hist/hist/src/TGraph.cxx $WHERE
cp -pv $ROOTDIR/hist/hist/src/TH1.cxx $WHERE
cp -pv $ROOTDIR/hist/histpainter/inc/TGraphPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/inc/THistPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/TGraphPainter.cxx $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/THistPainter.cxx $WHERE
