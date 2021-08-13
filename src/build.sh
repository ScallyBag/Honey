#!/bin/bash
#set -x

###  modify as appropriate for you system
### all builds have added features, 4 opening books can be used, adaptive ply,
### play by FIDE Elo ratings or CCRL Elo ratings
###

### time the compile process
#DATE=$(shell date +"%m/%d/%y")
starttime=`date +%s`


#ARCH="ARCH=x86-32" #ARCH="ARCH=x86-64" #ARCH="ARCH=x86-64-sse" #ARCH="ARCH=x86-64-mmx" #ARCH="ARCH=x86-64-sse2" #ARCH="ARCH=x86-64-ssse3"
#ARCH="ARCH=x86-64-sse41" #ARCH="ARCH=x86-64-modern" #ARCH="ARCH=x86-64-bmi2" #ARCH="ARCH=armv7" #ARCH="ARCH=ppc-32" #ARCH="ARCH=ppc-64comp"

#COMP="COMP=clang"
COMP="COMP=mingw"
#COMP="COMP=gcc"
#COMP="COMP=icc"

#BUILD="profile-build"
BUILD="profile-build"
OS=W
BUILD="build"
function mke() {
CXXFLAGS='' make -j30 $BUILD  $COMP "$@"
}
make clean
#if false; then
#  for ENG in "WEAK=yes"
#    do
#    for ARCH in "x86-64"
#      do
#      mke $ENG ARCH=$ARCH && wait
#      rename 12-R1.exe 12-$OS-$ARCH.exe *.exe
#    done
#  done
#fi
#read

BUILD="profile-build"
#BUILD="build"

function mke() {
CXXFLAGS='' make -j30 $BUILD  $COMP "$@"
}
  #for ENG in "NOIR=yes" "BLAU=yes" "HONEY=yes" "STOCKFISH=yes" "BETH=yes"
  #do
  for ARCH in "x86-64-avx2"
  ##for ARCH in "x86-32"
  ##for ARCH in  "x86-64-avx2"  "x86-64" "x86-64-sse41" "x86-64-modern" "x86-64-bmi2"
  do
    echo $ARCH
    #CPPFLAGS="-march=native"
    mke ARCH=$ARCH && wait
    rename  Honey-14.exe Honey-v14-$ARCH.exe *.exe
    #rename  v13.1-.exe v13.1.exe *.exe
  done
#done

#read # hack to stop script
### The script code belows computes the bench nodes for each version, and updates the Makefile
### with the bench nodes and the date this was run.
echo ""
mv benchnodes.txt benchnodes_old.txt
echo "$( date +'Based on commits through %m/%d/%Y:')">> benchnodes.txt
echo "======================================================">> benchnodes.txt
grep -E 'searched|Nodes/second' *.bench  /dev/null >> benchnodes.txt
echo "======================================================">> benchnodes.txt
sed -i -e  's/^/### /g' benchnodes.txt
#rm *.nodes benchnodes.txt-e
echo "$(<benchnodes.txt)"
sed -i.bak -e '1200,1372d' ../src/Makefile
sed '1199r benchnodes.txt' <../src/Makefile >../src/Makefile.tmp
mv ../src/Makefile.tmp ../src/Makefile
rm *.bench
#strip Black* Blue* Honey* Weak* Stock*

end=`date +%s`
runtime=$((end-starttime))
echo ""
echo Processing time $runtime seconds...
