#!/bin/bash
#
# Script that creates a patch file for the current src folder compared to a freshly built src folder
#

if [ -z $1 ]
then
    echo "Please provide a file name!"
else

    # Build a fresh src
    ./scripts/building/build_src.sh ./src_tmp

    # Create a patch file for the current src dir
    diff -ruN src_tmp src > patches/$1

    # Cleanup
    rm -rf src_tmp
fi