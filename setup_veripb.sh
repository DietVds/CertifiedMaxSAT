#!/usr/bin/env bash

# Assumptions
# - Virtual env is activated
# - User has access to veripb repository

# Clone veripb
git clone git@gitlab.com:miao_research/VeriPB.git

# Build 
cd VeriPB
pip3 install ./

# Clean up
cd ..
rm -rf VeriPB
