#!/usr/bin/env bash
#
# Script that extracts extracts VeriPB and builds/installs it
#

ROOT_DIR=$(pwd)

# Unzip
unzip base/VeriPB-master.zip

# Build 
pip3 install ./

# Clean up
cd ..
rm -rf src_tmp