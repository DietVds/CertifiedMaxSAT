#!/bin/bash
#
# Script that extracts Minisat and QMaxSAT, applies the qmaxsat patch and merges them to create the source code
#

if [ -z $1 ]
then
    echo "Please provide an output path!"
else
    ROOT_DIR=$(pwd)

    # Untar archives
    cd base
    tar xvzf minisat-2.2.0.tar.gz                       #--> no need to specify folder, it is archived like this
    tar xvzf qmaxsat-v0-1.tgz --one-top-level=qmaxsat   #--> need to specifiy subfolder

    # Delete unneeded build files
    rm qmaxsat/0.1/*.o
    rm qmaxsat/0.1/*.or
    rm qmaxsat/0.1/qmaxsat_static
    rm minisat/core/Main.cc
    rm minisat/core/Solver.cc

    # Merge
    cp -rf qmaxsat/mtl minisat
    cp -rf qmaxsat/0.1/* minisat/core

    # Apply qmaxsat fix patch
    cd minisat/core
    patch < $ROOT_DIR/patches/qmaxsat_fix.patch

    # Copy source to given directory
    cd $ROOT_DIR
    cp -rf base/minisat $1

    # Cleanup
    rm -rf base/minisat
    rm -rf base/qmaxsat
fi