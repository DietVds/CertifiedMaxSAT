#!/usr/bin/env bash

# Assumptions
# - Virtual env is activated

# Unrar
cd veripb
unrar x -ad veripb.rar
cd veripb

# Build 
pip3 install ./

# Clean up
cd ..
rm -rf veripb
