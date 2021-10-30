#!/usr/bin/env bash

# Assumptions
# - Virtual env is activated

# Unrar
cd veripb
unrar x -ad veripb-54cf2c28b1ebea7292dc618f213878d716644bc3.rar 
cd veripb-54cf2c28b1ebea7292dc618f213878d716644bc3

# Build 
pip3 install ./

# Clean up
cd ..
rm -rf veripb-54cf2c28b1ebea7292dc618f213878d716644bc3 
