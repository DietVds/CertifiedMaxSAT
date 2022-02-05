#!/usr/bin/env bash

# Assumptions
# - Virtual env is activated

ROOT_DIR=$(pwd)

./scripts/build_veripb_src.sh ./src_tmp

# Unrar
cd src_tmp
patch -p1 < $ROOT_DIR/patches/veripb_wcnf.patch

# Build 
pip3 install ./

# Clean up
cd ..
rm -rf src_tmp
