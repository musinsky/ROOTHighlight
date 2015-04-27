#!/bin/bash

ROOTDIR=/cern/root_master
WHERE=.

# graph
cp -pv $ROOTDIR/hist/hist/inc/TVirtualGraphPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/inc/TGraphPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/TGraphPainter.cxx $WHERE

cp -pv $ROOTDIR/hist/hist/inc/TGraph.h $WHERE
cp -pv $ROOTDIR/hist/hist/src/TGraph.cxx $WHERE

# histo
cp -pv $ROOTDIR/hist/hist/inc/TVirtualHistPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/inc/THistPainter.h $WHERE
cp -pv $ROOTDIR/hist/histpainter/src/THistPainter.cxx $WHERE

cp -pv $ROOTDIR/hist/hist/inc/TH1.h $WHERE
cp -pv $ROOTDIR/hist/hist/src/TH1.cxx $WHERE

cp -pv $ROOTDIR/graf3d/gl/inc/TGLHistPainter.h $WHERE
cp -pv $ROOTDIR/graf3d/gl/src/TGLHistPainter.cxx $WHERE
