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
    #unrar x -ad veripb.rar 
    tar xvzf VeriPB-master.tar.gz
    mv VeriPB-master veripb
    
    # Copy source to given directory
    cd $ROOT_DIR
    cp -rf base/veripb $1
    
    # Cleanup 
    rm -rf base/veripb
fi
