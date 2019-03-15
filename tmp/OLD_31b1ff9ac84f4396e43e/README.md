- info from 2019-03-15

- from https://gist.github.com/musinsky/31b1ff9ac84f4396e43e
- revisions from 2014-10-01 to 2014-10-06

-rw-r--r-- 1 musinsky musinsky   1240 Oct  6  2014 graphtext2.C
-rw-rw-r-- 1 musinsky musinsky   1427 Oct  6  2014 highlight1.C
-rw-rw-r-- 1 musinsky musinsky   2767 Oct  6  2014 highlight2.C
-rw-rw-r-- 1 musinsky musinsky   4295 Oct  6  2014 highlight3.C
-rw-r--r-- 1 musinsky musinsky   3341 Oct  6  2014 quantiles2.C
-rw-rw-r-- 1 musinsky musinsky    769 Mar 15 18:00 README.md
-rw-rw-r-- 1 musinsky musinsky   7272 Oct  6  2014 ROOT_5_6.patch
-rw-rw-r-- 1 musinsky musinsky   3939 Oct  6  2014 ROOT_5.patch
-rw-rw-r-- 1 musinsky musinsky   3225 Oct  6  2014 ROOT_6.patch
-rw-r--r-- 1 musinsky musinsky  87999 Oct  2  2014 TGraph.cxx
-rw-r--r-- 1 musinsky musinsky   9964 Oct  2  2014 TGraph.h
-rw-r--r-- 1 musinsky musinsky 143183 Oct  2  2014 TGraphPainter.cxx
-rw-r--r-- 1 musinsky musinsky   3017 Oct  2  2014 TGraphPainter.h
-rw-r--r-- 1 musinsky musinsky  87830 Sep 28  2014 TGraph.R6.cxx
-rw-r--r-- 1 musinsky musinsky   9668 Sep 28  2014 TGraph.R6.h
-rw-r--r-- 1 musinsky musinsky  88543 Oct  2  2014 TGraph.R6.patched.cxx
-rw-r--r-- 1 musinsky musinsky   9962 Oct  2  2014 TGraph.R6.patched.h
-rw-r--r-- 1 musinsky musinsky   2501 Oct  2  2014 TVirtualGraphPainter.h



- original info

ROOT Graph Highlight
====================

ROOT5 = branch v5-34-00-patches (06.10.2014)
ROOT6 = branch master           (06.10.2014)

---

# diff
## common (ROOT5 and ROOT6)
git diff hist/hist/inc/TVirtualGraphPainter.h hist/histpainter/inc/TGraphPainter.h hist/histpainter/src/TGraphPainter.cxx > ROOT_5_6.patch

## extra ROOT5
git diff hist/hist/inc/TGraph.h hist/hist/src/TGraph.cxx > ROOT_5.patch

## extra ROOT6
git diff hist/hist/inc/TGraph.h hist/hist/src/TGraph.cxx > ROOT_6.patch

---

# patch
## ROOT5
git apply ROOT_5_6.patch
git apply ROOT_5.patch

## ROOT6
git apply ROOT_5_6.patch
git apply ROOT_6.patch
